##!/usr/bin/env python3
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
#distThresh = 40
diff = 20 # Vilken funkar bäst? 18 eller 20 eller något annat?
theta_min = 315 + diff
theta_max = 45 + diff

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00

spi_sens = spidev.SpiDev()
spi_sens.open(0,1) # Har inte testat att CS1 funkar men det borde den göra
spi_sens.mode = 0b00

def get_target(lista):
        r = 0
        theta = 0
        maximus = len(lista)-1
        #l_max = 0
        #r_max = 0
        try:
                first = int(lista[0][0])
                last = int(lista[maximus][0])
                #print("Conversion successfull")
        except ValueError:
                print("Conversion failed")
                return theta_max - 45
        for i in range(maximus+1):
                try:
                        dist = int(lista[i][0])
                        arg = int(lista[i][1])
                except ValueError:
                        continue
                distThresh = dist*3/4
                #if ((theta_max + 45 > arg > theta_max) and dist > l_max):
                        #l_max = dist
                #elif((theta_min - 45 < arg < theta_min) and dist > r_max):
                        #r_max = dist
                #elif (i==0):
                if (i==0):
                        if (dist > r and (theta_max > arg or arg > theta_min)  and int(lista[maximus-1][0]) > distThresh and int(lista[2][0])>distThresh and int(lista[maximus-2][0]) > distThresh and int(lista[3][0]) > distThresh):
                                r = dist
                                theta = arg
                elif (i==1):
                        if (dist > r and (theta_max > arg or arg > theta_min)  and int(lista[3][0]) > distThresh and int(lista[maximus-1][0])>distThresh and last > distThresh and int(lista[4][0]) > distThresh):
                                r = dist
                                theta = arg
                elif (i==maximus):
                        if (dist > r and (theta_max > arg or arg > theta_min) and int(lista[2][0]) > distThresh and int(lista[maximus-3][0])>distThresh and int(lista[maximus-2][0]) > distThresh and int(lista[1][0]) > distThresh):
                                r = dist
                                theta = arg
                elif (i==maximus-1):
                        if (dist > r and (theta_max > arg or arg > theta_min) and first > distThresh and int(lista[maximus-4][0]) > distThresh and int(lista[maximus-3][0])>distThresh and int(lista[1][0]) > distThresh ):
                                r = dist
                                theta = arg
                elif (i==maximus-2):
                        if(dist > r and (theta_max > arg or arg > theta_min) and first > distThresh and last > distThresh and int(lista[maximus-4][0]) > distThresh and int(lista[maximus-5][0]) > distThresh ):
                                r = dist
                                theta = arg
                elif (i==2):
                        if (dist > r and (theta_max > arg or arg > theta_min)  and int(lista[4][0]) > distThresh and first >distThresh and last > distThresh and int(lista[5][0]) > distThresh):
                                r = dist
                                theta = arg
                elif (i != 0 and i !=maximus):
                        if (dist > r and (theta_max > arg or arg > theta_min) and int(lista[i-2][0])>distThresh and int(lista[i+2][0])>distThresh and int(lista[i-3][0])>distThresh and int(lista[i+3][0])>distThresh ):
                                r = dist
                                theta = arg
                        
        return theta
        #if (l_max > 2*r):
                #return theta_max
        #elif (r_max > 2*r):
                #return theta_min
        #else:
                #print(r,theta)
                #return theta


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
                                angular = get_target(lista2)
                                #print(lista2)
                                spi.xfer2([138],250000,1,8)
                                if (angular > theta_min):
                                        angular -= theta_min
                                else:
                                        angular += (90-theta_max)
                                #print(angular)
                                angular = 90 - angular
                                spi.xfer2([int(angular/3)],250000,1,8)
                                #print(angular)
                                #resp = spi_sens.xfer2([0xFF,0,0,0,0],125000,1,8)
                                #print(resp)
                                i = 0
                                lista2= []
                                #break
