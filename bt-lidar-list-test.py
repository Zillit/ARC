import bluetooth
import time
import math
from tkinter import *

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


def getpos(point):
    distance = float(point[:point.find(":")])
    angle = float(point[point.find(":")+1:])
    x = get_x(distance/1, angle)
    y = get_y(distance/1, angle)
    return [x,y]
    

def getclosestpoint(lista):
    j = 0
    angle = 0
    for i in range(0,200):
        r = math.hypot(lista[i][0],lista[i][1])     # TODO: Minus bilens xy-pos
        theta = math.atan2(lista[i][1],lista[1][0])     # TODO: Minus bilens y-pos
        if( r>j and  -math.pi/4<theta<math.pi/4 ):
            j = r
            angle = theta
    return angle
        

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
sock.settimeout(5.0)
print("Connection Acquired")
lista =""
lista2 = []
i=0
for i in range(0,200):
    lista2.append([1000,1000])


while True:
    data = sock.recv(1024).decode("utf-8")
    if data:
        lista += data
    while lista.find("\n") != -1:
        lista2[i]=getpos(lista[:lista.find("\n")-1])
        lista = lista[lista.find("\n")+1:]
    if i < 199:
        i += 1
    else:
        i=0
        #print(lista2)
        print(getclosestpoint(lista2))





sock.close()
