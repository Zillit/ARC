// C++ version!
// g++ graphics.cpp ../../common/*.c ../../common/Linux/*.c -lGL -o mainGraphics -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm -fpermissive
//note, add -fpermissive in order to allow implicid conversion from void* to whatever, like in C programming

// You *might* need to compile C++ with g++, and then this should do it:
// g++ graphics.cpp -c -o mainGraphics.o -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES
// gcc mainGraphics.o ../../common/*.c ../../common/Linux/*.c -lGL -o perspectivebunny -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm

#ifdef __APPLE__
#include <OpenGL/gl3.h>
// linking hint for Lightweight IDE
//uses framework Cocoa
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "defines.h"
#include "ModelObj.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <cstdint>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <cstring>
#include <zmq.hpp>
#include <sstream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n) Sleep(n)
#endif
#include <zmq.hpp>
#include <thread>
#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

using namespace std;
using namespace OPENGL;

//For test model. It rotate to indicate running program.
Model *m;

// Reference to shader program
GLuint program, program_terrain;
GLuint tex_terrain;

GLfloat tmpLastX, tmpLastY;
struct cameraSample;
Model *marchingSquereModel;

GLfloat FLOOR_RANGE = 3, FLOOR_FALLOFF = 0.1;
GLuint FLOOR_DEPT_RES = 128, FLOOR_WIDTH_RES = 128;
GLuint MAP_MIDDLE_X = FLOOR_WIDTH_RES / 2;
GLuint PLANNING_MAX_LEFT = FLOOR_WIDTH_RES / 3;
GLuint PLANNING_MAX_RIGHT = FLOOR_WIDTH_RES - PLANNING_MAX_LEFT;
GLuint PLANNING_DEPTH = 120;
GLfloat MINIMUM_ALLOWED_HIGHT_OF_FLOOR_TO_BE_CONSIDERED_FREE = 0.5;
GLuint MAXIMUM_LADAR_POINTS = 400;
GLfloat LADAR_SCALER = 5;
GLint MAX_SPEED = 5;
GLint MIN_SPEED = 0;
GLint MIN_THETA = -47;
GLint MAX_THETA = 47;

boost::circular_buffer<pair<GLfloat, GLfloat>> ladarPoints(MAXIMUM_LADAR_POINTS);
//Required to provide thread safty
boost::mutex ladarPointsMutex;
//Vectors for setting up the camera
vec3 CAM_POS{0, 5, 40}, TARGET{0, 0, 25}, UP{0, 1, 0};
//Matrix for the floor model.
mat4 modelCoords;
GLfloat PI = 3.1415927;
//-70 seems good to compensate for what is sent.
GLfloat LADAR_ANGLE_OFFSET = -70;//(-0.5)*PI/180; //LADAR does consider straigth ahead to be angle 0.
//Declaration of thread.
thread first;
//Rotation of a model, to show if the program is working
int rotation = 0;
//Request number to identify what command is sent.
int REQUEST_NUMBER = 0;
zmq::context_t context(1);

//  Socket to talk to server
zmq::socket_t subscriber(context, ZMQ_SUB);
zmq::socket_t requester(context, ZMQ_REQ);

//Class incapsulating 
class CarPilot
{
  public:
    CarPilot(){};
    ~CarPilot() = default;
    void planRoad();
    void changeSpeed(int dV);
    void changeDirection(int dTheta);
    void setSpeed(int newSpeed, int newTheta = 0)
    {
        speed = newSpeed;
        theta = newTheta;
    };
    void carTick();
    void setCurrentSpeedBasedOnSensors(float speed){}
    void sendMessage(string message);
    static char *s_recv(void *socket);
    static int s_send(void *socket, char *string);
    void paintPlan(Model *map);
    void toggleAutoPilot() {auto_pilot=!auto_pilot;};
    void changeDepthOfThetaSearch(int delta) {depth_of_theta_search=std::max(depth_of_theta_search+delta,1);};
    void disableAutoPilot() {auto_pilot=false;};
    void printVariables();

  private:
    string address = "STYROR ";
    string instruction = "";
    int speed = 0;
    int theta = 0;
    float current_speed=0;
    bool update_car_pilot = false;
    int depth_of_theta_search = 1;
    std::chrono::time_point<std::chrono::system_clock> clock_last = chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_between_updates = chrono::milliseconds(200);
    std::vector<pair<GLint, GLint>> planned_path;
    GLfloat con_of_path_search_increase = 2;
    bool auto_pilot=false;
};
void CarPilot::printVariables()
{
    cout << "speed: " << speed << " theta: " << theta << " depth_of_theta_search: ";
    cout << depth_of_theta_search << endl;
}
void CarPilot::changeSpeed(int dV)
{
    speed = std::min(std::max(speed += dV, MIN_SPEED), MAX_SPEED);
    update_car_pilot = true;
}
void CarPilot::changeDirection(int dTheta)
{
    theta = std::min(std::max(theta += dTheta, MIN_THETA), MAX_THETA);
    update_car_pilot = true;
}
void modifyFloorY(GLint, GLint, GLfloat, Model *, bool);
//Updates driving instruction and keep in contact with the hardware
void CarPilot::carTick()
{
    if(auto_pilot)
    {
        int counter=0;
        int deltaTheta=0;
        for(auto it : planned_path)
        {
            if(counter > depth_of_theta_search)
            {
                break;
            }
            deltaTheta+=it.first+it.second-MAP_MIDDLE_X;
            counter++;
        }
        changeDirection(deltaTheta);
        setSpeed(MAX_SPEED, theta);
    }
    auto clock_now = chrono::high_resolution_clock::now();
    std::chrono::duration<double> dTime = clock_now - clock_last;
    if (update_car_pilot && auto_pilot)
    {
        planRoad();
        sendMessage(address + " " + to_string(speed) + " " + to_string(theta));
        // cout << address << " " << to_string(speed) <<" " << to_string(theta);
        update_car_pilot = false;
        clock_last = chrono::high_resolution_clock::now();
    }
    else if (dTime > duration_between_updates)
    {
        planRoad();
        if (theta > 0)
            theta = 0;
        else if (theta < 0)
            theta = 0;
        sendMessage(address + " " + to_string(speed) + " " + to_string(theta));
        clock_last = chrono::high_resolution_clock::now();
        // cout << address << " " << to_string(speed) <<" " << to_string(theta);
    }
}
static char *CarPilot::s_recv(void *socket)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strndup(buffer, sizeof(buffer) - 1);
}
static int CarPilot::s_send(void *socket, char *string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}
void CarPilot::sendMessage(string message)
{
    char *charBuf = message.c_str();
    s_send(requester, charBuf);
    char *response = s_recv(requester);
    istringstream iss(response);
    iss >> address >> instruction;
    // cout << "Address: " << address << " Instruction: " << instruction << ", to message: " << message << ", as request number: " << REQUEST_NUMBER << endl;
    // free(response);
    // free(charBuf);
    REQUEST_NUMBER++;
}
GLfloat getFloorY(GLint, GLint, Model *);
//Will give a representtation of the planed path on the map
void CarPilot::paintPlan(Model *map)
{
    int endDepth=planned_path.size();
    for (int iter = 1; iter < endDepth; iter++)
    {
        // cout << iter << "start: " << planned_path.at(iter).first << " and width: " << planned_path.at(iter).second << endl;
        modifyFloorY(planned_path.at(iter).first + planned_path.at(iter).second, FLOOR_DEPT_RES-iter, std::min(MAX_SPEED,endDepth-iter), marchingSquereModel, false);
        modifyFloorY(planned_path.at(iter).first + planned_path.at(iter).second + 1, FLOOR_DEPT_RES-iter, std::min(MAX_SPEED,endDepth-iter), marchingSquereModel, false);
        modifyFloorY(planned_path.at(iter).first + planned_path.at(iter).second - 1, FLOOR_DEPT_RES-iter, std::min(MAX_SPEED,endDepth-iter), marchingSquereModel, false);
    }
}
//Generate a clear path based on the information on the marching squere
void CarPilot::planRoad()
{
    int xStart, xEnd;
    planned_path.clear();
    //For initilizing the algoritm.
    pair<int, int> clearSpace = {MAP_MIDDLE_X-2, 4};
    planned_path.push_back(clearSpace);
    for (int depthIterator = 1; depthIterator < PLANNING_DEPTH; depthIterator++)
    {
        xStart = std::max((int)PLANNING_MAX_LEFT, (int)(MAP_MIDDLE_X - depthIterator * con_of_path_search_increase-3));
        xEnd = std::min((int)PLANNING_MAX_RIGHT, (int)(MAP_MIDDLE_X + depthIterator * con_of_path_search_increase+3));
        int xFirstOpenSpace = -1;
        int openSpaceDistance = -1;
        bool hasFoundOpenSpace = false;
        for (int widthIterator = xStart; widthIterator < xEnd; widthIterator++)
        {
            if (getFloorY(widthIterator, FLOOR_DEPT_RES-depthIterator, marchingSquereModel) < MINIMUM_ALLOWED_HIGHT_OF_FLOOR_TO_BE_CONSIDERED_FREE)
            {
                hasFoundOpenSpace = true;
                xFirstOpenSpace = widthIterator;
                while ((getFloorY(widthIterator, FLOOR_DEPT_RES-depthIterator, marchingSquereModel) < MINIMUM_ALLOWED_HIGHT_OF_FLOOR_TO_BE_CONSIDERED_FREE) && (widthIterator < xEnd))
                {
                    widthIterator++;
                    openSpaceDistance++;
                }
            }
            if ((clearSpace.second < openSpaceDistance) && (planned_path.back().first<(xFirstOpenSpace+openSpaceDistance)) && ((planned_path.back().first+planned_path.back().second)>xFirstOpenSpace))
            {
                clearSpace = {xFirstOpenSpace, openSpaceDistance};
            }
        }
        if (hasFoundOpenSpace)
        {
            clearSpace.second=clearSpace.second/2;
            planned_path.push_back(clearSpace);
        }
        else
        {
            //Destroy a few parts of the planned path in order to avoid collision.
            for(int i=0;i<4;i++)
            {
                planned_path.pop_back();
            }
            return;
        }
        hasFoundOpenSpace = false;
    }
}
CarPilot car;
//Converts data into x and z coordinates
pair<GLfloat, GLfloat> generateLadarPointFromInputData(GLfloat distance, GLfloat angle)
{
    return pair<GLfloat, GLfloat>(distance * cos(angle), -distance * sin(angle));
}
//For graphical effects
vec3 GenerateNormalFromPoints(GLfloat Zleft, GLfloat Zright, GLfloat Zupright, GLfloat Zdownleft, GLfloat Zup, GLfloat Zdown, int FLOOR_WIDTH_RES, int FLOOR_DEPT_RES)
{
    vec3 temp;
    temp.x = (2 * (Zleft - Zright) - Zupright + Zdownleft + Zup - Zdown);
    temp.y = 6.0;
    temp.z = (2 * (Zdown - Zup) + Zupright + Zdownleft - Zup - Zleft);
    return temp;
}
//Make sure that the point is within the distance to be painted
bool pairInRange(pair<GLint, GLint> p)
{
    return (p.first > 0 && p.first < FLOOR_WIDTH_RES && p.second > 0 && p.second < FLOOR_DEPT_RES);
}
//For making the hight bubbles. The range can be addaptedd as to making sure the car is not allowed to drive to close to the walls 
void findSurroundingCoords(vec3 coords, GLuint range, GLfloat falloff, map<pair<GLfloat, GLfloat>, GLfloat> &mapObj)
{
    GLuint dubbleRange = range * range;
    int x = coords.x + 0.5;
    int z = coords.z + 0.5;
    for (int xIt = 0; xIt <= range; xIt++)
    {
        for (int zIt = 0; zIt <= range; zIt++)
        {
            GLfloat distance = xIt * xIt + zIt * zIt;
            if (distance < dubbleRange)
            {
                pair<GLint, GLint> mapObjPair{xIt + x, zIt + z};
                GLfloat val = 1 + 10 / (1 + falloff * distance);
                if (pairInRange(mapObjPair))
                    mapObj[mapObjPair] = max(val, mapObj[mapObjPair]);

                mapObjPair = {-xIt + x, zIt + z};
                if (pairInRange(mapObjPair))
                    mapObj[mapObjPair] = max(val, mapObj[mapObjPair]);

                mapObjPair = {xIt + x, -zIt + z};
                if (pairInRange(mapObjPair))
                    mapObj[mapObjPair] = max(val, mapObj[mapObjPair]);

                mapObjPair = {-xIt + x, -zIt + z};
                if (pairInRange(mapObjPair))
                    mapObj[mapObjPair] = max(val, mapObj[mapObjPair]);
            }
        }
    }
}
//Resets the squere
void clearFloorY(Model *m)
{
    int max = FLOOR_WIDTH_RES * FLOOR_DEPT_RES;
    for (int iter = 0; iter < max; iter++)
    {
        m->vertexArray[iter * 3 + 1] = 0;
    }
}
//Function for checking the hight of a vertices 
GLfloat getFloorY(GLint x, GLint z, Model *m)
{
    return m->vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1];
}
void modifyFloorY(GLint x, GLint z, GLfloat y, Model *m, bool additive = false)
{
    if (additive)
        m->vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] += y;
    else
        m->vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = y;
}
void generateNormalsInTerrain(Model *tm)
{
    for (auto x = 1; x < FLOOR_WIDTH_RES; x++)
        for (auto z = 2; z < FLOOR_DEPT_RES; z++)
        {
            vec3 temp = GenerateNormalFromPoints(m->vertexArray[(-FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 m->vertexArray[(FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 m->vertexArray[(FLOOR_WIDTH_RES + FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 m->vertexArray[(-FLOOR_WIDTH_RES - FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 m->vertexArray[(FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 m->vertexArray[(-FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1], FLOOR_WIDTH_RES, FLOOR_DEPT_RES);
            // Normal vectors. You need to calculate these.
            tm->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 0] = temp.x;
            tm->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = temp.y;
            tm->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 2] = temp.z;
        }
}
Model *generateFlatTerrrain()
{
    GLuint vertexCount = FLOOR_DEPT_RES * FLOOR_WIDTH_RES;
    GLuint triangleCount = (FLOOR_DEPT_RES - 1) * (FLOOR_WIDTH_RES - 1) * 2;
    GLuint vertexArraySize = sizeof(GLfloat) * 3 * vertexCount;
    GLuint texCoordArraySize = sizeof(GLfloat) * 2 * vertexCount;
    GLuint indexArraySize = sizeof(GLfloat) * 3 * triangleCount;
    GLfloat vertexArray[vertexArraySize];
    GLfloat normalArray[vertexArraySize];
    GLfloat texCoordArray[texCoordArraySize];
    GLuint indexArray[indexArraySize];
    for (auto x = 0; x < FLOOR_WIDTH_RES; x++)
        for (auto z = 0; z < FLOOR_DEPT_RES; z++)
        {
            // Vertex array. You need to scale this properly
            vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 0] = x / 1.0;
            vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = 0;
            vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 2] = z / 1.0;
            // Texture coordinates. You may want to scale them.
            texCoordArray[(x + z * FLOOR_WIDTH_RES) * 2 + 0] = x; // (float)x / FLOOR_WIDTH_RES;
            texCoordArray[(x + z * FLOOR_WIDTH_RES) * 2 + 1] = z; // (float)z / FLOOR_DEPT_RES;
        }
    for (auto x = 1; x < FLOOR_WIDTH_RES; x++)
        for (auto z = 2; z < FLOOR_DEPT_RES; z++)
        {
            vec3 temp = GenerateNormalFromPoints(vertexArray[(-FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 vertexArray[(FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 vertexArray[(FLOOR_WIDTH_RES + FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 vertexArray[(-FLOOR_WIDTH_RES - FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 vertexArray[(FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                                 vertexArray[(-FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1], FLOOR_WIDTH_RES, FLOOR_DEPT_RES);
            // Normal vectors. You need to calculate these.
            normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 0] = temp.x;
            normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = temp.y;
            normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 2] = temp.z;
        }
    for (auto x = 0; x < FLOOR_WIDTH_RES - 1; x++)
        for (auto z = 0; z < FLOOR_DEPT_RES - 1; z++)
        {
            // Triangle 1
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 0] = x + z * FLOOR_WIDTH_RES;
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 1] = x + (z + 1) * FLOOR_WIDTH_RES;
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 2] = x + 1 + z * FLOOR_WIDTH_RES;
            // Triangle 2
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 3] = x + 1 + z * FLOOR_WIDTH_RES;
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 4] = x + (z + 1) * FLOOR_WIDTH_RES;
            indexArray[(x + z * (FLOOR_WIDTH_RES - 1)) * 6 + 5] = x + 1 + (z + 1) * FLOOR_WIDTH_RES;
        }

    // End of terrain generation

    // Create Model and upload to GPU:

    Model *model = LoadDataToModel(
        &vertexArray[0],
        &normalArray[0],
        &texCoordArray[0],
        NULL,
        &indexArray[0],
        vertexCount,
        (int)triangleCount * 3);

    return model;
}
void paintLadarPoints(boost::circular_buffer<pair<GLfloat, GLfloat>> &ladarPoints, mat4 translation, Model *m)
{
    //Lock for thread saftey
    boost::lock_guard<boost::mutex> guard(ladarPointsMutex);
    map<pair<GLfloat, GLfloat>, GLfloat> tempMap;
    for (it : ladarPoints)
    {
        pair<GLfloat, GLfloat> temp{it.first - translation.m[3], +it.second - translation.m[11]};
        if (pairInRange(temp))
            findSurroundingCoords(vec3(temp.first, 0, temp.second), 5, 0.1, tempMap);
        // modifyFloorY(temp.first, temp.second, it.second, marchingSquereModel);
    }
    for (it : tempMap)
    {
        modifyFloorY(it.first.first, it.first.second, it.second, m);
    }
    tempMap.clear();
}
//C based function for sending and reseving instructions.
static char *s_recv(void *socket)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strndup(buffer, sizeof(buffer) - 1);
}
static int s_send(void *socket, char *string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}
void sendMessage(string message)
{
    char *charBuf = message.c_str();
    s_send(requester, charBuf);
    char *response = s_recv(requester);
    string address, instruction;
    istringstream iss(response);
    iss >> address >> instruction;
    // cout << "Address: " << address << " Instrucion: " << instruction << ", to message: " << message << ", as request number: " << REQUEST_NUMBER << endl;
    REQUEST_NUMBER++;
}
void updateCamera(vec3, vec3);
//Initolasation of all the OpenGL function and models.
void init(void)
{
    // GL inits
    glClearColor(0.2, 0.2, 0.3, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_TRUE);
    printError("GL inits");
    modelCoords = IdentityMatrix();
    modelCoords = T(-64, 0, -128);

    projectionMatrix = frustum(LEFT_, RIGHT_, BOTTOM_, TOP_, NEAR_, FAR_);
    rotationMatrix = IdentityMatrix();
    total = IdentityMatrix();
    // camMatrix = lookAt(0, 5, 15, 0, 0, 0, 0, 1, 0); //T(0,0,-2);
    updateCamera(CAM_POS,TARGET);
    // Load and compile shader
    program = loadShaders("persp.vert", "persp.frag");
    m = LoadModelPlus("bunny.obj");
    program_terrain = loadShaders("terrain.vert", "terrain.frag");
    printError("init shader");
    glUseProgram(program_terrain);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program_terrain, "tex"), 0); // Texture unit 0
    LoadTGATextureSimple("maskros512.tga", &tex_terrain);
    glBindTexture(GL_TEXTURE_2D, tex_terrain);

    // End of upload of geometry
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUseProgram(program_terrain);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    printError("init arrays");
}
//For adding data from LADAR input
void ladarPointsAdd(pair<GLfloat, GLfloat> point)
{
    boost::lock_guard<boost::mutex> guard(ladarPointsMutex);
    ladarPoints.push_back(point);
}
//Thread for listening to the publicer. 
void fetchLADARPoints()
{
    string instruction;
    int angle, distance;
    zmq::message_t update;
    while (true)
    {
        while (subscriber.recv(&update))
        {
            std::istringstream iss(static_cast<char *>(update.data()));
            iss >> angle >> distance;
            GLfloat modDistance = distance / LADAR_SCALER;
            GLfloat modAngle = angle - LADAR_ANGLE_OFFSET;
            // cout << "modAngle: " << modAngle << " modDistance: " << modDistance << endl;
            //Make sure that we do not care about data that is to close to the machine
            if (modDistance > 5 && modDistance < 150) //&& 0 < modAngle && (angle - LADAR_ANGLE_OFFSET) < 180)
            {
                ladarPointsAdd(generateLadarPointFromInputData(distance / LADAR_SCALER, (angle - LADAR_ANGLE_OFFSET) * PI / 180));
            }
        }
        this_thread::sleep_for(chrono::microseconds(500));
    }
}
//Main loop of the program, paint all the things on screen.
void display(void)
{
    //OpenGL for clearing things from the past
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Clear old stuff from the Marching squere
    clearFloorY(marchingSquereModel);
    paintLadarPoints(ladarPoints, modelCoords, marchingSquereModel);
    //Do the driving based on current state.
    car.carTick();
    car.paintPlan(marchingSquereModel);
    //Make sure that the data is put into the graphic card.
    ReloadModelData(marchingSquereModel);
    printError("pre display");
    //variable for the rotation of the model
    a += 0.05;
    //Pain the squere and then add it all in the right way to the graphic card.
    glUseProgram(program_terrain);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_terrain);
    //Put the coordinates for the model into view space
    total = modelCoords * camMatrix;
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(marchingSquereModel, program_terrain, "inPosition", "inNormal", "inTexCoord");
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    mat4 rotationMatrix = Rz(a / 3) * Ry(a);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");
    printError("display");
    glutSwapBuffers();
}

void setUpCamera()
{
    void cameraManipulationInput(unsigned char, int, int);
    //Setup for the keyboard interrupt function
    glutKeyboardFunc(cameraManipulationInput);
}
//Function for uppdating camera
void updateCamera(vec3 pos, vec3 target)
{
    camMatrix = lookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, UP.x, UP.y, UP.z);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUseProgram(program_terrain);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "camMatrix"), 1, GL_TRUE, camMatrix.m);
}
void cameraManipulationInput(unsigned char key, int x, int y)
{
    //Simple switch-case for different keyboard keys
    switch (key)
    {
    case 't':
        car.toggleAutoPilot();
        break;
    // case 'c':
    //     system("ssh -t -p 4444 arc@nhikim91.ddns.net ssh -p 2222 pi@localhost");
    //     break;
    case '0':
        LADAR_SCALER += 0.5;
        cout << LADAR_SCALER << endl;
        break;
    case '9':
        LADAR_SCALER = std::max(LADAR_SCALER - 0.5, 1.0);
        break;
    case '5':
        MAX_SPEED++;
        cout << MAX_SPEED << endl;
        break;
    case '4':
        MAX_SPEED--;
        break;
    case '6':
        car.changeDepthOfThetaSearch(-1);
        break;
    case '7':
        car.changeDepthOfThetaSearch(+1);
        break;
    case '8':
        car.printVariables();
        break;
    case 'w':
        CAM_POS += vec3(0, 0, -1);
        TARGET += vec3(0, 0, -1);
        updateCamera(CAM_POS, TARGET);
        break;
    case 's':
        CAM_POS += vec3(0, 0, 1);
        TARGET += vec3(0, 0, 1);
        updateCamera(CAM_POS, TARGET);
        cout << "camera pos " << CAM_POS.z << " target " << TARGET.z << endl;
        break;
    case 'd':
        CAM_POS += vec3(1, 0, 0);
        TARGET += vec3(1, 0, 0);
        updateCamera(CAM_POS, TARGET);
        break;
    case 'a':
        CAM_POS += vec3(-1, 0, 0);
        TARGET += vec3(-1, 0, 0);
        updateCamera(CAM_POS, TARGET);
        break;
    case 'i':
        //Manual drive increase speed
        car.changeSpeed(2);
        break;
    case 'k':
        car.changeSpeed(-5);
        break;
    case 'j':
        //Manual tur left
        car.changeDirection(-10);
        break;
    case 'l':
        //Manual tur right
        car.changeDirection(10);
        break;
    case 'b':
        //Reset camera to start values
        CAM_POS = {0, 5, 40};
        TARGET = {0, 0, 25};
        updateCamera(CAM_POS, TARGET);
        break;
    case 'q':
        //Make sure to turn of everyting and put it into standby mode
        sendMessage("STYROR 0 0");
        car.disableAutoPilot();
        car.setSpeed(0, 0);
        break;
    // case 'v':
    //     break;
    default:
        break;
    }
}
//Exit vector at interrupt
void cleanupSocketAtExit()
{
    //Make sure that the car is disabled without User.
    sendMessage("STYROR 0 0");
    car.disableAutoPilot();
    car.setSpeed(0, 0);
    //Closes connections
    zmq_close(&subscriber);
    zmq_close(&requester);
    zmq_term(&context);
    //Makes sure that all threads are terminated.
    terminate();
}
int main(int argc, char *argv[])
{
    CarPilot car;
    atexit(cleanupSocketAtExit);
    thread first(fetchLADARPoints);
    subscriber.connect("tcp://localhost:2555");
    requester.connect("tcp://localhost:2550");
    cout << "test after connect" << endl;
    //  Subscribe to id, is nothing
    const char *filter = (argc > 1) ? argv[1] : "";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

//Function for correctly setting up OpenGL
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitWindowSize(1200, 1200);
    glutCreateWindow(WINDOW_NAME.c_str());
    glutDisplayFunc(display);
    glutRepeatingTimer(20);
    init();
    setUpCamera();
    vector<cameraSample> testVector{};
    for (int it = 0; it < 2000; it++)
    {
        cameraSample temp{rand() % FLOOR_DEPT_RES, rand() % FLOOR_DEPT_RES};
        testVector.push_back(temp);
    }
    marchingSquereModel = generateFlatTerrrain();
    paintLadarPoints(ladarPoints, modelCoords, marchingSquereModel);
    glutMainLoop();
}
