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
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "test.h"
#include "defines.h"
#include "spi.h"
#include "hinder_det.h"



using namespace std;
using namespace cv;
using namespace ARC;

int main(int argc, char *argv[])
{
   
    SpiCom* spi0 = new SpiCom;
char* test=NULL;
    spi0->readAndWrite(0,test, 5);


// Camera test

/*
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

        Mat imgHSV;

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;

        inRange(imgHSV, Scalar(38, 50, 50), Scalar(72, 255, 255), imgThresholded); //Threshold the image

        imshow("Threshold", imgThresholded);
        imshow("Original", imgOriginal);
        waitKey(30);


        //for(int n{}; n < COLS_TO_MEASURE; n++)
        //{
            cout << y_distance_vector(imgOriginal)[0];
        //}

        cout << endl;


    }

*/
    // graphicsInit(argc, argv);
    // glutMainLoop();
    // glutTimerFunc(10,&timer,0);
    return 0;
}
