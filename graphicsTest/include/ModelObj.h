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
// #include "defines.h"

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
  void printValues(vector<GLfloat> vec);
  void generateModel();
  Model *getM() { return m; };

  int vertexCount;
  int triangleCount;
  vector<GLfloat> vertexVector;
  vector<GLfloat> normalVector;
  vector<GLfloat> texCoordVector;
  vector<GLuint> indexVector;
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

#endif