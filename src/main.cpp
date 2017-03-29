// #include <iostream>
// #include <string>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>

//#include "graphics.h"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "test.h"
#include "defines.h"
#include "spi.h"
#include "hinder_det.h"



using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    /*
    SpiCom* spi0 = new SpiCom;
char* test=NULL;
    spi0->readAndWrite(0,test, 5);
*/

    int measured_cols{5};
    VideoCapture cap(0); //capture the video from web cam

    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }

    while(true)
    {   
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            return -1;
        }

        imshow("Original", imgOriginal);
        waitKey(30);

        vector<int> a{detection_of_green(imgOriginal, measured_cols)};

        for(int n{}; n < measured_cols; n++)
        {
            cout << horizontol_degre(n, measured_cols) << " ";
        }

        cout << endl;

        for(int n{}; n < measured_cols; n++)
        {
            cout << vertical_degre(imgOriginal, a[n]) << " ";
        }

        cout << endl;


    }


    // graphicsInit(argc, argv);
    // glutMainLoop();
    // glutTimerFunc(10,&timer,0);
    return 0;
}
