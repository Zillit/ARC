#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "defines.h"
#include "hinder_det.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <zmq.hpp>

using namespace std;
using namespace cv;
using namespace ARC_CAMERA;
using namespace zmq;
zmq::context_t context (1);
zmq::socket_t requester (context, ZMQ_REQ);


//C based function for sending and reseving instructions.

static char *s_recv(void *socket)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strndup(buffer, sizeof(buffer) - 1);
}
static int s_send(void *socket, char *string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}
void sendMessage(string message)
{
    char *charBuf = message.c_str();
    s_send(requester, charBuf);
    char *response = s_recv(requester);
    string address, instruction;
    istringstream iss(response);
    iss >> address >> instruction;
    cout << "Address: " << address << " Instrucion: " << instruction << ", to message: " << message << ", as request number: " << REQUEST_NUMBER << endl;
}

int main()
{
   
 
   requester.connect ("tcp://localhost:5567");

   int lhsv[3] = {ILOWH, ILOWS, ILOWV};
   int hhsv[3] = {IHIGHH, IHIGHS, IHIGHV};
        
   int lb[] = {75, 100, 100};
   int hb[] = {130, 255, 255};
   
#ifdef __arm__
   raspicam::RaspiCam_Cv Camera;
   Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
   Camera.set(CV_CAP_PROP_FRAME_WIDTH, 320);
   
   if ( !Camera.open() )  
   {
      cout << "Cannot open the web cam on raspi" << endl;
      return -1;
   }

#else
   VideoCapture Camera(0);
      
   if ( !Camera.isOpened() )  
   {
      cout << "Cannot open the web cam on lap" << endl;
      return -1;
   }

#endif

   // Används för mållinje 
   int num_laps = 0;
   int z = 1;

   while(true)
   {   
      Mat imgOriginal;
                
      Camera.grab();
                
      Camera.retrieve(imgOriginal);
      
      Mat all_green = detectionOfColor(imgOriginal, lhsv, hhsv);
      Mat black = detectionOfColor(imgOriginal, lb, hb);
      //Mat lines = detectLines(imgOriginal);
      //vector<Vec4i> num_line = linesInImage(lines);
      /*
      for( size_t i = 0; i < num_line.size(); i++ )
      {
          Vec4i l = num_line[i];
          line( lines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
      }
      */             
      //imshow("all_green", all_green);
      imshow("imgOriginal", imgOriginal);
      imshow("Black", black);
      //imshow("Lines", lines);
      waitKey(30);
        
      vector<ColoredObject> object = framedObjects(all_green);

      for(int n = 0; n < object.size(); n++)
      {
         cout << ", object " << object[n].distance(object[n].angleClose());
            
         if(object[n].distance(object[n].angleClose()) < 2)
         {
			string string_to_send = to_string((int)round(100 * object[n].distance(object[n].angleClose())));
			string_to_send += " " + to_string((int)round(object[n].angleClose()));
			string_to_send += " " + to_string((int)round(object[n].angleFar()));
			
			cout << " String to send: " << string_to_send << endl;
			/*
			if(string_to_send.size() <= 32)
			{
				zmq::message_t request (string_to_send.size());
				memcpy (request.data (), "Object", string_to_send.size());
				std::cout << "Sending Object" << std::endl;
				socket.send (request);
						
				zmq::message_t reply;
				socket.recv (&reply);
			}
			*/
         }
      }
      
      //Viktigt för mållinje ---------------
      vector<ColoredObject> goal_line= framedObjects(black);
         
      if(!goal_line.empty())
      {
         for(int n{}; n < goal_line.size(); n++)
         {
            cout << " Goal line d,num: " << goal_line[n].yDistance()  << ", ";
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
         string message="ARCCAM " + to_string(num_laps) + " 0 /n";

         //memcpy (request.data (), "ARCCAM", 6);
         std::cout << "Sending Goal_line" << std::endl;
         sendMessage(message);
      }	
		
      cout << num_laps << endl;
      /*
      if(num_laps >= NUM_LAPS)
      {
            zmq::message_t request (6);
            memcpy (request.data (), "ARCCAM", 6);
            std::cout << "Sending Goal_line" << std::endl;
            socket.send (request);
                
            zmq::message_t reply;
            socket.recv (&reply);
      }
      */
      //----------------------------------------- 
      
   }
   return 0;
}

//-----------------------------------------------------------------------------

