#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include <iostream>
#include <math.h> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv; 

vector<int> detect(Mat camera_img);
vector<double> pixel_to_lenght(vector<int> pixles);

