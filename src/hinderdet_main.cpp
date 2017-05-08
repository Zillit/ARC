
#include <string>

#include "opencv2/highgui/highgui.hpp"
#include "defines.h"
#include "hinder_det.h"
#include <iostream>

using namespace std;
using namespace cv;
using namespace ARC;

int main(int argc, char *argv[])
{

// Camera test
	int lhsv[3] = {ILOWH, ILOWS, ILOWV};
	int hhsv[3] = {IHIGHH, IHIGHS, IHIGHV};
	
	int lb[] = {0, 0, 0};
	int hb[] = {255, 255, 30};

	raspicam::RaspiCam_Cv Camera;
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 320);

    if ( !Camera.open() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        return -1;
    }
    
    int num_laps = 0;
    int z = 1;
    

    while(true)
    {   
        Mat imgOriginal;
		
		Camera.grab();
		
        Camera.retrieve(imgOriginal); // read a new frame from video
        
        Mat all_green = detectionOfColor(imgOriginal, lhsv, hhsv);
        Mat black = detectionOfColor(imgOriginal, lb, hb);
        //Mat lines = detect_lines(imgOriginal);
		
		//imshow("lines",lines);
		imshow("all_green", all_green);
        imshow("imgOriginal", imgOriginal);
        imshow("Black", black);
        waitKey(30);
        
		vector<ColoredObject> object = framedObjects(all_green);
		
        if(!object.empty())
        {
			for(int n{}; n < object.size(); n++)
			{
				cout << object[n].yDistance()  << ", ";
				if(object[n].yDistance() < 0.5)
				{
					z = 0;
					break;
				}
			}
		}
		else if(object.empty() && z == 0)
		{
			z = 1;
			num_laps++;
		}	
		
		cout << num_laps << endl;
    }
    return 0;
}

//-----------------------------------------------------------------------------

