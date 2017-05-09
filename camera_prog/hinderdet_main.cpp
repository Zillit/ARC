#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "defines.h"
#include "hinder_det.h"
#include <iostream>
#include <zmq.hpp>

using namespace std;
using namespace cv;
using namespace ARC;

int main()
{
   zmq::context_t context (1);
   zmq::socket_t socket (context, ZMQ_REQ);
   socket.connect ("tcp://localhost:5567");

   int lhsv[3] = {ILOWH, ILOWS, ILOWV};
   int hhsv[3] = {IHIGHH, IHIGHS, IHIGHV};
        
   int lb[] = {0, 0, 0};
   int hb[] = {255, 255, 30};
   
#ifdef _arm_
   raspicam::RaspiCam_Cv Camera;
   Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
   Camera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
   
   if ( !Camera.open() )  
   {
      cout << "Cannot open the web cam" << endl;
      return -1;
   }

#else
   VideoCapture Camera(0); //capture the video from web cam
   
   if ( !Camera.isOpened() )  
   {
      cout << "Cannot open the web cam" << endl;
      return -1;
   }

#endif

    
   int num_laps = 0;
   int z = 1;

   while(true)
   {   
      Mat imgOriginal;
                
      Camera.grab();
                
      Camera.retrieve(imgOriginal);
        
      Mat all_green = detectionOfColor(imgOriginal, lhsv, hhsv);
      Mat black = detectionOfColor(imgOriginal, lb, hb);
                
      imshow("all_green", all_green);
      imshow("imgOriginal", imgOriginal);
      imshow("Black", black);
      waitKey(30);
        
      vector<ColoredObject> object = framedObjects(all_green);
      vector<ColoredObject> goal_line= framedObjects(black);

      for(int n = 0; n < object.size(); n++)
      {
         cout << ", object " << object[n].distance(object[n].angleClose());
            
         if(object[n].distance(object[n].angleClose()) < 2)
         {
			string string_to_send = to_string((int)round(100 * object[n].distance(object[n].angleClose())));
			string_to_send += " " + to_string((int)round(object[n].angleClose()));
			string_to_send += " " + to_string((int)round(object[n].angleFar()));
			
			cout << " String to send: " << string_to_send << endl;
			
			if(string_to_send.size() <= 32)
			{
				zmq::message_t request (string_to_send.size());
				memcpy (request.data (), "Object", string_to_send.size());
				std::cout << "Sending Object" << std::endl;
				socket.send (request);
						
				zmq::message_t reply;
				socket.recv (&reply);
			}
         }
      }
         
      if(!goal_line.empty())
      {
         for(int n{}; n < goal_line.size(); n++)
         {
            cout << " Goal line: " << goal_line[n].yDistance()  << ", ";
            if(goal_line[n].yDistance() < 0.5)
            {
               z = 0;
               break;
            }
         }
      }
      else if(goal_line.empty() && z == 0)
      {
         z = 1;
         num_laps++;
      }	
		
      cout << num_laps << endl;

      if(num_laps >= NUM_LAPS)
      {
            zmq::message_t request (4);
            memcpy (request.data (), "ARCCAR stop", 4);
            std::cout << "Sending Goal_line" << std::endl;
            socket.send (request);
                
            zmq::message_t reply;
            socket.recv (&reply);
      } 
   }
   return 0;
}

//-----------------------------------------------------------------------------
