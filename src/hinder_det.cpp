
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


vector<Colored_Object> framed_objects(Mat imgThresholded)
{

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(imgThresholded, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    
	int numObjects = contours.size();
	vector<Colored_Object> objects;

	/// Approximate contours to polygons + get bounding rects
	vector<vector<Point>> contours_poly(numObjects);
	vector<Rect> boundRect(numObjects);
	
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
			  Colored_Object tmp(boundRect[index].tl().x, boundRect[index].br().x, boundRect[index].br().y);
			  objects.push_back(tmp);
          	}
     	}
	
	return objects;
}

double Colored_Object::angle_close()
{
	double left_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosL - (PIXEL_WIDTH/2));
	double right_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosR - (PIXEL_WIDTH/2));

	if (abs(left_angle) < abs(right_angle)) 
	{
		return left_angle;
	}
	else
	{ 
	return right_angle;
	}
}

double Colored_Object::angle_far()
{
	double left_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosL - (PIXEL_WIDTH/2));
	double right_angle = HORIZONTAL_FOV/PIXEL_WIDTH * (XPosR - (PIXEL_WIDTH/2));

	if (abs(left_angle) < abs(right_angle)) 
	{
		return right_angle;
	}
	else
	{ 
	return left_angle;
	}
}

double Colored_Object::ydistance()
{
	int phi = 90 - ANGEL_OF_CAMERA - (VERTICAL_FOV/2);
	
	return HEIGHT_OF_CAMERA * tan((phi + (VERTICAL_FOV/PIXEL_HEIGHT * (PIXEL_HEIGHT - YPos)))*PI/180);
}

double Colored_Object::distance(double angle)
{
	return ydistance() / cos(angle * PI/180);
}
