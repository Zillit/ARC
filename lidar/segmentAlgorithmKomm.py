#!/usr/bin/env python3
import bluetooth
import time
import spidev
import zmq
import math



bd_addr = "00:06:66:03:A6:A5" #FireFly Blåtandsadress på LADAR-tornet
port = 1

# Initiera och anslut alla ZeroMQ-anslutningar
context = zmq.Context()
LIDARpub = context.socket(zmq.PUB)
LIDARpub.bind("tcp://*:2555")
LIDARsub = context.socket(zmq.SUB)
LIDARsub.connect("tcp://localhost:5569")
LIDARsub.setsockopt(zmq.SUBSCRIBE, b"")
CAMsub = context.socket(zmq.SUB)
CAMsub.connect("tcp://localhost:2505")
CAMsub.setsockopt(zmq.SUBSCRIBE, b"")

# Försök ansluta till LADAR-tornet via Blåtand, stäng
# ner all anslutning ifall det inte går
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

# Definiera alla globala variabler
lista =""
lista2 = []
i=0
speed = 168
stopThresh = 60
frontStopThresh = 40
startSpeed = 168
diff = 20
theta_min = 285 + diff
theta_max = 75 + diff

# Initiera SPI-anslutningarna till Styr- och Sensormodul
spi = spidev.SpiDev()
spi.open(0,0)
spi.mode = 0b00

spi_sens = spidev.SpiDev()
spi_sens.open(0,1) 
spi_sens.mode = 0b00


# Funktion för att hitta en målpunkt efter
# ett varvs läsningar från tornet
def get_target(lista):
        # Definiera de variabler som används i funktionen
        global theta_max
        global theta_min
        global stopThresh
        r = 0
        theta = diff
        maximus = len(lista)-1
        l_min = 100
        r_min = 100
        # Zoneangles är en 2D-lista med 10x3 element
        # Varje yttre element motsvarar ett spann på
        # 15 grader, från -75 till +75 grader riktat framåt
        # De tre inre elementen i varje yttre element är avstånd
        # till närmaste punkt i spannet, vinkel till punkten och
        # huruvida det är ett grönt hinder i punkten eller inte
        zoneangles = [[5096,theta_min+8,0],[5096,theta_min+23,0],[5096,theta_min+38,0],[5096,theta_min+53,0],[5096,theta_max-83,0],[5096,theta_max-68,0],[5096,theta_max-53,0],[5096,theta_max-38,0],[5096,theta_max-23,0],[5096,theta_max-8,0]]
        messageCam = "0 0 0"
        illegal_left_angle = 0
        illegal_right_angle = 0
        # messageCam tar in alla meddelanden från kameran
        # och sparar det senaste. Ifall det är ett grönt hinder framför
        # bilen sätts vinklarna till höger respektive vänster sida av hindret
        # som illegal_right_angle och illegal_left_angle
        while True:
                try:
                        messageCam=CAMsub.recv_string(zmq.DONTWAIT)
                except:
                        break
        if (messageCam.split()[0] == "ARCANG"):
                illegal_left_angle = int(messageCam.split()[1]) + diff + 360
                illegal_right_angle = int(messageCam.split()[2]) + diff + 360
        # En for-loop som går igenom varje LADAR-punkt i listan och hanterar den
        for i in range(maximus+1):
                dist = lista[i][0]
                arg = lista[i][1]
                # Ifall vinkeln på punkten är bakom bilen går loopen vidare
                # till nästa punkt
                if (theta_max + 30 < arg < theta_min - 30):
                        continue
                # Om vinkeln är till höger eller vänster om bilen +- 15 grader och
                # närmare än den hittills närmaste punkten i det spannet sätts
                # l_min till punktens avstånd
                elif ((theta_max + 30 > arg > theta_max) and dist < l_min):
                        l_min = dist
                elif ((theta_min - 30 < arg < theta_min) and dist < r_min):
                        r_min = dist
		# Ifall punkten är den hittills närmaste punkten i sitt vinkelspann
		# sparas dess avstånd och vinkel i zoneangles
                elif (theta_min <= arg < theta_min+15 and (zoneangles[0][0]>dist)): 
                        zoneangles[0][0] = dist
                        zoneangles[0][1] = arg
                elif (theta_min+15 <= arg < theta_min+30 and (zoneangles[1][0]>dist)): 
                        zoneangles[1][0] = dist
                        zoneangles[1][1] = arg
                elif (theta_min+30 <= arg < theta_min+45 and (zoneangles[2][0]>dist)):
                        zoneangles[2][0] = dist
                        zoneangles[2][1] = arg
                elif ((theta_min+45 <= arg) or (arg < theta_max-90) and (zoneangles[3][0]>dist)):
                        zoneangles[3][0] = dist
                        zoneangles[3][1] = arg
                elif (theta_max-90 <= arg < theta_max-75 and (zoneangles[4][0]>dist)):
                        zoneangles[4][0] = dist
                        zoneangles[4][1] = arg
                elif (theta_max-75 <= arg < theta_max-60 and (zoneangles[5][0]>dist)):
                        zoneangles[5][0] = dist
                        zoneangles[5][1] = arg
                elif (theta_max-60 <= arg < theta_max-45 and (zoneangles[6][0]>dist)):
                        zoneangles[6][0] = dist
                        zoneangles[6][1] = arg
                elif (theta_max-45 <= arg < theta_max-30 and (zoneangles[7][0]>dist)): 
                        zoneangles[7][0] = dist
                        zoneangles[7][1] = arg
                elif (theta_max-30 <= arg < theta_max-15 and (zoneangles[8][0]>dist)): 
                        zoneangles[8][0] = dist
                        zoneangles[8][1] = arg
                elif (theta_max-15 <= arg <= theta_max and (zoneangles[9][0]>dist)): 
                        zoneangles[9][0] = dist
                        zoneangles[9][1] = arg
        # Alla tio spann gås igenom och ifall de har ett grönt hinder i sig
        # flaggas deras tredje inre element i zoneangles till 1
        for kindex in range(10):
                if ((illegal_right_angle < zoneangles[kindex][1] < illegal_left_angle) or  (illegal_right_angle < zoneangles[kindex][1]+360 < illegal_left_angle)):
                        zoneangles[kindex][2] = 1
        # Tar fram punkten längst bort av alla spann i zoneangles, förutsatt att
        # spannet och dess närmaste granne på vardera sida inte har ett hinder i sig
        # och att punkten i dess grannspann åtminstone är på 3/8-dels avstånd av
        # punkten i aktuellt spann
        # Detta görs först för specialfallen första och sista yttre elementet i zoneangles
        # då dessa endast har en granne var och sedan i en for-loop för resterande spann
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
        # Ifall avståndet i något av de två mittenspannet är närmare än frontStopThresh
        # returneras avståndet 10, som stannar bilen i main-loopen
        if ((1 < zoneangles[4][0] < frontStopThresh) or (1 < zoneangles[5][0] < frontStopThresh)):
                #Greater than 1 to avoid the "1=infinity" problem with the LidarLite v3
                return (10,0)
        # Ifall bilen är för nära väggen och tänker köra in i den,
        # sikta iväg ifrån väggen istället
        elif ((l_min < 20) and (diff < theta < theta_max)):
                return (100,diff-2)
        elif ((r_min < 20) and (( theta < diff) or ( theta > theta_min))):
                return (100,diff+2) 
        # Ifall inga av de ovanstående undantagen skett, returnera avstånd och
        # vinkel till den ovan utvalda punkten
        else:   
                return (r, theta)

# Initierar Blåtandsanslutningen ifall den tappats
def bt_init():
        global bd_addr
        global port
        global sock
        while True:
                #Stäng socketen ifall detta inte gjorts
                try:
                        sock.close()
                except:
                        pass
                # Försök återansluta, skicka meddelande vidare och försök
                # igen ifall det inte går
                try:
                        sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
                        sock.connect((bd_addr, port))
                        sock.settimeout(5.0)
                        print("Connection Acquired 2")
                        break
                except Exception as e:
                        LIDARpub.send_string("%s %i %i" % ("LADAR" ,180 ,180 ))
                        print(e)
                        time.sleep(0.5)
                        continue

# Ta emot hastighet och kommando om manuell eller autonom styrning ska användas
def get_command(state):
        global speed
        command = ""
        temp = ""
        decision = ""
        # While-loop som tar emot alla inkommna meddelanden från
        # arc_ssh och sparar det sista
        while True:
                try:
                        command = LIDARsub.recv_string(zmq.DONTWAIT)                except:
                        break
        try:
                decision, maxspeed, temp = command.split()
        except:
                pass
        # Sätt hastighet och returnera True ifall autonom styrning
        # ska köras, annars stanna bilen och returnera False
        if (decision == "MODEAU"):
                speed = int(maxspeed)+158
                return True
        elif (decision == "MODEMA"):
                spi.xfer2([158],250000,1,8)
                spi.xfer2([53],250000,1,8)
                #print("Faalse")
                return False
        # Ifall inget, eller felaktigt, meddelande inkommit
        # returnera stadiet det tidigare var i
        else:
                return state

# Main-loopen
def main():
        # Definiera alla variabler som används
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
        Auto_bool = False 
        while True:
                # Läs in data från Blåtandsmottagaren och spara i strängen lista
                data = sock.recv(1024).decode("utf-8")
                if data:
                        lista += data 
                        while lista.find("\n") != -1:
                                # Kasta bort inkorrekt data, spara vinkel och avstånd annars
                                try:
                                        dist = int(lista[:lista.find(":")])	
                                        angle = int(lista[lista.find(":")+1:lista.find("\n")-1]) 
                                except ValueError:
                                        lista = lista[lista.find("\n")+1:]
                                        continue
                                # Lägg till vinkel och avstånd sist i lista2
                                lista2.append([dist, angle])
                                # Skicka vidare till arc_ssh ifall avståndet är större än 1
                                # (då det är Lidar Lite v3:s default-värde om inget avstånd
                                # lyckas uppmätas)
                                if (dist > 1):
                                        LIDARpub.send_string("%s %i %i" % ("LADAR", angle, dist))
                                # Ta bort sparad data ifrån strängen lista
                                lista = lista[lista.find("\n")+1:]
                                # Om vinkeln är bakom bilen, kontrollera ifall det är autonom
                                # eller manuell styrning som gäller
                                if ((170 < angle < 190) and (i > 20)):
                                        Auto_bool = get_command(Auto_bool)
                                        # Ifall manuell styrning, sätt räknaren till 0 och töm lista2
                                        if Auto_bool == False:
                                                i = 0
                                                lista2 = []
                                        else:
                                                # Hämta målvinkel och avstånd från get_target
                                                (targetDist, angular) = get_target(lista2)
                                                # Konvertera vinkel till korrekt stil för styrservot
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
                                                # Ifall avståndet till målet är för nära, stanna då bilen
                                                # Kör annars i satt hastighet.
                                                if (targetDist > stopThresh):   
                                                        spi.xfer2([speed],250000,1,8)
                                                else:
                                                        spi.xfer2([158],250000,1,8)
                                                # Skicka vinkeln till styrmodulen för att svänga        
                                                spi.xfer2([int(angular)],250000,1,8)
                                                # Skicka vidare målvinkel och avstånd till arc_ssh
                                                LIDARpub.send_string("%s %i %i \n" % ("DECION", int(angular), int(speed)))
                                                # Nollställ räknare och töm lista2
                                                i = 0
                                                lista2 = []
                                # Räkna upp räknaren annars
                                else:
                                        i += 1
                                                
        



        
# Kör main-loopen
if __name__ == '__main__':
        while True:
                try:
                        main()
                # Vid Blåtandsfel, försök stänga socketen
                # och anslut igen
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
                # Vid övriga fel, stäng ner allt.
                except Exception as e:
                        print("exited " + str(e))
                        spi.xfer2([158],250000,1,8)
                        spi.xfer2([53],250000,1,8)
                        sock.close()
                        LIDARpub.close()
                        LIDARsub.close()
                        context.term()






