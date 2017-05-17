import math
import numpy as np
import cv2
import sys
import zmq
import defines

context = zmq.Context()
CAMERApub = context.socket(zmq.PUB)
CAMERApub.bind("tcp://*:2505")

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
        return ( -self.yB + self.yT ) *( self.xR - self.xL )
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
        return defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))

    def leftAngle(self):
        return defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))    
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
cap = cv2.VideoCapture(0)

CAMERApub.send_string("%s %i %i" %("ARCCAM", 1, 0))

while(1):

    _, frame = cap.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_blue = np.array([38,50,50])
    upper_blue = np.array([75,255,255])
    imgthresh = cv2.inRange(hsv, lower_blue, upper_blue)
    
    kernel = np.ones((5,5), np.uint8)
    
    imgthresh = cv2.erode(imgthresh, kernel, iterations=1)
    imgthresh = cv2.dilate(imgthresh, kernel, iterations=1)

    imgthresh = cv2.dilate(imgthresh, kernel, iterations=1)
    imgthresh = cv2.erode(imgthresh, kernel, iterations=1)

    imgthresh = cv2.medianBlur(imgthresh,5)

    im2, contours, hierarchy = cv2.findContours(imgthresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    objects = []

    for cnt in contours:
        x,y,w,h = cv2.boundingRect(cnt)
        tmp = Rectangle(x, defines.PIXEL_HEIGHT - y, x+w, defines.PIXEL_HEIGHT - (y+h))
        #print(tmp.closestDistance())
        if (tmp.area() > 600):
            objects.append(tmp) 
            cv2.rectangle(imgthresh, (x,y), (x+w,y+h), (100,255,40), 2)
            
        
    cv2.drawContours(frame, contours, -1, (255,0,0), 3)

    cv2.imshow('frame',frame)
    cv2.imshow('imgthresh',imgthresh)
    #cv2.imshow('res',res)
    #cv2.imshow('Cont', contours)
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break
    
    if len(objects) != 0:
        for i in range(len(objects)):
            if objects[i].yB < 200:
                z = 0
                break
    elif ((len(objects) == 0) & z == 0):
        z = 1
        GOAL_COUNTER = GOAL_COUNTER + 1
        print(GOAL_COUNTER)
        CAMERApub.send_string("%s %i %i" %("ARCCAM", GOAL_COUNTER, 0))

    

cv2.destroyAllWindows()