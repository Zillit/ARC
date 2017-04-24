#include "hinder_det_canny.h"

using namespace std;
using namespace cv;
using namespace ARC;

vector<int> detect(Mat camera_img)
{
	int width(PIXEL_WIDTH/COLS_TO_MEASURE);
	vector<int> pixels(COLS_TO_MEASURE, PIXEL_HEIGHT);

	Mat detect;
	Mat dst;

	cvtColor(camera_img, detect, COLOR_BGR2GRAY);

	blur(detect, detect, Size(3,3));

	Canny(detect, detect, CANNY_THRE, 3*CANNY_THRE, 3);

	dst = Scalar::all(0);

	camera_img.copyTo(dst, detect);


	for(int count_cols = 1; count_cols <= COLS_TO_MEASURE; count_cols++)
	{

	for(int y = 0;y <= PIXEL_HEIGHT; y += 10) 
	{
		if(dst.at<uchar>(PIXEL_HEIGHT - y, count_cols*width) != 0) 
		{ 
			pixels[count_cols - 1] = y;
			break;
		}
		else if (y == PIXEL_HEIGHT )
		{
			pixels[count_cols-1] = PIXEL_HEIGHT;
		}
	}
	}

	return pixels;
}

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
