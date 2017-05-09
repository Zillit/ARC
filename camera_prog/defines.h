#ifndef DEFINES_H
#define DEFINES_H
#include <string>
using namespace std;
namespace OPENGL
{
    const float LEFT_=-0.5;
    const float RIGHT_=0.5;
    const float NEAR_=1;
    const float FAR_=300;
    const float TOP_=0.5;
    const float BOTTOM_=-0.5;
    const int WINDOW_HIGHT=960;
    const int WINDOW_WIDTH=1200;
    const string WINDOW_NAME="ARC User Main Window V 0.1";
}
namespace ARC
{
    const float TOP_SPEED=10;
    
    
    const int COLS_TO_MEASURE=10;
    const double HORIZONTAL_FOV = 62.2;
    const double VERTICAL_FOV = 48.8;
    const int ILOWH = 32;
    const int IHIGHH = 78;
    const int ILOWS = 30; 
    const int IHIGHS = 255;
    const int ILOWV = 30;
    const int IHIGHV = 255;
    const double HEIGHT_OF_CAMERA = 0.32;
    const double ANGEL_OF_CAMERA = 20;
    const int PIXEL_HEIGHT = 240;
    const int PIXEL_WIDTH = 320;
    const int MESSURED_COL_RIGHT_LEFT_FROM_CENTER = 20;
    const int CANNY_THRE = 100;
    const int MAX_NUM_OBJECTS = 10;
    const int MIN_AREA = 100;
    const int NUM_LAPS = 3;

}

#endif
