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
// #include "camera.cpp"
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
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n) Sleep(n)
#endif
// #include <zmq.h>
#include <zmq.hpp>
#include <sstream>
#include <thread>
#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

using namespace std;
using namespace OPENGL;

Model *m, *skyBox;

// Reference to shader program
GLuint program, program_skybox, program_terrain;
GLuint tex_sky, tex_terrain;

GLfloat tmpLastX, tmpLastY;
struct cameraSample;

ModelObj *testClass;
map<pair<GLint, GLint>, GLfloat> testMap;
Model *testM;

GLfloat FLOOR_RANGE = 3, FLOOR_FALLOFF = 0.1;
GLuint FLOOR_DEPT_RES = 128, FLOOR_WIDTH_RES = 128;
GLuint MAXIMUM_LADAR_POINTS = 800;
GLfloat LADAR_SCALER=10;
GLint MAX_SPEED=10;
GLint MIN_SPEED=-10;
GLint MIN_THETA=-30;
GLint MAX_THETA=30;

boost::circular_buffer<pair<GLfloat,GLfloat>> ladarPoints(MAXIMUM_LADAR_POINTS);
boost::mutex ladarPointsMutex;
vec3 CAM_POS{0, 5, 15}, TARGET{0, 0, 0}, UP{0, 1, 0};
mat4 modelCoords;
GLfloat modelAngleY = 0;
thread first;
int rotation = 0;
int REQUEST_NUMBER=0;
    zmq::context_t context (1);

    //  Socket to talk to server
    zmq::socket_t subscriber (context, ZMQ_SUB);
    zmq::socket_t requester (context, ZMQ_REQ);

class CarPilot
{
public:
    // CarPilot(zmq::socket_t *requester):requester(requester){};
    CarPilot(){};
    ~CarPilot()=default;
    void changeSpeed(int dV);
    void changeDirection(int dTheta);
    void carTick();
    void sendMessage(string message);
    static char* s_recv(void *socket);
    static int s_send(void *socket, char* string);
private:
    // zmq::socket_t* requester;
    string address="STYROR ";
    string instruction="";
    int speed=0;
    int theta=0;
    bool update_car_pilot=false;
};

void CarPilot::changeSpeed(int dV)
{
    update_car_pilot=true;
    speed=std::min(std::max(speed+=dV, MIN_SPEED),MAX_SPEED);
}

void CarPilot::changeDirection(int dTheta)
{
    update_car_pilot=true;
    theta=std::min(std::max(theta+=dTheta, MIN_THETA),MAX_THETA);
}

void CarPilot::carTick()
{
    // if (update_car_pilot)
    // {
        sendMessage(address + to_string(speed) + " " + to_string(theta));
        // cout << address << " " << to_string(speed) <<" " << to_string(theta);
        update_car_pilot=false;
    // }
    // else
    // {
    //     if(speed>0)
    //         speed--;
    //     else if(speed<0)
    //         speed++;
    //     if(theta>0)
    //         theta--;
    //     else if(theta<0)
    //         theta++;
    //     sendMessage(address + to_string(speed) + " " + to_string(theta));
    //     // cout << address << " " << to_string(speed) <<" " << to_string(theta);
    // }
}
// void CarPilot::sendMessage(string message)
// {
//         string reply_message="";
//         zmq::message_t request(message.size());
//         memcpy(request.data(),&message,message.size());
//         cout << "Sending test number: " << REQUEST_NUMBER << "..." << endl;
//         requester.send (request);
//         zmq::message_t reply;
//         requester.recv(&reply);
//         istringstream iss(static_cast<char*>(reply.data()));
//         iss >> reply_message;
//         cout << "Recived: " << reply_message << ", to message: " << message << ", as request number: "<< REQUEST_NUMBER << endl;
//         REQUEST_NUMBER++;
// }
static char* CarPilot::s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strndup (buffer, sizeof(buffer) - 1);
}
static int CarPilot::s_send (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}
void CarPilot::sendMessage(string message)
{
        char* charBuf=message.c_str();
        s_send(requester,charBuf);
        char* response=s_recv(requester);
        istringstream iss(response);
        iss >> address >> instruction;
        cout << "Address: " << address << " Instruction: " << instruction << ", to message: " << message << ", as request number: "<< REQUEST_NUMBER << endl;
        // free(response);
        // free(charBuf);
        REQUEST_NUMBER++;
}
CarPilot car;
pair<GLfloat, GLfloat> generateLadarPointFromInputData(GLfloat distance, GLfloat angle)
{
    return pair<GLfloat, GLfloat>(distance * cos(angle*3.1415927/180 - modelAngleY),-distance * sin(angle*3.1415927/180 - modelAngleY));
}

vec3 GenerateNormalFromPoints(GLfloat Zleft, GLfloat Zright, GLfloat Zupright, GLfloat Zdownleft, GLfloat Zup, GLfloat Zdown, int FLOOR_WIDTH_RES, int FLOOR_DEPT_RES)
{
    vec3 temp;
    temp.x = (2 * (Zleft - Zright) - Zupright + Zdownleft + Zup - Zdown);
    temp.y = 6.0;
    temp.z = (2 * (Zdown - Zup) + Zupright + Zdownleft - Zup - Zleft);
    return temp;
}
bool pairInRange(pair<GLint, GLint> p)
{
    return (p.first > 0 && p.first < FLOOR_WIDTH_RES && p.second > 0 && p.second < FLOOR_DEPT_RES);
}
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
void clearFloorY(Model *m)
{
    int max = FLOOR_WIDTH_RES * FLOOR_DEPT_RES;
    for (int iter = 0; iter < max; iter++)
    {
        m->vertexArray[iter * 3 + 1] = 0;
    }
    ReloadModelData(m);
}
void modifyFloorY(GLint x, GLint z, GLfloat y, Model *m, bool additive = false)
{
    if (additive)
        m->vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] += y;
    else
        m->vertexArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = y;
    if (x > 1 && x < FLOOR_WIDTH_RES && z > 1 && z < FLOOR_DEPT_RES)
    {
        vec3 temp = GenerateNormalFromPoints(m->vertexArray[(-FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                             m->vertexArray[(FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                             m->vertexArray[(FLOOR_WIDTH_RES + FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                             m->vertexArray[(-FLOOR_WIDTH_RES - FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                             m->vertexArray[(FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
                                             m->vertexArray[(-FLOOR_DEPT_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1], FLOOR_WIDTH_RES, FLOOR_DEPT_RES);
        m->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 0] = temp.x;
        m->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 1] = temp.y;
        m->normalArray[(x + z * FLOOR_WIDTH_RES) * 3 + 2] = temp.z;
    }
    //  ReloadModelData(m);
}
// void TriangleGeometric::generateTriangleMesh(float length, float width, int FLOOR_DEPT_RES, int FLOOR_WIDTH_RES, float scaler = 1)
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
    // GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
    // GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
    // GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
    // GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount * 3);
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
    boost::lock_guard<boost::mutex> guard(ladarPointsMutex);
    map<pair<GLfloat, GLfloat>, GLfloat> tempMap;

    for (it : ladarPoints)
    {
        pair<GLfloat, GLfloat> temp{it.first-translation.m[3], + it.second-translation.m[11]};
        if (pairInRange(temp))
            findSurroundingCoords(vec3(temp.first, 0, temp.second), 5, 0.1, tempMap);
        // modifyFloorY(temp.first, temp.second, it.second, testM);
    }
    for (it : tempMap)
    {
        modifyFloorY(it.first.first, it.first.second, it.second, m);
    }
    tempMap.clear();
    ReloadModelData(m);
}
static char* s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strndup (buffer, sizeof(buffer) - 1);
}
static int s_send (void *socket, char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}
void sendMessage(string message)
{
        char* charBuf=message.c_str();
        s_send(requester,charBuf);
        char* response=s_recv(requester);
        string address, instruction;
        istringstream iss(response);
        iss >> address >> instruction;
        cout << "Address: " << address << " Instrucion: " << instruction << ", to message: " << message << ", as request number: "<< REQUEST_NUMBER << endl;
        REQUEST_NUMBER++;
}
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
    camMatrix = lookAt(0, 5, 15, 0, 0, 0, 0, 1, 0); //T(0,0,-2);

    // Load and compile shader
    program = loadShaders("persp.vert", "persp.frag");
    m = LoadModelPlus("bunny.obj");
    program_skybox = loadShaders("skybox.vert", "skybox.frag");
    program_terrain = loadShaders("terrain.vert", "terrain.frag");
    printError("init shader");
    glUseProgram(program_terrain);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program_terrain, "tex"), 0); // Texture unit 0
    LoadTGATextureSimple("maskros512.tga", &tex_terrain);
    glBindTexture(GL_TEXTURE_2D, tex_terrain);

    // End of upload of geometry
    glUseProgram(program_skybox);
    glUniformMatrix4fv(glGetUniformLocation(program_skybox, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(program_skybox, "tex"), 1);
    LoadTGATextureSimple("SkyBox512.tga", &tex_sky);
    glBindTexture(GL_TEXTURE_2D, tex_sky); // Bind Our Texture tex_terrain
    skyBox = LoadModelPlus("skybox.obj");
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUseProgram(program_terrain);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    printError("init arrays");
}
void ladarPointsAdd(pair<GLfloat,GLfloat> point)
{
    boost::lock_guard<boost::mutex> guard(ladarPointsMutex);
    ladarPoints.push_back(point);
}
void fetchLADARPoints()
{
    string instruction;
    int angle,distance;
        zmq::message_t update;
    while (subscriber.recv(&update)) 
    {
        std::istringstream iss(static_cast<char*>(update.data()));
        // iss >> instruction;
        // if(instruction == "l")
        // {
            iss >> angle >> distance;
            // cout << "angle: " << angle <<" distance: " << distance <<endl;
            // ladarPoints.push_back(generateLadarPointFromInputData(distance/LADAR_SCALER,angle));
            ladarPointsAdd(generateLadarPointFromInputData(distance/LADAR_SCALER,angle));
            this_thread::sleep_for(chrono::milliseconds(1));
        // }
        // else
        // cout << "Got the bad instruction: " << instruction << endl;
    }
}
void paintLines(vec3 begin, vec3 end)
{
}
void display(void)
{
    car.carTick();
    // zmq::message_t update;
    // for(int i=0;i<200;i++)
    // {
    //     subFromARC.recv(&update);
    //     int angle, distance;
    //     istringstream iss(static_cast<char*>(update.data()));
    //     iss >> angle >> distance;
    //     cout << "Angle: " << angle << " and Distance: " << distance << endl;
    // }
    // // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUseProgram(program_skybox);
    // total =T(camMatrix.m[3],camMatrix.m[7]-0.5,camMatrix.m[11])*camMatrix;

    // glUniformMatrix4fv(glGetUniformLocation(program_skybox, "mdlMatrix"), 1, GL_TRUE, total.m);
    // DrawModel(skyBox,program_skybox, "inPositon","inNormal","inTexCoord");
    clearFloorY(testM);
    paintLadarPoints(ladarPoints, modelCoords, testM);
    printError("pre display");
    a += 0.05;
    glUseProgram(program_terrain);
    
    // vec3 b{0.0,0.5,0.0};
    // vec3 e{0.0,10.0,-20.0};
    // paintLines(b,e);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_terrain);
    total = modelCoords  * camMatrix;
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    // glUniformMatrix4fv(glGetUniformLocation(program_terrain, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(testM, program_terrain, "inPosition", "inNormal", "inTexCoord");
    glUseProgram(program);
    // Just a smidgen of C++: Operator overloading.
    //	mat4 rotationMatrix = Mult(Rz(a/3), Ry(a)); is replaced by:
    // glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    // glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    mat4 rotationMatrix = Rz(a / 3) * Ry(a);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");
    // cout << "test2" << endl;
    // total = S(5, 5, 5) * rotationMatrix;
    // cout << "test3" << endl;
    printError("display");

    glutSwapBuffers();
}

void setUpCamera()
{
    void cameraManipulationMouseInput();
    void cameraManipulationInput(unsigned char, int, int);
    void mouse();

    // _cameraMatrix = &camMatrix;

    // camera = SetVector(0, 2, -20);
    // lookAtPoint = SetVector(0, 0, 1); //must not be touched; otherwise does not work anymore
    // up = SetVector(0, 1, 0);
    // camera_go_straight = lookAtPoint;				     // z-FLOOR_WIDTH_RESis
    // camera_go_up = up;						     // y-FLOOR_WIDTH_RESis
    // camera_go_side = CrossProduct(camera_go_straight, camera_go_up); // x-FLOOR_WIDTH_RESis

    // updateCameraMatrix();

    // glutPassiveMotionFunc(mouse);
    // glutMouseFunc(cameraManipulationMouseInput);
    glutKeyboardFunc(cameraManipulationInput);
    // glutMotionFunc(cameraManipulationMouseFunc);
}
void updateCamera(vec3 pos, vec3 target)
{
    camMatrix = lookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, UP.x, UP.y, UP.z);
    // glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUseProgram(program_terrain);
    glUniformMatrix4fv(glGetUniformLocation(program_terrain, "camMatrix"), 1, GL_TRUE, camMatrix.m);
}
void cameraManipulationInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':
        // cout << "Test of t key" << endl;
        break;
    case 'c':
        // system("ssh -t -p 4444 arc@nhikim91.ddns.net ssh -p 2222 pi@localhost");
        break;
    case 'w':
        CAM_POS += vec3(0, 0, -1);
        TARGET += vec3(0, 0, -1);
        updateCamera(CAM_POS, TARGET);
        // printf("echo -e \" %x %x %x %x %x %x \" > fifo_to_control", 0x20, 0, 0, 0xA4, 'A', 'B');
        // sendToARC(changeSpeed(deltaSpeed));
        break;
    case 's':
        CAM_POS += vec3(0, 0, 1);
        TARGET += vec3(0, 0, 1);
        updateCamera(CAM_POS, TARGET);
        // sendToARC(changeSpeed(-deltaSpeed));
        // system("whoami");
        break;
    case 'd':
        CAM_POS += vec3(1, 0, 0);
        TARGET += vec3(1, 0, 0);
        updateCamera(CAM_POS, TARGET);
        // sendToARC(changeDirection(deltaTheta));
        break;
    case 'a':
        CAM_POS += vec3(-1, 0, 0);
        TARGET += vec3(-1, 0, 0);
        updateCamera(CAM_POS, TARGET);
        // sendToARC(changeDirection(-deltaTheta));
        break;
    case 'i':
        car.changeSpeed(1);
        // modelCoords = modelCoords * T(0, 0, -SPEED_TRANS);
        // clearFloorY(testM);
        // paintLadarPoints(ladarPoints, modelCoords, testM);
        break;
    case 'k':
        car.changeSpeed(-1);
    //     modelCoords = modelCoords * T(0, 0, SPEED_TRANS);
    //     clearFloorY(testM);
    //     paintLadarPoints(ladarPoints, modelCoords * rotationMatrix, testM);
        break;
    case 'j':
        car.changeDirection(-1);
    //     modelCoords = modelCoords * T(-SPEED_TRANS, 0, 0);
    //     clearFloorY(testM);
    //     paintLadarPoints(ladarPoints, modelCoords * rotationMatrix, testM);
        break;
    case 'l':
        car.changeDirection(1);
    //     modelCoords = modelCoords * T(SPEED_TRANS, 0, 0);
    //     clearFloorY(testM);
    //     paintLadarPoints(ladarPoints, modelCoords, testM);
        break;
    case 'u':
        car.carTick();
    //     rotation++;
    //     rotationMatrix = T(64, 0, 0) * Ry(SPEED_ROT * rotation) * T(-64, 0, 0);
    //     clearFloorY(testM);
    //     paintLadarPoints(ladarPoints, modelCoords * rotationMatrix, testM);
        break;
    // case 'o':
    //     rotation--;
    //     rotationMatrix = T(64, 0, 0) * Ry(SPEED_ROT * rotation) * T(-64, 0, 0);
    //     clearFloorY(testM);
    //     paintLadarPoints(ladarPoints, modelCoords * rotationMatrix, testM);
    //     break;
    case 'g':
        for (int i = 1; i < 24; i++)
        {
            // pair<GLfloat,GLfloat>testPair{-12+i*24+modelCoords.m[3],-12+i*12+modelCoords.m[11]};
            ladarPoints.push_back(generateLadarPointFromInputData(20 + i * 3, i / 3.14));
        }
        break;
    case 'b':
        CAM_POS = {0, 5, 15};
        TARGET = {0, 0, 0};
        updateCamera(CAM_POS, TARGET);
        break;
    case 'q':
        sendMessage("STYROR 0 +30");    
        break;
    case 'v':
        sendMessage("STYROR 15 -30");
        break;
    default:
        break;
    }
}
void cleanupSocketAtExit()
{
    zmq_close(&subscriber);
    zmq_close(&requester);
    zmq_term(&context);
    //Makes sure that all threads are terminated.
    terminate();
}
int main(int argc, char *argv[])
{
    atexit(cleanupSocketAtExit);
    thread first(fetchLADARPoints);
    subscriber.connect("tcp://localhost:2555");
cout << "test before connect" << endl;
    requester.connect("tcp://localhost:2550");
cout << "test after connect" << endl;
    //  Subscribe to id, is nothing
    const char *filter = (argc > 1)? argv [1]: "";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));
    CarPilot car;//&requester);
    

        //  Prepare our context and socket
    // zmq::context_t context(1);
    // zmq::socket_t subFromARC (context, ZMQ_SUB);
    // subFromARC.connect("tcp://localhost:4555");
    //    const char *filter = "";
    // subFromARC.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));
    // Socket.setsockopt_string(ZMQ_SUBSCRIBE,'',0);
    // Socket.setsockopt(ZMQ_RCVHWM,1000);

    // zmq::socket_t frontreq(context, ZMQ_REQ);
    // rc = zmq_connect(frontreq, "tcp://nhkim91.ddns.net:2225");
    // assert(rc == 0);

    // while (true)
    // {
    //     cout << "before connect request" << endl;
    //     zmq::message_t request;

    //     //  Wait for next request from client
    //     frontend.recv(&request);
    //     cout << "after connect request" << endl;
    //     std::cout << "Received Hello" << std::endl;

    //     zmq::message_t reply(5);
    //     memcpy(reply.data(), "World", 5);
    //     frontend.send(reply);

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
        // MarchingSquere *testClass = new MarchingSquere;
        // testClass->generateFloor(FLOOR_DEPT_RESOLUTION,FLOOR_WIDTH_RESOLUTION);
        testM = generateFlatTerrrain();
        // total = camMatrix * projectionMatrix * modelCoords;
        vec3 testCoords[testVector.size()];
        // for (int i = 0; i < testVector.size(); i++)
        // {
        //     testCoords[i].x = (int)(0.5 + testVector.at(i).relativeX);
        //     testCoords[i].y = rand()%10;
        //     testCoords[i].z = (int)(0.5 + testVector.at(i).relativeZ);
        //     pair<GLint,GLint>temp{testCoords[i].x, testCoords[i].z};
        //     ladarPoints[temp]=testCoords[i].y;
        //     // cout << "x: " << testCoords[i].x << " y: " << testCoords[i].y << " z: " << testCoords[i].z << endl;
        // }
        
        paintLadarPoints(ladarPoints, modelCoords, testM);
        // for(int i=0;i<50;i++)
        // {
        //     modifyFloorY(100-rand()%5,91-rand()%5, 3, testM);
        // }

        // findSurroundingCoords(vec3(50,0,32), 5, 0.1,testMap);
        // findSurroundingCoords(vec3(40,0,60), 5, 0.1,testMap);
        // findSurroundingCoords(vec3(20,0,20), 5, 0.1,testMap);
        // findSurroundingCoords(vec3(80,0,12), 5, 0.1,testMap);
        // for(it:testMap)
        // {
        //     cout << "testMap: " << it.first.first << "  " << it.first.second << " " << it.second << endl;
        // }

        // total= Ry(180);
        // total = T(-64,0,0);
        // CenterModel(testM);
        // testClass->generateModel();
        // cout << "test main" << endl;
        // vector<GLfloat> vert={-10.0f,0.0f,0.0f,
        //             0.0f,5.0f,0.0f,
        //             10.0f,0.0f,0.0f};
        // vector<GLfloat> norm={0.0f,1.0f,0.0f};
        // vector<GLfloat> tex={0.0f,0.0f};
        // vector<GLuint> ind={0,3,6};
        // testM =LoadDataToModel(&vert[0],
        //                         &norm[0],
        //                         &tex[0],
        //                         NULL,
        //                         &ind[0],
        //                         3,
        //                         3
        // );
        glutMainLoop();
        //  Send reply back to client
    // }
}
