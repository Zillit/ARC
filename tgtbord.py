import pygame
import sys
import time
import numpy as np
import spidev
import picamera 

import os

camera = picamera.PiCamera()
pygame.init()
surface = pygame.display.set_mode((500,500))
camera.start_preview()
camera.stop_preview()


done = False
count = True
degrees = 15
speed = 0
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00
resp = spi.xfer2([degrees],500000,5,8)

while not done:
    keys = pygame.key.get_pressed()
    while keys[pygame.K_RIGHT]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if degrees + delta < 30:
            degrees = degrees + delta
        else:
            degrees = 30.0
        resp = spi.xfer2([int(degrees)],500000,5,8)
        #print(int(degrees))
        keys = pygame.key.get_pressed()
        count = False
    resp = spi.xfer2([15],500000,5,8)
    degrees = 15;

    count = True;

    while keys[pygame.K_LEFT]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        keys = pygame.key.get_pressed()
        stop = time.time()
        delta = (stop - start)/30
        if degrees - delta > 0:
            degrees = degrees - delta
        else:
            degrees = 0.0
        resp = spi.xfer2([int(degrees)],500000,5,8)
        count = False
    resp = spi.xfer2([15],500000,5,8)    
    degrees = 15;

    count = True
    
    while keys[pygame.K_UP]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if speed + delta < 30:
            speed = speed + delta
        else:
            speed = 30.0
        resp = spi.xfer2([int(speed)+128],500000,5,8)
        keys = pygame.key.get_pressed()
        count = False
    count = True

    while keys[pygame.K_DOWN]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if speed - delta > 0:
            speed = speed - delta
        else:
            speed = 0
        resp = spi.xfer2([int(speed)+128],500000,5,8)
        keys = pygame.key.get_pressed()
        count = False
    count = True

    if keys[pygame.K_ESCAPE]:
        done = True   
    count = True
    pygame.event.pump()

degrees = 0
speed = 0
print("Du har parkerat bilen. Välkommen åter. Bilen svänger i {} grader och kör i {} km/h".format(degrees,speed))
