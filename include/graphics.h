#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <iostream>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GL/glew.h> // NOTE: You MUST include GLEW before any other OpenGL-related libraries!
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp" // NOTE: Need to compile, hence the quotes
#include "glm/gtc/matrix_transform.hpp"
// #include <SOIL.h>

// #include "MicroGlut.h"
#include "GL_utilities.h"
#include "defines.h"

using namespace std;

glm::mat4 projectionMatrix, total, skyBoxTotal, modelView, camMatrix, skyBoxCamera, skyBoxCoord;
GLuint program;



const GLchar *vertex_shader[] = {
    "void main(void) {\n",
    "    gl_Position = ftransform();\n",
    "    gl_FrontColor = gl_Color;\n",
    "}"
};

const GLchar *color_shader[] = {
    "void main() {\n",
    "    gl_FragColor = gl_Color;\n",
    "}"
};

class shader_prog {
    GLuint vertex_shader, fragment_shader, prog;

    template <int N>
    GLuint compile(GLuint type, char const *(&source)[N]) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, N, source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            throw std::logic_error(log);
            return false;
        }
        return shader;
    }
public:
    template <int N, int M>
    shader_prog(GLchar const *(&v_source)[N], GLchar const *(&f_source)[M]) {
        vertex_shader = compile(GL_VERTEX_SHADER, v_source);
        fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);
        prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);
    }

    operator GLuint() { return prog; }
    void operator()() { glUseProgram(prog); }

    ~shader_prog() {
        glDeleteProgram(prog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
};
void draw() { 
    // compile and link the specified shaders:
    static shader_prog prog(vertex_shader, color_shader);
    // Use the compiled shaders:    
    prog(); 
cout << "test" << endl;

    // Draw something:
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-1.0f, 0.0f, -1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, -1.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3d(0.0, -1.0, -1.0);
    glEnd();
}
#ifdef __cplusplus
extern "C" {
#endif
void init()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(OPENGL::LEFT_, OPENGL::RIGHT_, OPENGL::BOTTOM_, OPENGL::TOP_,
              OPENGL::NEAR_, OPENGL::FAR_);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, -3, 0, 0, 0, 0, 1, 0);

    // // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    cout << "GL inits" << endl;
    glFrustum(OPENGL::LEFT_, OPENGL::RIGHT_, OPENGL::BOTTOM_, OPENGL::TOP_,
        OPENGL::NEAR_, OPENGL::FAR_);

    program = loadShader("basic.vert", "basic.frag");
    glUseProgram(program);
    cout << "init shader" << endl;

        glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
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

void display(void)
{
    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // glColor3f(1.0, 1.0, 0.0);
    // glBegin(GL_QUADS);
    // glVertex3f(-0.5, -0.5, -0.5);
    // glVertex3f(0.5, -0.5, -0.5);
    // glVertex3f(0.5, 0.5, -0.5);
    // glVertex3f(-0.5, 0.5, -0.5);
    // glEnd();
    // Don't wait start processing buffered OpenGL routines
    draw();
    glFlush();
    glutSwapBuffers();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // // a += 0.01f;
    // // b += 1;
    // cout << "pre display" << endl;
    // // modelView = T(-128, 0, -128);
    // total = camMatrix, modelView;
    // // drawSkyBox();
    // glUseProgram(program);
    // // glActiveTexture(GL_TEXTURE0);
    // glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, &total[0][0]);
    // // glBindTexture(GL_TEXTURE_2D, tex1); // Bind Our Texture tex1
    // // glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    // // glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    // // glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    // // glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    // // tm = GenerateTerrain(&ttex, cos(a * 3));
    // // DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
    // // drawGroundObject(1 + 5 * sin(a) + a, 15 * cos(a), m);
    // // drawGroundObject(0, 0, m2);
}

void timer(int i)
{
    glutTimerFunc(10, &timer, i);
    glutPostRedisplay();
}

void graphicsInit(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    // glutInitContextVersion(3, 2);
    glutInitWindowSize(OPENGL::WINDOW_WIDTH, OPENGL::WINDOW_HIGHT);
    glutCreateWindow(OPENGL::WINDOW_NAME.c_str());
    glutDisplayFunc(display);
    init();
}
#ifdef __cplusplus
}
#endif

#endif