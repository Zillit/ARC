#!/usr/bin/env python3
import bluetooth
import time
import spidev
import zmq
import math
#from multiprocessing import Process, Value


bd_addr = "00:06:66:03:A6:A5" #FireFly Bluetooth adress on Lidar tower
port = 1


context = zmq.Context()
###sender = context.socket(zmq.PUB)
###sender.bind("tcp://*:5565")
###ID = int(10001) # ID for the ZMQ publisher
#LIDARpub connects to arc_ssh.py
LIDARpub = context.socket(zmq.PUB)
LIDARpub.bind("tcp://*:2555")
LIDARsub = context.socket(zmq.SUB)
LIDARsub.connect("tcp://localhost:5569")
LIDARsub.setsockopt(zmq.SUBSCRIBE, b"")
CAMsub = context.socket(zmq.SUB)
CAMsub.connect("tcp://localhost:2505")
CAMsub.setsockopt(zmq.SUBSCRIBE, b"")

try:
        sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
        sock.connect((bd_addr, port))
        sock.settimeout(5.0)
        print("Connection Acquired 1")
except:
        sock.close()
        LIDARpub.close()
        LIDARsub.close()
        context.term()
lista =""
lista2 = []
i=0
#Auto_bool = False
#Auto_bool = True
#distThresh = 40
speed = 168
stopThresh = 60
frontStopThresh = 40
startSpeed = 168
controlConst = 0.25
closeConst = 1
diff = 20 # Vilken funkar bäst? 18 eller 20 eller något annat?
theta_min = 285 + diff
theta_max = 75 + diff

spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00

spi_sens = spidev.SpiDev()
spi_sens.open(0,1) 
spi_sens.mode = 0b00

def get_target(lista):
        global theta_max
        global theta_min
        global stopThresh
        r = 0
        theta = diff
        maximus = len(lista)-1
        l_min = 100
        r_min = 100
        zoneangles = [[5096,theta_min+8,0],[5096,theta_min+23,0],[5096,theta_min+38,0],[5096,theta_min+53,0],[5096,theta_max-83,0],[5096,theta_max-68,0],[5096,theta_max-53,0],[5096,theta_max-38,0],[5096,theta_max-23,0],[5096,theta_max-8,0]]
        messageCam = "0 0 0"
        illegal_left_angle = 0
        illegal_right_angle = 0
        while True:
                try:
                        messageCam=CAMsub.recv_string(zmq.DONTWAIT)
                       # print(messageCam)
                except:
                        #print("Break")
                        break
        if (messageCam.split()[0] == "ARCANG"):
                illegal_left_angle = int(messageCam.split()[1]) + diff + 360
                illegal_right_angle = int(messageCam.split()[2]) + diff + 360
        #if 
        #zoneangles = [[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0],[5096,0]]
        for i in range(maximus+1):
                dist = lista[i][0]
                arg = lista[i][1]
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
                        #zoneangles[0][1] = arg
                elif (theta_min+15 <= arg < theta_min+30 and (zoneangles[1][0]>dist)): 
                        zoneangles[1][0] = dist
                        #zoneangles[1][1] = arg
                elif (theta_min+30 <= arg < theta_min+45 and (zoneangles[2][0]>dist)):
                        zoneangles[2][0] = dist
                        #zoneangles[2][1] = arg
                elif ((theta_min+45 <= arg) or (arg < theta_max-90) and (zoneangles[3][0]>dist)):
                        zoneangles[3][0] = dist
                        #zoneangles[3][1] = arg
                elif (theta_max-90 <= arg < theta_max-75 and (zoneangles[4][0]>dist)):
                        zoneangles[4][0] = dist
                        #zoneangles[4][1] = arg
                elif (theta_max-75 <= arg < theta_max-60 and (zoneangles[5][0]>dist)):
                        zoneangles[5][0] = dist
                        #zoneangles[5][1] = arg
                elif (theta_max-60 <= arg < theta_max-45 and (zoneangles[6][0]>dist)):
                        zoneangles[6][0] = dist
                        #zoneangles[6][1] = arg
                elif (theta_max-45 <= arg < theta_max-30 and (zoneangles[7][0]>dist)): 
                        zoneangles[7][0] = dist
                        #zoneangles[7][1] = arg
                elif (theta_max-30 <= arg < theta_max-15 and (zoneangles[8][0]>dist)): 
                        zoneangles[8][0] = dist
                        #zoneangles[8][1] = arg
                elif (theta_max-15 <= arg <= theta_max and (zoneangles[9][0]>dist)): #Rightmost
                        zoneangles[9][0] = dist
                        #zoneangles[9][1] = arg
	#### Turns away from a wall if it is too close
        for kindex in range(10):
                if ((illegal_right_angle < zoneangles[kindex][1] < illegal_left_angle) or  (illegal_right_angle < zoneangles[kindex][1]+360 < illegal_left_angle)):
                        zoneangles[kindex][2] = 1
        if (zoneangles[0][0] > r and zoneangles[0][0] !=5096 and zoneangles[1][0] > int(zoneangles[0][0]*3/8) and ((zoneangles[0][2] + zoneangles[1][2]) == 0)):#int(zoneangles[0][0]/2)):
                r = zoneangles[0][0]
                theta = zoneangles[0][1]
        if (zoneangles[9][0] > r and zoneangles[9][0] !=5096 and zoneangles[8][0] > int(zoneangles[9][0]*3/8) and ((zoneangles[8][2] + zoneangles[9][2]) == 0)):
                r = zoneangles[9][0]
                theta = zoneangles[9][1]
        for jindex in range(1,9):
                if (zoneangles[jindex][0]>r and zoneangles[jindex][0] !=5096 and zoneangles[jindex-1][0] > int(zoneangles[jindex][0]*3/8) and zoneangles[jindex+1][0] > int(zoneangles[jindex][0]*3/8) and ((zoneangles[jindex-1][2] + zoneangles[jindex][2] + zoneangles[jindex+1][2]) == 0)):
                        r = zoneangles[jindex][0]
                        theta = zoneangles[jindex][1]
        if ((1 < zoneangles[4][0] < frontStopThresh) or (1 < zoneangles[5][0] < frontStopThresh)):
                #Greater than 1 to avoid the "1=infinity" problem with the LidarLite v3
                return (10,0)
        elif ((l_min < 20) and (diff < theta < theta_max)):
                return (100,diff-2)
        elif ((r_min < 20) and (( theta < diff) or ( theta > theta_min))):
                return (100,diff+2) 
        else:   
                return (r, theta)

def bt_init():
        while True:
                try:
                        sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
                        sock.connect((bd_addr, port))
                        sock.settimeout(5.0)
                        print("Connection Acquired 2")
                        break
                except Exception as e:
                        LIDARpub.send_string("%s %i %i" % ("LADAR" ,180 ,180 ))
                        print(e)
                        continue
        
def get_command(state):
        global speed
        command = ""
        temp = ""
        decision = ""
        while True:
                try:
                        command = LIDARsub.recv_string(zmq.DONTWAIT)
                        #print(command)
                except:
                        #print("Break")
                        break
        try:
                decision, maxspeed, temp = command.split()
        except:
                pass
        #speed = int(maxspeed)
        if (decision == "MODEAU"):
                #print("Truu")
                speed = int(maxspeed)+158
                return True
        elif (decision == "MODEMA"):
                spi.xfer2([158],250000,1,8)
                spi.xfer2([53],250000,1,8)
                #print("Faalse")
                return False
        else:
                return state


def main():
        global lista
        global lista2
        global i
        global Auto_bool
        global stopThresh
        global startSpeed
        global controlConst
        global speed
        global diff
        global theta_min
        global theta_max
        dist=0
        angle=0
        Auto_bool = False #ENDAST FÖR TEST, ÄNDRA TILL FALSE
        while True:
                data = sock.recv(1024).decode("utf-8") # Read Bluetooth buffer for Lidar data
                if data:
                        lista += data 
                        while lista.find("\n") != -1:
                                try:
                                        dist = int(lista[:lista.find(":")])	
                                        angle = int(lista[lista.find(":")+1:lista.find("\n")-1]) 
                                except ValueError:
                                        lista = lista[lista.find("\n")+1:]
                                        #print(dist, angle)
                                        continue
                                lista2.append([dist, angle])
                                if (dist > 1):
                                        LIDARpub.send_string("%s %i %i" % ("LADAR", angle, dist))
                                lista = lista[lista.find("\n")+1:] 
                                if ((170 < angle < 190) and (i > 20)):
                                        Auto_bool = get_command(Auto_bool)
                                        #print(Auto_bool)
                                        if Auto_bool == False:
                                                i = 0
                                                lista2 = []
                                                #print("loop")
                                        else:
                                                (targetDist, angular) = get_target(lista2)
                                                ###   Hastighet   ###
                                                #if (targetDist > stopThresh):
                                                 #       spi.xfer2([138],250000,1,8)
                                                #else:
                                                #        spi.xfer2([128],250000,1,8)
                                                #        ###STANNA BILJÄVELN###
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
                                                #if (targetDist < 70):                   #avstånd mindre än 70  =>  hjulens vinklar spelar mindre roll
                                                        #speed = int(startSpeed + 20/(1 + (speed - startSpeed)/1) * (1 - math.pow(abs(53-angular)/60, closeConst) *min(targetDist,500)/500))*controlConst)
                                                #else:
                                                 #       speed = int(startSpeed + 20/(1 + (speed - startSpeed)/1) * (1 - min(targetDist,500)/500*abs(53-angular)/60)*controlConst)



                                                #speed = int(startSpeed + 20*(1 - abs(53-angular)/60 *min(targetDist,500)/500)*controlConst)
                                                #Nästan lite för försiktig ---   #speed = int(startSpeed + 20/(1 + speed - startSpeed) * (1 - min(targetDist,500)/500*abs(53-angular)/60)*controlConst)
                                                
                                                 
                                                #speed = 168    
                                                        ###   Hastighet   ###
                                                if (targetDist > stopThresh):   
                                                        spi.xfer2([speed],250000,1,8)
                                                else:
                                                        spi.xfer2([158],250000,1,8)
                                                        ###STANNA BILJÄVELN###
                                                
                                                spi.xfer2([int(angular)],250000,1,8)
                                                LIDARpub.send_string("%s %i %i \n" % ("DECION", int(angular), int(speed)))
                                                i = 0
                                                lista2 = []
                                else:
                                        i += 1
                                                
        



        

'''
def main():
        auto_loop()
'''

if __name__ == '__main__':
        while True:
                try:
                        main()
                except bluetooth.btcommon.BluetoothError:
                        try:
                                sock.close()
                                print("BT-closed")
                        except:
                                print("BT-pass")
                                pass
                        spi.xfer2([158],250000,1,8)
                        spi.xfer2([53],250000,1,8)
                        bt_init()
                        continue
                except Exception as e:
                        print("exited " + str(e))
                        spi.xfer2([158],250000,1,8)
                        spi.xfer2([53],250000,1,8)
                        sock.close()
                        LIDARpub.close()
                        LIDARsub.close()
                        context.term()






