// //#include <iostream>
// //#include <vector>
// //#include "opencv2/highgui/highgui.hpp"
// //#include "opencv2/imgproc/imgproc.hpp"

#include "hinder_det.h"
#define PI 3.14159265

using namespace std;
using namespace cv; 
using namespace ARC;

/*
vector<int> width_between_measure()
{
	int half_pixel_width{PIXEL_WIDTH/2};
	int pixel_width{COLS_TO_MEASURE/PIXEL_WIDTH};
	vector<int> messure_pixel_width(COLS_TO_MEASURE);
	
	for(int i; i < COLS_TO_MEASURE; i++)
	{
		
	}
}
	*/
	
vector<int> cols_x_value()
{
	int width_between_measure{PIXEL_WIDTH / (COLS_TO_MEASURE - 1)};
	vector<int> x_value(COLS_TO_MEASURE);
	
	for(int i{1}; i <= COLS_TO_MEASURE/2 - 1; i++)
	{
		x_value[i - 1] = i * width_between_measure;
	}
	
	x_value[COLS_TO_MEASURE/2 - 1] = PIXEL_WIDTH/2 - MESSURED_COL_RIGHT_LEFT_FROM_CENTER;
	x_value[COLS_TO_MEASURE/2] = PIXEL_WIDTH/2;
	x_value[COLS_TO_MEASURE/2 + 1] = PIXEL_WIDTH/2 + MESSURED_COL_RIGHT_LEFT_FROM_CENTER;
	
	for(int i{COLS_TO_MEASURE/2 + 3}; i <= COLS_TO_MEASURE; i++)
	{
		x_value[i - 1] = (i - 2) * width_between_measure;
	}
	
	return x_value;
}

vector<int> detection_of_green(Mat camera_img)
{

    int width_between_measure(PIXEL_WIDTH/COLS_TO_MEASURE);
    vector<int> pixel_height_to_with (COLS_TO_MEASURE, PIXEL_HEIGHT);

     Mat imgHSV;

	cvtColor(camera_img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat imgThresholded;

    inRange(imgHSV, Scalar(ILOWH, ILOWS, ILOWV), Scalar(IHIGHH, IHIGHS, IHIGHV), imgThresholded); //Threshold the image


     //morphological opening (remove small objects from the foreground)
     erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
     dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

     //morphological closing (fill small holes in the foreground)
     dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
     erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    Moments picture_moments{moments(imgThresholded)};
    double pixel_area{picture_moments.m00};
	double max_pixel_height{PIXEL_HEIGHT/2 + 
		ANGEL_OF_CAMERA*PIXEL_HEIGHT/VERTICAL_FOV};

    if(pixel_area > 10000)
    {
        for(int count_cols{1}; count_cols <= COLS_TO_MEASURE; count_cols++){

            for(int y{};y <= max_pixel_height; y += 10) 
            {
                if(imgThresholded.at<uchar>(PIXEL_HEIGHT - y, count_cols*width_between_measure) != 0) 
                { 
                    pixel_height_to_with[count_cols - 1] = y;
                    break;
                }
                else if (y == PIXEL_HEIGHT )
                {
                    pixel_height_to_with[count_cols-1] = PIXEL_HEIGHT;
                }
            }
        }
    }
	return pixel_height_to_with;
 }

double vertical_degre(Mat imgOriginal, int n_pixels) 
{  
    return (double) VERTICAL_FOV*n_pixels/PIXEL_HEIGHT;
}

double horizontol_degre(int measured_cols)
{   
    double degre_space{HORIZONTAL_FOV/(COLS_TO_MEASURE + 1)};
    if(measured_cols == COLS_TO_MEASURE/2) return 0;
    if(measured_cols < COLS_TO_MEASURE/2) return (-HORIZONTAL_FOV/2 + degre_space + degre_space*measured_cols);
    if(measured_cols > COLS_TO_MEASURE/2) return (degre_space + degre_space*measured_cols - HORIZONTAL_FOV/2);
} 

double y_distance(double vertical_degree)
{
     double distance{HEIGHT_OF_CAMERA*tan(((90 - (VERTICAL_FOV/2) + vertical_degree - ANGEL_OF_CAMERA) * PI)/180)};
     
     if(distance < 0) return -1;
     else return distance;
}

vector<double> y_distance_vector(Mat camera_img)
{
    vector<double> distance (COLS_TO_MEASURE);
    vector<int> number_pixels{detection_of_green(camera_img)};

    for(int i{}; i < COLS_TO_MEASURE; i++)
    {
        distance[i] = round(y_distance(vertical_degre(camera_img, number_pixels[i])) * 100) / 100;
    }
    return distance;
}

/*
int main()
{
    VideoCapture cap(0); //capture the video from web cam

//       if ( !cap.isOpened() )  // if not success, exit program
//       {
//         cout << "Cannot open the web cam" << endl;
//         return -1;
//         }

//     while (true)
//        {
//           Mat imgOriginal;

//           bool bSuccess = cap.read(imgOriginal); // read a new frame from video

//             if (!bSuccess) //if not success, break loop
//             {
//                 cout << "Cannot read a frame from video stream" << endl;
//                 break;
//             }

//             imshow("Original", imgOriginal);
//             waitKey(30);


//         vector<int> a = detection_of_green(imgOriginal, 15);


//         for(int i{}; i < 15; ++i)
//         {
//             if (a[i] == 0) return -1;
//             cout << vertical_degre(imgOriginal, a[i]) << " ";
//         }
//             cout << endl;

//        }

       return 0;
}
*/
