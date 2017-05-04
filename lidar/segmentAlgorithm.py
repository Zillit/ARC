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
#distThresh = 40
stopThresh = 40
diff = 20 # Vilken funkar bäst? 18 eller 20 eller något annat?
theta_min = 285 + diff #315
theta_max = 75 + diff

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
        l_min = 100
        r_min = 100
        zoneangles = [[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0]]
        try:
                first = int(lista[0][0])
                last = int(lista[maximus][0])
        except ValueError:
                return (100,diff)
        for i in range(maximus+1):
                try:
                        dist = int(lista[i][0])
                        arg = int(lista[i][1])
                except ValueError:
                        print("Error: " + str(lista[i]))
                        continue
                if (theta_max + 30 < arg < theta_min - 30):
                        continue
                elif ((theta_max + 30 > arg > theta_max) and dist < l_min):
                        l_min = dist
                elif ((theta_min - 30 < arg < theta_min) and dist < r_min):
                        r_min = dist
		### TODO: Adaptive code that figures out adequate number of regions by itself, and puts them in a list or something
		### Range atm: 285-299, 300-314 etc, and 0-14, 15-29 etc
		### If the point is in the zone and closer than all earlier points in the zone, save it
                elif (theta_min <= arg < theta_min+15 and (zoneangles[0][0]>dist)): #Leftmost
                        zoneangles[0][0] = dist
                        zoneangles[0][1] = arg
                        #print("Max 0: " + str(dist))
                elif (theta_min+15 <= arg < theta_min+30 and (zoneangles[1][0]>dist)): 
                        zoneangles[1][0] = dist
                        zoneangles[1][1] = arg
                        #print("Max 1: " + str(dist))
                elif (theta_min+30 <= arg < theta_min+45 and (zoneangles[2][0]>dist)):
                        zoneangles[2][0] = dist
                        zoneangles[2][1] = arg
                        #print("Max 2: " + str(dist))
                elif ((theta_min+45 <= arg) or (arg < theta_max-90) and (zoneangles[3][0]>dist)):
                        zoneangles[3][0] = dist
                        zoneangles[3][1] = arg
                        #print("Max 3: " + str(dist))
                elif (theta_max-90 <= arg < theta_max-75 and (zoneangles[4][0]>dist)):
                        zoneangles[4][0] = dist
                        zoneangles[4][1] = arg
                        #print("Max 4: " + str(dist))
                elif (theta_max-75 <= arg < theta_max-60 and (zoneangles[5][0]>dist)):
                        zoneangles[5][0] = dist
                        zoneangles[5][1] = arg
                        #print("Max 5: " + str(dist))
                elif (theta_max-60 <= arg < theta_max-45 and (zoneangles[6][0]>dist)):
                        zoneangles[6][0] = dist
                        zoneangles[6][1] = arg
                        #print("Max 6: " + str(dist)) 
                elif (theta_max-45 <= arg < theta_max-30 and (zoneangles[7][0]>dist)): 
                        zoneangles[7][0] = dist
                        zoneangles[7][1] = arg
                        #print("Max 7: " + str(dist))
                elif (theta_max-30 <= arg < theta_max-15 and (zoneangles[8][0]>dist)): 
                        zoneangles[8][0] = dist
                        zoneangles[8][1] = arg
                        #print("Max 8: " + str(dist))
                elif (theta_max-15 <= arg <= theta_max and (zoneangles[9][0]>dist)): #Rightmost
                        zoneangles[9][0] = dist
                        zoneangles[9][1] = arg
                        #print("Max 9: " + str(dist))
		#### Turns away from a wall if it is too close
        #if ((l_min < 20) and (diff < theta < theta_max)):
                #print("Left")
                #return diff-5
        #elif ((r_min < 20) and (( theta < diff) or ( theta > theta_min))):
                #print("Right")
                #return diff+5
        #else:
        #print(zoneangles)
        for jindex in range(len(zoneangles)):
                if (zoneangles[jindex][0]>r and zoneangles[jindex][0] !=5096 ):
                        r = zoneangles[jindex][0]
                        theta = zoneangles[jindex][1]
                #print("Chosen angle: " + str(theta))
                #print("Chosen distance: "+ str(r))
        if ((1 < zoneangles[4][0] < stopThresh) or (1 < zoneangles[5][0] < stopThresh)):
                #Greater than 1 to avoid the "1=infinity" problem witht he LidarLite v3
                return (10,0)
        elif ((l_min < 20) and (diff < theta < theta_max)):
                #print("Left")
                return (100,diff-5)
        elif ((r_min < 20) and (( theta < diff) or ( theta > theta_min))):
                #print("Right")
                return (100,diff+5) 
        else:      
                return (r, theta)


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
                                (targetDist, angular) = get_target(lista2)
                                ###   Hastighet   ###
                                if (targetDist > stopThresh):
                                        spi.xfer2([138],250000,1,8)
                                else:
                                        spi.xfer2([128],250000,1,8)
                                        ###STANNA BILJÄVELN###
                                #TODO: Speed = maxspeed*(1-abs(1-angularFörNollFörNoll))*r*KONSTANT
                                #Ovan låter hastigheten sänkas ju mer man svänger och ju nämare hindret är
                                if (angular > theta_min):
                                        angular -= theta_min
                                else:
                                        angular += (150-theta_max)
                                angular = 150 - angular
                                if (angular > 130):
                                        angular = 100
                                elif (angular < 20):
                                        angular = 6
                                else:
                                        angular = int(angular*47/55-11)
                                        
                                #print(angular)
                                spi.xfer2([int(angular)],250000,1,8)
                                #resp = spi_sens.xfer2([0xFF,0,0,0,0],125000,1,8)
                                #print(resp)
                                i = 0
                                lista2= []
                                #break
