// Stanford bunny loaded from OBJ, perspective projection

// Variant using VectorUtils, which simplifies the matrix handling (but also hides the contents)

// Compile under Linux:
// gcc perspectivebunny.c ../../common/*.c ../../common/Linux/*.c -lGL -o perspectivebunny -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -l

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
#include <stdio.h>
#include <stdarg.h> // va_*
#include <string.h> // strlen, strcpy

Model *m;
// Reference to shader program
GLuint program;

int deltaSpeed = 1;
int deltaTheta = 1;

void init(void)
{
    // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_TRUE);
    printError("GL inits");

    // Load and compile shader
    program = loadShaders("persp.vert", "persp.frag");
    glUseProgram(program);
    printError("init shader");

    // Upload geometry to the GPU:
    m = LoadModelPlus("bunny.obj");

    // End of upload of geometry

    mat4 rotationMatrix = IdentityMatrix();
    mat4 camMatrix = lookAt(0, 1, 2, 0, 0, 0, 0, 1, 0); //T(0,0,-2);
    mat4 projectionMatrix = frustum(0.7, -0.7, 0.7, -0.7, 1, 30);

    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

    printError("init arrays");
}

float a = 0;

void display(void)
{
    printError("pre display");

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // a += 0.1;
    // mat4 rotationMatrix = Mult(Rz(a / 3), Ry(a));
    // glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
    // DrawModel(m, program, "inPosition", "inNormal", 0L);

    printError("display");

    glutSwapBuffers();
}
void timer(int arg)
{
    glutTimerFunc(20, timer, 0);
    glutPostRedisplay();
}
struct testStruct
{
    int value;
};

void sendToARC(struct testStruct command)
{
    printf("test sendToARC");
    printf(command.value);
}

struct testStruct changeSpeed(int deltaSpeed)
{

    // printf((char)deltaSpeed);
    // struct testStruct t={deltaSpeed};
    // return t;
}

char *concat(int count, ...)
{
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for (i = 0; i < count; i++)
        len += strlen(va_arg(ap, char *));
    va_end(ap);

    // Allocate memory to concat strings
    char *merged = calloc(sizeof(char), len);
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for (i = 0; i < count; i++)
    {
        char *s = va_arg(ap, char *);
        strcpy(merged + null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

void sendControlCommand(int speed, int theta)
{
    int checksum = 0x20 + 0 + 0 + speed + theta;

    char *temp;
    temp = concat(8, "echo -e \"", (char)0x20, (char)0, (char)0, (char)checksum, (char)speed, (char)theta, "\" > fifo_to_control");
    printf("echo -e \" %x %x %x %x %x %x \" > fifo_to_control", 0x20, 0, 0, checksum, speed, theta);
    printf(temp);
    free(temp);
}
void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':
        // the following need to be fixed.
        // sendControlCommand('A', 'B');
        break;
    case 'c':
        system("ssh -t -p 4444 arc@nhikim91.ddns.net ssh -p 2222 pi@localhost");
        break;
    case 'w':
        printf("echo -e \" %x %x %x %x %x %x \" > fifo_to_control", 0x20, 0, 0, 0xA4, 'A', 'B');
            // sendToARC(changeSpeed(deltaSpeed));
            break;
    case 's':
        // sendToARC(changeSpeed(-deltaSpeed));
        system("whoami");
        break;
    case 'd':
        // sendToARC(changeDirection(deltaTheta));
        break;
    case 'a':
        // sendToARC(changeDirection(-deltaTheta));
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutCreateWindow("GL3 obj and perspective example with VectorUtils");
    glutDisplayFunc(display);
    // glutTimerFunc(20, timer, 0);

    glutRepeatingTimer(20);
    glutKeyboardFunc(key);
    init();
    glutMainLoop();
}
