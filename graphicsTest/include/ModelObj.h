#ifndef MODELOBJ_H
#define MODELOBJ_H

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

#include <iostream>
#include <string>
#include <vector>
#include <iterator>

using namespace std;
struct cameraSample
{
  float relativeZ;
  float relativeX;
};

class ModelObj
{
public:
  ModelObj(){};
  ~ModelObj();
  void printValues(GLfloat* vec,GLuint size);
  void generateModel();
  Model *getM() { return m; };
protected:
  int vertexCount;
  int triangleCount;
  GLfloat vertexVector[];
  GLfloat normalVector[];
  GLfloat texCoordVector[];
  GLuint indexVector[];
  // Space for saving VBO and VAO IDs
  GLuint vao;            // VAO
  GLuint vb, ib, nb, tb; // VBOs
  Model *m;
};

class TriangleGeometric : public ModelObj
{
public:
  TriangleGeometric(){};
  ~TriangleGeometric();
  void generateTriangleMesh(vector<cameraSample> points);

private:
};
class Floor : public TriangleGeometric
{
public:
  Floor();
  ~Floor();
  void generateFlatFloor();

private:
};

class MarchingSquere:public ModelObj
{
public:
 MarchingSquere(){};
 ~MarchingSquere();
 void generateFloor(GLuint FLOOR_DEPT_RES, GLuint FLOOR_WIDTH_RES);
 vec3 generateNormalFromPoints(GLfloat Zleft, GLfloat Zright, GLfloat Zupright, GLfloat Zdownleft, GLfloat Zup, GLfloat Zdown, GLuint dept, GLuint width);
};

#endif