#include <iostream>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

vector<int> detection_of_green(Mat camera_img ,int cols_to_measure)
{
    int pixel_height = camera_img.rows;
    int pixel_width = camera_img.cols; 

    int width_between_measure{pixel_width/(cols_to_measure+1)};
    vector<int> pixel_height_to_with (cols_to_measure);

    int iLowH = 38;
    int iHighH = 72;

    int iLowS = 50; 
    int iHighS = 255;

    int iLowV = 50;
    int iHighV = 255;


    Mat imgHSV;


    cvtColor(camera_img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

    Mat imgThresholded;

    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
      

    //morphological opening (remove small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (fill small holes in the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


    for(int count_cols{1}; count_cols <= cols_to_measure; count_cols++){

        for(int y{};y <= pixel_height; y = y + 5) {

            if(imgThresholded.at<uchar>(pixel_height - y, count_cols*width_between_measure) != 0) 
            { 
                pixel_height_to_with[count_cols - 1] = y;
                break;
            }
            else if (y == pixel_height )
            {
                pixel_height_to_with[count_cols-1] = pixel_height;
            }
        }
    }

return pixel_height_to_with;
}

int vertical_degre(Mat imgOriginal, int n_pixels) 
{
    int pixel_height{imgOriginal.rows};
    return 62/pixel_height*n_pixels;
}

/*
int main()
{
    VideoCapture cap(0); //capture the video from web cam

      if ( !cap.isOpened() )  // if not success, exit program
      {
        cout << "Cannot open the web cam" << endl;
        return -1;
        }

    while (true)
       {
          Mat imgOriginal;

          bool bSuccess = cap.read(imgOriginal); // read a new frame from video

            if (!bSuccess) //if not success, break loop
            {
                cout << "Cannot read a frame from video stream" << endl;
                break;
            }

            imshow("Original", imgOriginal);
            waitKey(30);


        vector<int> a = detection_of_green(imgOriginal, 15);


        for(int i{}; i < 15; ++i)
        {
            if (a[i] == 0) return -1;
            cout << vertical_degre(imgOriginal, a[i]) << " ";
        }
            cout << endl;

       }

       return 0;
}
*/