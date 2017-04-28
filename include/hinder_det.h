#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include <iostream>
#include <math.h> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
//#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv; 

/*
 * An object with a color
 */
class Colored_Object
{
	private:
	int XPosL = 0;
	int XPosR = 0;
	int YPos = 0;

	public:
	Colored_Object(int xpl, int xpr, int yp) : XPosL{xpl}, XPosR{xpr} ,YPos{yp} {}
	virtual ~Colored_Object() {}
	double angle_close();
	double angle_far();
	double distance();
};

/*
 * Detect colored objekt in image
 */
Mat detection_of_color(Mat camera_img, int* lowHSV, int* highHSV);

/* 
 * Detect lines in image
 */
Mat detect_lines(Mat camera_img);

/*
 * Calculate pixels to a obstical
 */
vector<Colored_Object> object_pos(Mat imgThresholded);

/*
 * Calculate the distace to a obstical
 */
vector<double> pixel_to_lenght(vector<int> pixels);


/* 
 * Degre from bottome of camera verticaly
 */
//double vertical_degre(Mat imgOriginal, int n_pixels);

/* 
 * Degree from the center of the camera horizontoly
 */ 
//double horizontol_degre(int measured_cols);

/*
 * Calculate the distace to a obstical
 */
//double y_distance(double vertical_degree);

/*
 * Return a vector of distances to obsticals
 * in diffrent directions
 */
//vector<double> y_distance_vector(Mat camera_img);

/*
 * Calculate where in the picture the distance mesurement hapens
 */
//vector<int> cols_x_value();

#endif
