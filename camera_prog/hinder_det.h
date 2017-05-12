#ifndef HINDER_DET_H
#define HINDER_DET_H

#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "defines.h"
#ifdef __arm__
#include <raspicam/raspicam_cv.h>
#endif

using namespace std;
using namespace cv; 

/*
 * Ett färgat objekt eller markering
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
	 * Beräknar vinkeln till objektets närmsta hörn
	 */
	double angleClose();

	/*
	 * Beräknar vinkeln till objektets bortre hörn
	 */
	double angleFar();

	/*
     * Beräknar avståndet "rakt fram" till objektet
 	 */
	double yDistance();
	
	/*
 	 * Beräknar det totala avståndet till objektet
 	 */
	double distance(double angle);
};

/*
 * Upptäcker färgade objekt i bilden
 */
Mat detectionOfColor(Mat camera_img, int* lowHSV, int* highHSV);

/* 
 * Upptäcker linjer i bilden
 */
Mat detectLines(Mat camera_img);

/*
 * Return evry line in image
 */
vector<Vec4i> linesInImage(Mat camera_img);

/*
 * Skapar rektanglar av pixelklumpar
 */
vector<ColoredObject> framedObjects(Mat imgThresholded);

#endif
