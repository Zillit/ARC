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

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00

def get_target(lista):
        r = 0
        theta = 0
        maximus = len(lista)-1
        first = int(lista[0][0])
        last = int(lista[maximus][0])
        for i in range(len(lista)):
                dist = int(lista[i][0])
                arg = int(lista[i][1])
                if (i==0):
                        if ((45 > arg or arg > 315)  and last > distThresh and int(lista[1][0])>distThresh):
                                r = dist
                                theta = arg
                elif (i==maximus):
                        if ((45 > arg or arg > 315) and first > distThresh and int(lista[maximus-1][0])>distThresh):
                                r = dist
                                theta = arg
                elif (i != 0 and i !=maximus):
                        if (dist > r and (45 > arg or arg > 315) and int(lista[i-1][0])>distThresh and int(lista[i+1][0])>distThresh):
                                r = dist
                                theta = arg
                        
        print(r,theta)
        return theta


while True:
        data = sock.recv(1024).decode("utf-8") # Read Bluetooth buffer for Lidar data
        if data:
                lista += data 
                while lista.find("\n") != -1:
                        dist = lista[:lista.find(":")]	
                        angle = lista[lista.find(":")+1:lista.find("\n")-1] 
                        lista2.append([dist, angle])
                        lista = lista[lista.find("\n")+1:] 
                        if i < 100:
                                i += 1
                        else:
                                angular = get_target(lista2)
                                print(angular)
                                #spi.xfer2([int(0xF1, 10)],250000,1,8)
                                if (angular > 315):
                                        angular -= 315
                                else:
                                        angular + 45
                                spi.xfer2([int(angular/3)],250000,1,8)
                                i = 0
                                lista2= []
                                break
		
