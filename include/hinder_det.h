// #ifndef HINDER_DET_H
// #define HINDER_DET_H

#include <vector>
#include <iostream>
#include <math.h> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv; 
//using namespace ARC;

/*
 * Calculate number of pixel to green objekt in picture
 * in even spaced coloums
 */
vector<int> detection_of_green(Mat camera_img);

/* 
 * Degre from bottome of camera verticaly
 */
double vertical_degre(Mat imgOriginal, int n_pixels);

/* 
 * Degree from the center of the camera horizontoly
 */ 
double horizontol_degre(int measured_cols);

/*
 * Calculate the distace to a obstical
 */
double y_distance(double vertical_degree);

vector<double> y_distance_vector(Mat camera_img);

vector<int> cols_x_value();

#endif
