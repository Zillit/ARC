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
spi_sens.open(0,1) # Shit works yo
spi_sens.mode = 0b00

#context = zmq.Context()
#socket = context.socket(zmq.REP)
#socket.bind("tcp://*:5566")


def sensor_transmit():
	resp = spi_sens.xfer2([0xFF,0,0,0,0],8000,1,8) # Ta emot 4 sensorvärden via spi
	data = str(resp[1])+" "+str(resp[3]) # Gör om till en sträng
	return data # Returnera
	
	
def styr_transmit(data):
	spi_styr.xfer2([data],250000,1,8) # Skicka kommando till styrmodulen via spi


def main():
        while True:
                try:
                        #command = socket.recv_string(zmq.DONTWAIT)
                        
                        #styr_transmit(int(command))
                        reply = sensor_transmit()# Läs sensordata
                        #command.send_string(reply)
                        #print "Hello"
                        print reply
                        #print reply.split(' ',1)
                        time.sleep(0.1)
                except :
                        continue
                #time.sleep(0.1)
        socket.close()
        context.term()


if __name__ == '__main__':main()







