#include "ModelObj.h"

void ModelObj::printValues(vector<GLfloat> vec)
{
    for (auto i : vec)
    {
        cout << i << endl;
    }
}

void TriangleGeometric::generateTriangleMesh(vector<cameraSample> points)
{
    //generate node of the fan
        vertexVector={0.0f,0.0f,0.0f};
    cameraSample tempLast{0.0f, 0.0f};
    //generate the foreward points of the fan
    for (auto &i : points)
    {
        vertexVector.push_back(i.relativeX);
        vertexVector.push_back(0.0f);
        vertexVector.push_back(i.relativeZ);
        //somekind of texCoords, update after need
        texCoordVector.push_back(i.relativeX);
        texCoordVector.push_back(i.relativeZ);
        //Create normals vector
        normalVector.push_back(i.relativeX / 6 + tempLast.relativeX / 2);
        normalVector.push_back(1.0f);
        normalVector.push_back(i.relativeZ / 6 + tempLast.relativeZ / 2);
        tempLast.relativeX=i.relativeX;
        tempLast.relativeZ=i.relativeZ;
    }
    //Create the triangle indexes
    for (int i = 1; i < points.size(); i++)
    {
        indexVector.push_back(0); //Node
        indexVector.push_back(i);
        indexVector.push_back(i+1);
    }
    cout << "test size of points " << points.size() << endl;
    vertexCount = points.size()+1;
    triangleCount = points.size()-1;
}
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