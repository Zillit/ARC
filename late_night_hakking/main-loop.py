#!/usr/bin/env python3
#import sys 
import time
import spidev
import zmq

context =  zmq.Context()

lidar_sub = context.socket(zmq.SUB) # Subscribe till lidar-data
lidar_sub.connect("tcp://localhost:5556")
lidar_sub.setsockopt_string(zmq.SUBSCRIBE, "10001")
LADARpub = context.socket(zmq.PUB)
LADARpub.bind("tcp://*:5565")
LADAR.setsockopt_string(zmq.PUBLISH, "10001".encode('ascii'))

# Utkommenterad kamerakod då det inte finns någon 
# kamera-publisher implementerad ännu
'''
cam_sub = context.socket(zmq.SUB) # Subscribe till kamera-data
cam_sub.connect("tcp://localhost:5557")
lidar_sub.setsockopt_string(zmq.SUBSCRIBE, "10001")
'''

spi_pair = context.socket(zmq.PAIR) # Bind PAIR med SPI
spi_pair.bind("tcp://*:5558")

cam_list = [] # tom kamera-lista
lidar_list = [] # tom lista för lidar-data
i=0 # start i listan
for j in range(0,200): # fyll med 200 element
    lidar_list.append([1000,1000])

	
#Kan vara bra att implementera den här skiten också (͡°͜ʖ͡°)	
#def get_target(lista):
	#kaowdokwaodw
	#dlkplapwlda
	#return angle
def get_target(lista):
	r = 0
	theta = 0
	for i in range(len(lista)):
		dist = lista[i][0]
		arg = lista[i][1]
		if (dist > r and (arg < 45 or arg > 315)):
			r = dist
			theta = arg
	return theta
			
		

def main():
	# temporary main loop
	while True:
		
		# Läs av all lidar-data
		while True:
			try:
				rxdata = lidar_sub.recv_string(zmq.DONTWAIT)
				id, distance, angle = rxdata.split()
				LADARpub.send_string(rxdata)
				lidar_list[i] = [distance, angle]
				if i < 199:
					i += 1
				else:
					i=0
					target = get_target(lista2))
					print(target)
					#spi_pair.send_string(str(target))
					#reply = spi.pair.recv_string()
					#print(reply)
			except zmq.Again: #EAGAIN?
				break
		
		
		############################################
		# Just nu sparas all kamera-data i en list #
		# Vore eventuellt smartare ha en PUSH-PULL #
		# socket som frågar kameraprogrammet ifall #
		#     beslutad körväg är säker eller inte  #
		# istället för att läsa av den hela tiden  #
		############################################
		
		# Läs av all kamera-data, utkommenterad av samma skäl som ovan
		'''
		while True:
			try:
				rxdata = cam_sub.recv_string(zmq.DONTWAIT)
				id, distance, angle = rxdata.split()
				cam_list.append[distance, angle]
			except zmq.Again:
				break
		'''		
		time.sleep(0.001)
		except KeyboardInterrupt:
			break
	LADARpub.close()
	lidar_sub.close()
	spi_pair.close()
if __name__=='__main__':main()
