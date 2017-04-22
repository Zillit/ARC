import pygame
import sys
import time
import numpy as np
import spidev


pygame.init()
surface = pygame.display.set_mode((500,500))

done = False
count = True
degrees = 15
speed = 0
spi = spidev.SpiDev()
spi.open(0,0)
resp = spi.xfer([degrees,0xF])

while not done:
    keys = pygame.key.get_pressed()
    while keys[pygame.K_d]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if degrees + delta < 30:
            degrees = degrees + delta
        else:
            degrees = 30.0
        resp = spi.xfer([int(degrees),0xF])
        #print(int(degrees))
        keys = pygame.key.get_pressed()
        count = False

    count = True;

    while keys[pygame.K_a]:
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
        resp = spi.xfer([int(degrees),0xF])
        count = False

    while keys[pygame.K_w]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if speed + delta < 30:
            speed = speed + delta
        else:
            speed = 30.0
        resp = spi.xfer([int(speed)+64,0xF])
        keys = pygame.key.get_pressed()
        count = False
    count = True 

    while keys[pygame.K_s]:
        if count == True:
            start = time.time()
        pygame.event.pump()
        stop = time.time()
        delta = (stop - start)/30
        if speed - delta > 0:
            speed = speed - delta
        else:
            speed = 0
        resp = spi.xfer([int(speed)+64,0xF])
        keys = pygame.key.get_pressed()
        count = False

    if keys[pygame.K_ESCAPE]:
        done = True   
    count = True
    pygame.event.pump()

degrees = 0
speed = 0
print("Du har parkerat bilen. Välkommen åter. Bilen svänger i {} grader och kör i {} km/h".format(degrees,speed))
