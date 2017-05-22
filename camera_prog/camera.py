import math
import numpy as np
import cv2
import sys
import zmq
import defines

from picamera.array import PiRGBArray
from picamera import PiCamera
import time

context = zmq.Context()
CAMERApub = context.socket(zmq.PUB)
CAMERApub.bind("tcp://*:2505")
CAMERAsub = context.socket(zmq.SUB)
CAMERAsub.connect("tcp://localhost:2500")
CAMERAsub.setsockopt(zmq.SUBSCRIBE, b"")

z = 1

#
# Ett färgat objekt eller markering
#
class Rectangle:    
    def __init__(self, xL, yT, xR, yB):
        self.xL = xL
        self.yT = yT
        self.xR = xR
        self.yB = yB
    def area(self): 
        return ( self.yT - self.yB ) * ( self.xR - self.xL )
    def yT(self):
        return self.yT
    def yB(self):
        return self.yB
    def xL(self):
        return self.xL
    def xR(self):
        return self.xR
    #
    # Beräknar avståndet "rakt fram" till objektet
    #
    def yDistance(self):
        return defines.HEIGHT_OF_CAMERA * math.tan(((90 - defines.ANGEL_OF_CAMERA - (defines.VERTICAL_FOV/2)) + 
        (defines.VERTICAL_FOV/defines.PIXEL_HEIGHT * (defines.PIXEL_HEIGHT - self.yB)))*math.pi/180)
    #
    # Beräknar det totala avståndet till objektet
    #
    def distance(self, angle):
        return self.yDistance() / math.cos(angle * math.pi/180)
    #
    # Beräknar vinkeln till objektets närmsta hörn
    #    
    def angleClose(self): 
        left_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
        right_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))
        if (abs(left_angle) < abs(right_angle)): 
            return left_angle
        else: 
            return right_angle
    
    def closestDistance(self):
        if((self.leftAngle() <= 0) & (self.rightAngle() >= 0)):
            return self.yDistance()
        else:
            return self.distance(self.angleClose())
    
    def rightAngle(self):
        return -defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))

    def leftAngle(self):
        return -defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
    #
    # Beräknar vinkeln till objektets närmsta hörn
    #   
    def angleFar(self):
        left_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
        right_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))
        if (abs(left_angle) >= abs(right_angle)): 
            return left_angle
        else: 
            return right_angle

GOAL_COUNTER = 0

camera = PiCamera()
camera.resolution = (defines.PIXEL_WIDTH, defines.PIXEL_HEIGHT)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(defines.PIXEL_WIDTH, defines.PIXEL_HEIGHT))

time.sleep(0.1)


for img in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    try:
        messageCam=CAMERAsub.recv_string(zmq.DONTWAIT)
        if messageCam:
            print messageCam
            mes,laps, num = messageCam.split()
            GOAL_COUNTER = 0
    except:
        pass
    frame = img.array
    
    key = cv2.waitKey(1) & 0xFF
    #_, frame = cap.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    lower_blue = np.array([90,70,70])
    upper_blue = np.array([130,255,255])

    lower_green = np.array([40,50,50])
    upper_green = np.array([70,255,255])

    kernel = np.ones((5,5), np.uint8)
    
    imgthresh = cv2.inRange(hsv, lower_blue, upper_blue)
    img_green = cv2.inRange(hsv, lower_green, upper_green)

    img_green = cv2.erode(img_green, kernel, iterations=1)
    img_green = cv2.dilate(img_green, kernel, iterations=1)

    img_green = cv2.dilate(img_green, kernel, iterations=1)
    img_green = cv2.erode(img_green, kernel, iterations=1)

    img_green = cv2.medianBlur(img_green,5)
    
    imgthresh = cv2.erode(imgthresh, kernel, iterations=1)
    imgthresh = cv2.dilate(imgthresh, kernel, iterations=1)

    imgthresh = cv2.dilate(imgthresh, kernel, iterations=1)
    imgthresh = cv2.erode(imgthresh, kernel, iterations=1)

    imgthresh = cv2.medianBlur(imgthresh,5)

    contours, hierarchy = cv2.findContours(imgthresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours_green, hierarchy_green = cv2.findContours(img_green, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    #print contours
    objects = []
    objects_green = []
    
    for cnt in contours:
        x,y,w,h = cv2.boundingRect(cnt)
        tmp = Rectangle(x, defines.PIXEL_HEIGHT - y, x+w, defines.PIXEL_HEIGHT - (y+h))
        if (tmp.area() > 600):
            objects.append(tmp) 
            cv2.rectangle(imgthresh, (x,y), (x+w,y+h), (100,255,40), 2)

    for cnt in contours_green:
        x,y,w,h = cv2.boundingRect(cnt)
        tmp = Rectangle(x, defines.PIXEL_HEIGHT - y, x+w, defines.PIXEL_HEIGHT - (y+h))
        if (tmp.area() > 1000):
            objects_green.append(tmp) 
            cv2.rectangle(img_green, (x,y), (x+w,y+h), (100,255,40), 2)  
        
    cv2.drawContours(frame, contours, -1, (255,0,0), 3)
    cv2.drawContours(frame, contours_green, -1, (0,255,0), 3)
    cv2.imshow('frame',frame)
    cv2.imshow('imgthresh',imgthresh)
    cv2.imshow('green',img_green)
    #cv2.imshow('res',res)
    #cv2.imshow('Cont', contours)
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break
    
    if len(objects) != 0:
        for i in range(len(objects)):
            if objects[i].yB < defines.PIXEL_HEIGHT/10:
                z = 0
                break
    elif ((len(objects) == 0) & z == 0):
        z = 1
        GOAL_COUNTER = GOAL_COUNTER + 1
        
        #CAMERApub.send_string("%s %i %i \n" %("ARCCAM", GOAL_COUNTER, 100))
        print(GOAL_COUNTER)

    if len(objects_green) != 0:
         CAMERApub.send_string("%s %i %i \n" %("ARCCAM", objects_green[0].leftAngle() + 3, objects_green[0].rightAngle() - 3))

    rawCapture.truncate(0)

cv2.destroyAllWindows()
