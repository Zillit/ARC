#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include <iostream>
#include <math.h> 
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv; 

/*
 * An object with a color
 */
class Colored_Object
{
	private:
	int XPosLeft = 0;
	int XPosRight = 0;
	int YPos = 0;

	public:
	Colored_Object(int xpl, int xpr, int yp) : XPosL{xpl}, XPosR{xpr} ,YPos{yp} {}
	virtual ~Colored_Object() {}
	
	/*
	 * Calculates the closest angle to the obstictle
	 */
	double angle_close();
	
	/*
	 * Calculates the furthest angle to the obstictle
	 */
	double angle_far();

	/*
 	 * Calculates the distance straight forward to the obsticle
 	 */
	double ydistance();
	
	/*
 	 * Calculates the distance to the obsticle
 	 */
	double distance(double angle);
};

/*
 * Detect colored objects in image
 */
Mat detection_of_color(Mat camera_img, int* lowHSV, int* highHSV);

/* 
 * Detect lines in image
 */
Mat detect_lines(Mat camera_img);

/*
 * Calculate pixels to a obstical
 */
vector<Colored_Object> framed_objects(Mat imgThresholded);

#endif
