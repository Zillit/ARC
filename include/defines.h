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
    
    
    const int COLS_TO_MEASURE=15;
    const double HORIZONTAL_FOV = 62.2;
    const double VERTICAL_FOV = 48.8;
    const int ILOWH = 38;
    const int IHIGHH = 72;
    const int ILOWS = 50; 
    const int IHIGHS = 255;
    const int ILOWV = 50;
    const int IHIGHV = 255;
    const double HEIGHT_OF_CAMERA = 0.2;
    const double ANGEL_OF_CAMERA = 0;
    const int PIXEL_HEIGHT = 480;
    const int PIXEL_WIDTH = 640;
    const int MESSURED_COL_RIGHT_LEFT_FROM_CENTER = 20;
    const int CANNY_THRE = 60;

}

#endif
