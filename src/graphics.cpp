// Stanford bunny loaded from OBJ, perspective projection

// Variant using VectorUtils, which simplifies the matrix handling (but also hides the contents)

// C++ version!
// gcc perspectivebunny.cpp ../../common/*.c ../../common/Linux/*.c -lGL -o perspectivebunny -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm

// You *might* need to compile C++ with g++, and then this should do it:
// g++ perspectivebunny.cpp -c -o perspectivebunny.o -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES
// gcc perspectivebunny.o ../../common/*.c ../../common/Linux/*.c -lGL -o perspectivebunny -I../../common -I../../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm

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

Model *m;
	// Reference to shader program
	GLuint program;

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
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
	mat4 camMatrix = lookAt(0,1,2, 0,0,0, 0,1,0);//T(0,0,-2);
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

	a += 0.1;

// Just a smidgen of C++: Operator overloading.
//	mat4 rotationMatrix = Mult(Rz(a/3), Ry(a)); is replaced by:
	mat4 rotationMatrix = Rz(a/3) * Ry(a);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rotationMatrix.m);
	DrawModel(m, program, "inPosition", "inNormal", 0L);

	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 obj and perspective example with VectorUtils");
	glutDisplayFunc(display); 
	glutRepeatingTimer(20);
	init ();
	glutMainLoop();
}
