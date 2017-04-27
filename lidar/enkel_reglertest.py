import bluetooth
import time
import math
from tkinter import *
import sys
import time
import numpy as np
import spidev
import picamera 
import os

done = False
count = True
degrees = 15
speed = 0
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00
resp = spi.xfer2([degrees],500000,5,8)

bd_addr = "00:06:66:03:A6:A5"
port = 1


def get_x(distance, angle):
    try:
        wasd = distance*math.cos(math.radians(angle))
    except:
        wasd = float(99)
    return wasd


def get_y(distance,angle):
    try:
        wasd = -distance*math.sin(math.radians(angle))
    except:
        wasd = float(99)
    return wasd


def getangle(point):
    distance = float(point[:point.find(":")])
    angle = float(point[point.find(":")+1:])
    return angle
        

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
sock.settimeout(5.0)
print("Connection Acquired")
lista =""
lista2 = []
i=0
for i in range(0,200):
    lista2.append(1000)


while True:
    data = sock.recv(1024).decode("utf-8")
    if data:
        lista += data
    while lista.find("\n") != -1:
        lista2[i]=getangle(lista[:lista.find("\n")-1])
        lista = lista[lista.find("\n")+1:]
    if i < 199:
        i += 1
    else:
        i=0
        intList = [ int(x) for x in lista2]
        leftList = [x+270 for x in intList]
        rightList = [x+90 for x in intList]
        leftD = min(intList, key=lambda x:abs(x))
        rightD = min(intList, key=lambda x:abs(x))
        l = leftD+rightD
        print(str(leftD) + ' : ' + str(rightD) + ' : ' + str(l))





sock.close()
