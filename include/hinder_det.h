#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv; 

/*
 * Calculate number of pixel to green objekt in picture
 * in even spaced coloums
 */
vector<int> detection_of_green(Mat camera_img, int cols_to_measure);

/* 
 * Degre from bottome of camera verticaly
 */
int vertical_degre(Mat imgOriginal, int n_pixels);

/* 
 * Degree from the center of the camera horizontoly
 */ 
int horizontol_degre(int measured_cols, int number_of_cols);

#endif