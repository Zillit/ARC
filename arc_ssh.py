from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import random
import thread
import spidev

spi_styr = spidev.SpiDev()
spi_styr.open(0,0)
spi_styr.mode = 0b00

context = zmq.Context()
LIDARsub = context.socket(zmq.SUB)
LIDARsub.connect("tcp://localhost:5565")
#SPIreq = context.socket(zmq.REQ)
#SPIreq.connect("tcp://localhost:5566")
#CAMrep = context.socket(zmq.REP)
#CAMrep.connect("tcp://localhost:5567")
USERrep = context.socket(zmq.REP)
zmq.ssh.tunnel_connection(USERrep,"tcp://localhost:5550","arc@nhkim91.ddns.net:4444",password = "stavarett")
ARCpub = context.socket(zmq.PUB)
zmq.ssh.tunnel_connection(ARCpub,"tcp://localhost:4550","arc@nhkim91.ddns.net:4444",password = "stavarett")

ARCpub.setsockopt(zmq.SNDHWM,1000)
LIDARsub.setsockopt_string(zmq.SUBSCRIBE, "10001".decode('ascii'))

def styr_transmit(data):
	spi_styr.xfer2([data],250000,1,8)

def generateFaceLadarThread(threadName,delay):
    print("Started thread %s" % threadName)
    while True:
        try:
            angle=random.randrange(0,60)
            distance=random.randrange(10,3000)
            ARCpub.send_string("l %i %i \n" %(angle, distance))
            sleep(delay)
        except KeyboardInterrupt:
            ARCpub.close()
            USERrep.close()
            context.term()
            break

def sendRealDataThread(threadName,delay):
    while True:
        try:
            mess = LIDARsub.recv_string()
            id, distance, angle = mess.split()
            print mess             
            ARCpub.send_string("%i %i \n" %(int(angle), int(distance)))
            sleep(delay)
        except KeyboardInterrupt:
            ARCpub.close()
            USERrep.close()
            context.term()
            break

def sendCommandToSPI(thredName,delay,message):
    while True:
        try:
            speed, theta =message[7:].split()
            speed= int(speed)+128
            theta= int(theta)+53
            print speed
            print theta
            styr_transmit(speed)
            styr_transmit(theta)
            print("Recived request: %s" % message)
            sleep(delay)
            break
        except KeyboardInterrupt:
            break
    return
    
    
def main():
    thread.start_new_thread(sendRealDataThread, ("sendRealDataThread",0.01))
    #thread.start_new_thread(generateFaceLadarThread, ("Fake ladar points", 0.01))
    while True:
        '''
        try:
            stopsign=CAMrep.recv(zmq.DONTWAIT)
            if stopsign:
                thread.start_new_thread(sendCommandToSPI, ("%s thread" % stopsign,0.01,stopsign))
                CAMrep.send(" %s \n" % message)
        except:
            pass
        '''
        try:
            message=USERrep.recv()
            if message[:6] == "STYROR":
                thread.start_new_thread(sendCommandToSPI, ("%s thread" % message,0.01,message))
            USERrep.send(" %s  \n" % message)
            #SPIreq.send_string(message)
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    #SPIreq.close()
    #LIDARsub.close()
    context.term()
    
if __name__ == '__main__': main()
