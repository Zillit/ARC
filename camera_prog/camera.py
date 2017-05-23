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
<<<<<<< HEAD
# Ett fergat objekt eller markering
=======
# Ett fargat objekt eller markering
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
#
class Rectangle:    
    def __init__(self, xL, yT, xR, yB):
        self.xL = xL
        self.yT = yT
        self.xR = xR
        self.yB = yB
    # 
    # Area berakning
    #
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
<<<<<<< HEAD
    # Bereknar avstandet "rakt fram" till objektet
=======
    # Beraknar avstandet "rakt fram" till objektet
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
    #
    def yDistance(self):
        return defines.HEIGHT_OF_CAMERA * math.tan(((90 - defines.ANGEL_OF_CAMERA - (defines.VERTICAL_FOV/2)) + 
        (defines.VERTICAL_FOV/defines.PIXEL_HEIGHT * (defines.PIXEL_HEIGHT - self.yB)))*math.pi/180)
    #
<<<<<<< HEAD
    # Bereknar det totala avstandet till objektet
=======
    # Beraknar det totala avstandet till objektet
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
    #
    def distance(self, angle):
        return self.yDistance() / math.cos(angle * math.pi/180)
    #
<<<<<<< HEAD
    # Bereknar vinkeln till objektets nermsta horn
=======
    # Beraknar vinkeln till objektets narmsta horn
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
    #    
    def angleClose(self): 
        left_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
        right_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))
        if (abs(left_angle) < abs(right_angle)): 
            return left_angle
        else: 
            return right_angle
    
    #
    # Beraknar narmsta avstand till hinder
    #
    def closestDistance(self):
        if((self.leftAngle() <= 0) & (self.rightAngle() >= 0)):
            return self.yDistance()
        else:
            return self.distance(self.angleClose())
    
    # 
    # Beraknar vinkeln till hogra hornet av hinder
    #
    def rightAngle(self):
        return -defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))

    #
    # Beraknar vinkeln till vanstra hornet av hinder
    def leftAngle(self):
        return -defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
    #
<<<<<<< HEAD
    # Bereknar vinkeln till objektets narmsta horn
=======
    # Beraknar vinkeln till objektets narmsta horn
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
    #   
    def angleFar(self):
        left_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xL - (defines.PIXEL_WIDTH/2))
        right_angle = defines.HORIZONTAL_FOV/defines.PIXEL_WIDTH * (self.xR - (defines.PIXEL_WIDTH/2))
        if (abs(left_angle) >= abs(right_angle)): 
            return left_angle
        else: 
            return right_angle

# Antelet var bilen har akt
GOAL_COUNTER = 0

camera = PiCamera()
camera.resolution = (defines.PIXEL_WIDTH, defines.PIXEL_HEIGHT)
camera.framerate = 60
rawCapture = PiRGBArray(camera, size=(defines.PIXEL_WIDTH, defines.PIXEL_HEIGHT))

time.sleep(0.1)

# Kontrasvilkoren for mallinje
lower_goalline = np.array([90,70,70])
upper_goalline = np.array([130,255,255])

# Kontrasvilkoren for hinder(object)
lower_object = np.array([40,50,50])
upper_object = np.array([70,255,255])

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
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
<<<<<<< HEAD
    lower_blue = np.array([160,70,70])
    upper_blue = np.array([180,255,255])

    lower_green = np.array([40,50,50])
    upper_green = np.array([70,255,255])

=======
>>>>>>> 1ff52fbc789e5e09f3c803f40f94086b2b65e085
    kernel = np.ones((5,5), np.uint8)
    
    # Tar ut farger i kamerabild enligt kontrasvilkoren
    img_goalline = cv2.inRange(hsv, lower_goalline, upper_goalline)
    img_object = cv2.inRange(hsv, lower_object, upper_object)

    img_object = cv2.erode(img_object, kernel, iterations=1)
    img_object = cv2.dilate(img_object, kernel, iterations=1)

    img_object = cv2.dilate(img_object, kernel, iterations=1)
    img_object = cv2.erode(img_object, kernel, iterations=1)

    img_object = cv2.medianBlur(img_object,5)
    
    img_goalline = cv2.erode(img_goalline, kernel, iterations=1)
    img_goalline = cv2.dilate(img_goalline, kernel, iterations=1)

    img_goalline = cv2.dilate(img_goalline, kernel, iterations=1)
    img_goalline = cv2.erode(img_goalline, kernel, iterations=1)

    img_goalline = cv2.medianBlur(img_goalline,5)

    # Konturberakning for yter
    contours, hierarchy = cv2.findContours(img_goalline, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours_object, hierarchy_object = cv2.findContours(img_object, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    rect_objects = []
    rect_goalline = []
    
    for cnt in contours:
        x,y,w,h = cv2.boundingRect(cnt)
        tmp = Rectangle(x, defines.PIXEL_HEIGHT - y, x+w, defines.PIXEL_HEIGHT - (y+h))
        if (tmp.area() > 600):
            rect_objects.append(tmp) 
            cv2.rectangle(img_goalline, (x,y), (x+w,y+h), (100,255,40), 2)

    for cnt in contours_object:
        x,y,w,h = cv2.boundingRect(cnt)
        tmp = Rectangle(x, defines.PIXEL_HEIGHT - y, x+w, defines.PIXEL_HEIGHT - (y+h))
        if (tmp.area() > 600):
            rect_goalline.append(tmp) 
            cv2.rectangle(img_object, (x,y), (x+w,y+h), (100,255,40), 2)  
        
    cv2.drawContours(frame, contours, -1, (255,0,0), 3)
    cv2.drawContours(frame, contours_object, -1, (0,255,0), 3)
    cv2.imshow('frame',frame)
    cv2.imshow('img_goalline',img_goalline)
    cv2.imshow('object',img_object)
    k = cv2.waitKey(5) & 0xFF
    
    if k == 27:
        break
    
    #Har ar algoritmen for att bestamma passerade mallinjer
    if len(rect_goalline) != 0:
        for i in range(len(rect_goalline)):
            if rect_goalline[i].yB < defines.PIXEL_HEIGHT/10:
                z = 0
                break
    elif ((len(rect_goalline) == 0) & z == 0):
        z = 1
        GOAL_COUNTER = GOAL_COUNTER + 1
        CAMERApub.send_string("%s %i %i \n" %("ARCCAM", GOAL_COUNTER, 100))
        print(GOAL_COUNTER)

    # Skickar vinklar fran hinder till arc_ssh
    if len(rect_objects) != 0:
         CAMERApub.send_string("%s %i %i \n" %("ARCANG", rect_objects[0].leftAngle() + 3, rect_objects[0].rightAngle() - 3))

    rawCapture.truncate(0)

cv2.destroyAllWindows()
