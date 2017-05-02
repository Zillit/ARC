#!/usr/bin/env python3
import bluetooth
import time
import spidev


bd_addr = "00:06:66:03:A6:A5" #FireFly Bluetooth adress on Lidar tower
port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
sock.settimeout(5.0)
print("Connection Acquired")
lista =""
lista2 = []
i=0
distThresh = 40
leftMin = 225
leftMax = 315
rightMin = 45
rightMax = 135
P = 1

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00
spi.xfer2([15],250000,1,8)

def getLeft(lista):
        r = 100000
        for i in range(len(lista)):
                dist = int(lista[i][0])
                arg = int(lista[i][1])
                if (dist < r and (leftMax> arg and arg > leftMin)):
                        r = dist
                        theta = arg
                        
        #print(r,theta)
        return r

def getRight(lista):
        r = 100000
        for i in range(len(lista)):
                dist = int(lista[i][0])
                arg = int(lista[i][1])
                if (dist < r and (rightMax> arg and arg > rightMin)):
                        r = dist
                        theta = arg
        #print(r,theta)
        return r


while True:
        data = sock.recv(1024).decode("utf-8") # Read Bluetooth buffer for Lidar data
        if data:
                lista += data 
                while lista.find("\n") != -1:
                        dist = lista[:lista.find(":")]	
                        angle = lista[lista.find(":")+1:lista.find("\n")-1] 
                        lista2.append([dist, angle])
                        lista = lista[lista.find("\n")+1:] 
                        if i < 150:
                                i += 1
                        else:
                                leftD = getLeft(lista2)
                                rightD = getRight(lista2)
                                spi.xfer2([139],250000,1,8)
                                if leftD<rightD:
                                        degree = 15 - min((leftD-rightD)*P,15)
                                else:
                                        degree = 15 + min((rightD-leftD)*P,15)

                                spi.xfer2([int(degree)],250000,1,8)
                                i = 0
                                lista2= []
                                break
		
