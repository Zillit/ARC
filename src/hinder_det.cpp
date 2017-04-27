// //#include <iostream>
// //#include <vector>
// //#include "opencv2/highgui/highgui.hpp"
// //#include "opencv2/imgproc/imgproc.hpp"

#include "hinder_det.h"
#define PI 3.14159265

using namespace std;
using namespace cv; 
using namespace ARC;


Mat detection_of_color(Mat camera_img, int lowHSV[3], int highHSV[3])
{
	Mat imgHSV;

	cvtColor(camera_img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat imgThresholded;

	inRange(imgHSV, Scalar(lowHSV[0], lowHSV[1], lowHSV[2]), Scalar(highHSV[0], highHSV[1], highHSV[2]), imgThresholded); //Threshold the image

	//morphological opening (remove small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

	//morphological closing (fill small holes in the foreground)
	dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	
	return imgThresholded;
}


Mat detect_lines(Mat camera_img)
{
	Mat detect;
	Mat dst;

	cvtColor(camera_img, detect, COLOR_BGR2GRAY);

	blur(detect, detect, Size(3,3));

	Canny(detect, detect, CANNY_THRE, 3*CANNY_THRE, 3);

	dst = Scalar::all(0);

	camera_img.copyTo(dst, detect);
	
	return dst;
}

vector<Colored_Object> object_pos(Mat imgThresholded)
{

    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(imgThresholded, contours, hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
    int numObjects = contours.size();
	vector<Colored_Object> objects;

	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( numObjects );
	vector<Rect> boundRect( numObjects );

	
	for( int i = 0; i < numObjects; i++ )
    { 
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    }

	for(int index = 0; index < numObjects && index < MAX_NUM_OBJECTS; index++)
	{
          double area = boundRect[index].area();
          if (area > MIN_AREA)
          {
			  Colored_Object tmp = {boundRect[index].tl().x, boundRect[index].br().x, boundRect[index].br().y};
			  objects.push_back(tmp);
          }
     }
	 return objects;
}

double Colored_Object::angle_close()
{
	double left_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosL - (PIXEL_WIDTH/2));
	double right_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosR - (PIXEL_WIDTH/2));

	if (XPosL >= 320)
	{
		return left_angle;
	}
	else if (XPosR < 320)
	{
		return 	right_angle;
	}
	else if (abs(left_angle) < abs(right_angle)) 
	{
		return left_angle;
	}
	else
	{ 
	return right_angle;
	}
}
/*
vector<double> pixel_to_lenght(vector<int> pixels)
{
	vector<double> lenght(COLS_TO_MEASURE, 0);
	int phi = 90 - ANGEL_OF_CAMERA - (VERTICAL_FOV/2);
	double pixel_per_angel = VERTICAL_FOV/PIXEL_HEIGHT;

	for(int n = 0; n < COLS_TO_MEASURE; n++)
	{
		lenght[n] =  HEIGHT_OF_CAMERA * tan(phi + (pixel_per_angel * pixels[n]));
	}
	return lenght;
}
*/
/*
vector<int> width_between_measure()
{
	int half_pixel_width = PIXEL_WIDTH/2;
	int pixel_width = COLS_TO_MEASURE/PIXEL_WIDTH;
	vector<int> messure_pixel_width(COLS_TO_MEASURE);
	
	for(int i; i < COLS_TO_MEASURE; i++)
	{
		
	}
}
	*/
/*	
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
*/


/*
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
*/
