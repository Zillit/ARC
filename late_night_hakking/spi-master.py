################################################################################
# OBS: Antagligen måste denna köras innan main-loop körs igång för zmq är knas #
################################################################################
#!/usr/bin/env python3
#import sys 
import time
import spidev
import zmq

spi_styr = spidev.SpiDev()
spi_styr.open(0,0)
spi_styr.mode = 0b00

spi_sens = spidev.SpiDev()
spi_sens.open(0,1) # Har inte testat att CS1 funkar men det borde den göra
spi_sens.mode = 0b00

################################################
#    Om nu sensordata bara hämtas samtidigt    #
#    som styrbeslut skickas istället för att   #
#  kolla av den på annat initiativ än begäran  #
#    av huvudprogrammet räcker en PUSH-PULL    #
#       istället för att krångla med PAIR      #
################################################
context = zmq.Context()
socket = context.socket(zmq.REP)
socket.connect("tcp://localhost:5558")

def sensor_transmit():
	resp = spi_sens.xfer2([0xFF,0,0,0,0],250000,1,8) # Ta emot 4 sensorvärden via spi
	data = str(resp[1])+str(resp[2])+str(resp[3])+str(resp[4]) # Gör om till en sträng
	return data # Returnera
	
	
def styr_transmit(data):
	spi_styr.xfer2([data],250000,1,8) # Skicka kommando till styrmodulen via spi

while True:
	command = socket.recv_string() # Ta emot kommando via zmq
	styr_transmit(int(command)) # Skicka till styr
	#reply = sensor_transmit() # Läs sensordata 
	reply = "hakka datta"
	socket.send_string(reply) # Skicka vidare via zmq
	







