#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv; 

/*
 * An object with a color
 */
class ColoredObject
{
	private:
	int x_pos_left = 0;
	int x_pos_right = 0;
	int y_pos = 0;

	public:
	ColoredObject(int xpl, int xpr, int yp) : x_pos_left{xpl}, x_pos_right{xpr} ,y_pos{yp} {}
	virtual ~ColoredObject() {}

	/*
	 * Calculates the closest angle to the obstictle
	 */
	double angleClose();

	/*
	 * Calculates the furthest angle to the obstictle
	 */
	double angleFar();

	/*
     * Calculates the distance straight forward to the obsticle
 	 */
	double yDistance();
	
	/*
 	 * Calculates the distance to the obsticle
 	 */
	double distance(double angle);
};

/*
 * Detect colored objects in image
 */
Mat detectionOfColor(Mat camera_img, int* lowHSV, int* highHSV);

/* 
 * Detect lines in image
 */
Mat detectLines(Mat camera_img);

/*
 * Calculate pixels to a obstical
 */
vector<ColoredObject> framedObjects(Mat imgThresholded);

#endif
