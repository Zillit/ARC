// // #include <iostream>
// // #include <string>

// #include <iostream>
// #include <fstream>
// #include <algorithm>
// #include <vector>
// #include <string>
// #include <cmath>
// #include <thread>
// #include <chrono>

// #include "graphics.h"
// //#include "opencv2/imgproc/imgproc.hpp"
// //#include "opencv2/highgui/highgui.hpp"
// #include "VectorUtils3.h"
// #include "loadobj.h"
// #include "LoadTGA.h"
// #include "test.h"
// #include "defines.h"
// #include "spi.h"
// #include "hinder_det.h"
// #ifdef LOG1
// #include "log1.h"
// #else
// #include "log.h"
// #endif

// using namespace std;
// // using namespace cv;

// int main(int argc, char *argv[])
// {


//     graphicsInit(argc, argv);
//     glutMainLoop();
//     glutTimerFunc(10,&timer,0);

//     // SpiCom *spi0 = new SpiCom;
//     // char *test = NULL;
//     // spi0->readAndWrite(0, test, 5);
//     //   try
//     // {
//     //     FILELog::ReportingLevel() = FILELog::FromString(argv[1] ? argv[1] : "DEBUG1");
// 	//     const int count = 3;
//     //     FILE_LOG(logDEBUG) << "A loop with " << count << " iterations";
//     //     for (int i = 0; i != count; ++i)
//     //     {
// 	//         FILE_LOG(logDEBUG1) << "the counter i = " << i;
//     //     }
//     //     return 0;
//     // }
//     // catch(const std::exception& e)
//     // {
//     //     FILE_LOG(logERROR) << e.what();
//     // }

//     /* Camera test

//     int measured_cols{5};
//     VideoCapture cap(0); //capture the video from web cam

//     if ( !cap.isOpened() )  // if not success, exit program
//     {
//         cout << "Cannot open the web cam" << endl;
//         return -1;
//     }

//     while(true)
//     {   
//         Mat imgOriginal;

//         bool bSuccess = cap.read(imgOriginal); // read a new frame from video

//         if (!bSuccess) //if not success, break loop
//         {
//             cout << "Cannot read a frame from video stream" << endl;
//             return -1;
//         }

//         imshow("Original", imgOriginal);
//         waitKey(30);

//         vector<int> a{detection_of_green(imgOriginal, measured_cols)};

//         for(int n{}; n < measured_cols; n++)
//         {
//             cout << horizontol_degre(n, measured_cols) << " ";
//         }

//         cout << endl;

//         for(int n{}; n < measured_cols; n++)
//         {
//             cout << vertical_degre(imgOriginal, a[n]) << " ";
//         }

//         cout << endl;


//     }
// */
//     return 0;
// }
// //=============================================================================
// // Sample Application for GLEW, and cwc Application/Window class using freeglut
// //=============================================================================

// #include <GL/glew.h>
// #include <GL/freeglut.h>
// #include "glApplication.h"
// #include "glutWindow.h"
// #include <iostream>
// #include "glsl.h"

// //-----------------------------------------------------------------------------


// class myWindow : public cwc::glutWindow
// {
// protected:
//    cwc::glShaderManager SM;
//    cwc::glShader *shader;

// public:
// 	myWindow(){}

// 	virtual void OnRender(void)
// 	{
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
//       if (shader) shader->begin();
//       glutSolidSphere(1.0,32,32);
//       if (shader) shader->end();

//       glutSwapBuffers();

// 		//Repaint();
// 	}

// 	virtual void OnIdle() {}

// 	// When OnInit is called, a render context (in this case GLUT-Window) 
// 	// is already available!
// 	virtual void OnInit()
// 	{
// 		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
// 		glShadeModel(GL_SMOOTH);
// 		glEnable(GL_DEPTH_TEST);

// 		shader = SM.loadfromFile("vertexshader.txt","fragmentshader.txt"); // load (and compile, link) from file
// 		  if (shader==0) 
// 			  std::cout << "Error Loading, compiling or linking shader\n";

// 	}

// 	virtual void OnResize(int w, int h) {}
// 	virtual void OnClose(void){}
// 	virtual void OnMouseDown(int button, int x, int y) {}    
// 	virtual void OnMouseUp(int button, int x, int y) {}
// 	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y){}

// 	virtual void OnKeyDown(int nKey, char cAscii)
// 	{       
// 		if (cAscii == 27) // 0x1b = ESC
// 		{
// 			this->Close(); // Close Window!
// 		} 
// 	};

// 	virtual void OnKeyUp(int nKey, char cAscii)
// 	{
// 		if (cAscii == 'f')
// 		{
// 			SetFullscreen(true);
// 		}
// 		else if (cAscii == 'w')
// 		{
// 			SetFullscreen(false);
// 		}
// 	};
// };

// //-----------------------------------------------------------------------------

// class myApplication : public cwc::glApplication

#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>

#include "graphics.h"
// #include "opencv2/imgproc/imgproc.hpp"
// #include "opencv2/highgui/highgui.hpp"
// #include <raspicam/raspicam_cv.h>
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "test.h"
#include "defines.h"
// #include "spi.h"
#include "hinder_det.h"
#ifdef LOG1
#include "log1.h"
#else
#include "log.h"
#endif

using namespace std;
// using namespace cv;
using namespace ARC;

int main(int argc, char *argv[])
{
	// myApplication*  pApp = new myApplication;
	// myWindow* myWin = new myWindow();

	// pApp->run();
	// delete pApp;
	// return 0;
      try
    {
        FILELog::ReportingLevel() = FILELog::FromString(argv[1] ? argv[1] : "DEBUG1");
	    const int count = 3;
        FILE_LOG(logDEBUG) << "A loop with " << count << " iterations";
        for (int i = 0; i != count; ++i)
        {
	        FILE_LOG(logDEBUG1) << "the counter i = " << i;
        }
        return 0;
    }
    catch(const std::exception& e)
    {
        FILE_LOG(logERROR) << e.what();
    }

// Camera test

/*
    //VideoCapture cap(0); //capture the video from web cam

	raspicam::RaspiCam_Cv Camera;
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);

    if ( !Camera.open() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }
    


    while(true)
    {   
        Mat imgOriginal;
		
		Camera.grab();
		
        Camera.retrieve(imgOriginal); // read a new frame from video

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
        
       vector<double> a = y_distance_vector(imgOriginal);


        for(int n{}; n < COLS_TO_MEASURE; n++)
        {
            cout << a[n] << ", ";
        }

        cout << endl;


    }

*/
    // graphicsInit(argc, argv);
    // glutMainLoop();
    // glutTimerFunc(10,&timer,0);
    return 0;
}

//-----------------------------------------------------------------------------

