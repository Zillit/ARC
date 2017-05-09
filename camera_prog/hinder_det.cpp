#include "hinder_det.h"
#define PI 3.14159265

using namespace std;
using namespace cv; 
using namespace ARC;


Mat detectionOfColor(Mat camera_img, int low_hsv[3], int high_hsv[3])
{
	Mat img_hsv;

	cvtColor(camera_img, img_hsv, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat img_thresholded;

	inRange(img_hsv, Scalar(low_hsv[0], low_hsv[1], low_hsv[2]), Scalar(high_hsv[0], high_hsv[1], high_hsv[2]), img_thresholded); //Threshold the image

	//morphological opening (remove small objects from the foreground)
	erode(img_thresholded, img_thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	dilate( img_thresholded, img_thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

	//morphological closing (fill small holes in the foreground)
	dilate( img_thresholded, img_thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
	erode(img_thresholded, img_thresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
	
	return img_thresholded;
}


Mat detectLines(Mat camera_img)
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


vector<ColoredObject> framedObjects(Mat img_thresholded)
{

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(img_thresholded, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
	int num_objects = contours.size();
	vector<ColoredObject> objects;

	/// Approximate contours to polygons + get bounding rects
	vector<vector<Point>> contours_poly(num_objects);
	vector<Rect> bound_rect(num_objects);
	
	for( int i = 0; i < num_objects; i++ )
	{ 
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        bound_rect[i] = boundingRect( Mat(contours_poly[i]) );
	}

	for(int index = 0; index < num_objects && index < MAX_NUM_OBJECTS; index++)
	{
		double area = bound_rect[index].area();
        
		if (area > MIN_AREA)
         	{
			  ColoredObject tmp(bound_rect[index].tl().x, bound_rect[index].br().x, bound_rect[index].br().y);
			  objects.push_back(tmp);
          	}
     	}
	return objects;
}

double ColoredObject::angleClose()
{
	double left_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (x_pos_left - (PIXEL_WIDTH/2));
	double right_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (x_pos_right - (PIXEL_WIDTH/2));

	if (abs(left_angle) < abs(right_angle)) 
	{
		return left_angle;
	}
	else
	{ 
	return right_angle;
	}
}

double ColoredObject::angleFar()
{
	double left_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (x_pos_left - (PIXEL_WIDTH/2));
	double right_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (x_pos_right - (PIXEL_WIDTH/2));

	if (abs(left_angle) < abs(right_angle)) 
	{
		return right_angle;
	}
	else
    { 
        return left_angle;
    }
}

double ColoredObject::yDistance()
{
	int phi = 90 - ANGEL_OF_CAMERA - (VERTICAL_FOV/2);
	
	return HEIGHT_OF_CAMERA * tan((phi + (VERTICAL_FOV/PIXEL_HEIGHT * (PIXEL_HEIGHT - y_pos)))*PI/180);
}

double ColoredObject::distance(double angle)
{
	return yDistance() / cos(angle * PI/180);
}
