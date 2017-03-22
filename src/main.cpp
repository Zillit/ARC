#include <iostream>
#include <string>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "test.h"
#include "defines.cpp"



using namespace std;

mat4 projectionMatrix, total, skyBoxTotal, modelView, camMatrix, skyBoxCamera, skyBoxCoord;
GLuint program;

extern "C" { void init()
{

    // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    cout << "GL inits" << endl;
    projectionMatrix = frustum(ARC::LEFT_, ARC::RIGHT_, ARC::BOTTOM_, ARC::TOP_, 
        ARC::NEAR_, ARC::FAR_);

    program = loadShaders("terrain.vert", "terrain.frag");
    glUseProgram(program);
    cout << "init shader" << endl;

    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
    // LoadTGATextureSimple("maskros512.tga", &tex1);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, tex1);
    // LoadTGATextureData("fft-terrain.tga", &ttex);
    // cleanMap(tm);
    // tm = GenerateTerrain(&ttex, 1.0);
    // m = LoadModelPlus("octagon.obj");
    // m2 = LoadModelPlus("groundsphere.obj");

    // glUseProgram(program_skybox);
    // glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    // glActiveTexture(GL_TEXTURE1);
    // glUniform1i(glGetUniformLocation(program, "tex"), 1);
    // LoadTGATextureSimple("SkyBox512.tga", &tex_sky);

    // glBindTexture(GL_TEXTURE_2D, tex_sky); // Bind Our Texture tex1
    // skyBox = LoadModelPlus("skybox.obj");
    // Load terrain data
    glUseProgram(program);
    cout << "init terrain" << endl;
 }
}
void display(void)
{
    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // a += 0.01f;
    // b += 1;
    cout << "pre display" << endl;
    // modelView = T(-128, 0, -128);
    total = Mult(camMatrix, modelView);
    // drawSkyBox();
    glUseProgram(program);
    // glActiveTexture(GL_TEXTURE0);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    // glBindTexture(GL_TEXTURE_2D, tex1); // Bind Our Texture tex1
    // glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    // glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    // glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    // glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    // tm = GenerateTerrain(&ttex, cos(a * 3));
    // DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
    // drawGroundObject(1 + 5 * sin(a) + a, 15 * cos(a), m);
    // drawGroundObject(0, 0, m2);

    cout << "display 2" << endl;

    glutSwapBuffers();
}

void timer(int i)
{
    glutTimerFunc(10, &timer, i);
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 2);
    glutInitWindowSize(1200, 1200);
    glutCreateWindow("TSBK07 Main window");
    glutDisplayFunc(display);
    init();

    glutTimerFunc(10,&timer,0);
    return 0;
}
