#include "ModelObj.h"

void ModelObj::printValues(GLfloat* array,GLuint size)
{
    for (int i=0;i<size;i++)
    {
        cout << array[i] << endl;
    }
}

// void TriangleGeometric::generateTriangleMesh(vector<cameraSample> points)
// {
//     //generate node of the fan
//         vertexVector={0.0f,0.0f,0.0f};
//     cameraSample tempLast{0.0f, 0.0f};
//     //generate the foreward points of the fan
//     for (auto &i : points)
//     {
//         vertexVector.push_back(i.relativeX);
//         vertexVector.push_back(0.0f);
//         vertexVector.push_back(i.relativeZ);
//         //somekind of texCoords, update after need
//         texCoordVector.push_back(i.relativeX);
//         texCoordVector.push_back(i.relativeZ);
//         //Create normals vector
//         normalVector.push_back(i.relativeX / 6 + tempLast.relativeX / 2);
//         normalVector.push_back(1.0f);
//         normalVector.push_back(i.relativeZ / 6 + tempLast.relativeZ / 2);
//         tempLast.relativeX=i.relativeX;
//         tempLast.relativeZ=i.relativeZ;
//     }
//     //Create the triangle indexes
//     for (int i = 1; i < points.size(); i++)
//     {
//         indexVector.push_back(0); //Node
//         indexVector.push_back(i);
//         indexVector.push_back(i+1);
//     }
//     cout << "test size of points " << points.size() << endl;
//     vertexCount = points.size()+1;
//     triangleCount = points.size()-1;
// }
void ModelObj::generateModel()
{
    if (m == NULL)
        {
            m = LoadDataToModel(
            &vertexVector[0],
            &normalVector[0],
            &texCoordVector[0],
            NULL,
            &indexVector[0],
            vertexCount,
            triangleCount);
        }
    cout << "testGenerateModel: " << m->numIndices << endl;
}

void MarchingSquere::generateFloor(GLuint FLOOR_DEPT_RES, GLuint FLOOR_WIDTH_RES)
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
            vec3 temp = generateNormalFromPoints(vertexArray[(-FLOOR_WIDTH_RES + x + z * FLOOR_WIDTH_RES) * 3 + 1],
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
}

vec3 MarchingSquere::generateNormalFromPoints(GLfloat Zleft, GLfloat Zright, GLfloat Zupright, GLfloat Zdownleft, GLfloat Zup, GLfloat Zdown, GLuint dept, GLuint width)
{
    vec3 temp;
    temp.x = (2 * (Zleft - Zright) - Zupright + Zdownleft + Zup - Zdown);
    temp.y = 6.0;
    temp.z = (2 * (Zdown - Zup) + Zupright + Zdownleft - Zup - Zleft);
    return temp;
}