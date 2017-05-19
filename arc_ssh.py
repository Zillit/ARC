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

spi_sens = spidev.SpiDev()
spi_sens.open(0,1) # Shit works yo
spi_sens.mode = 0b00

context = zmq.Context()
LIDARpub = context.socket(zmq.PUB)
LIDARpub.bind("tcp://*:5569")
CAMERApub = context.socket(zmq.PUB)
CAMERApub.bind("tcp://*:2500")
LIDARsub = context.socket(zmq.SUB)
LIDARsub.connect("tcp://localhost:2555")#5565")
CAMERAsub = context.socket(zmq.SUB)
CAMERAsub.connect("tcp://localhost:2505")
#SPIreq = context.socket(zmq.REQ)
#SPIreq.connect("tcp://localhost:5566")
#CAMERApub = context.socket(zmq.REP)
#CAMERApub.connect("tcp://localhost:5567")
USERrep = context.socket(zmq.REP)
zmq.ssh.tunnel_connection(USERrep,"tcp://localhost:5550","pi@nhkim91.ddns.net:4444",password = "NewArc11")
ARCpub = context.socket(zmq.PUB)
zmq.ssh.tunnel_connection(ARCpub,"tcp://localhost:4550","pi@nhkim91.ddns.net:4444",password = "NewArc11")

ARCpub.setsockopt(zmq.SNDHWM,100)
LIDARsub.setsockopt(zmq.SUBSCRIBE, b"")
CAMERAsub.setsockopt(zmq.SUBSCRIBE, b"")

def styr_transmit(data):
	spi_styr.xfer2([data],250000,1,8)

def sendCamDataThread(threadName,delay):
    print("Enter %s " %threadName)
    while True:
        try:
            messageCam=CAMERAsub.recv_string()
            #if messageCam[:6] == "ARCCAM":
            ARCpub.send_string(messageCam)
            #messa, laps, num = messageCam.split()
            #if (laps >= 3):
            #        styr_transmit(158)
            #        LIDARpub.send_string("False")
            #CAMERApub.send(" %s \n" % message)
            print(messageCam)
            sleep(delay)
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    CAMERAsub.close()
    #CAMERApub.close()
    #SPIreq.close()
    LIDARsub.close()
    LIDARpub.close()
    context.term()
            

def sendRealDataThread(threadName,delay):
    sleep(delay)
    message_sent=0
    while True:
        try:
            mess = LIDARsub.recv_string()
            id, angle,distance= mess.split()
            message_sent=message_sent+1
            ARCpub.send_string("%s %i %i \n" %(str(id), int(angle), int(distance)))
            if (message_sent > 99):
                print mess
                message_sent=0
            try:
                messageCam=CAMERAsub.recv_string(zmq.DONTWAIT)
                ARCpub.send_string("%s 0" %(messageCam))
                print messageCam
            except:
                pass
        except KeyboardInterrupt:
            ARCpub.close()
            USERrep.close()
            CAMERApub.close()
            CAMERAsub.close()
            LIDARsub.close()
            LIDARpub.close()
            context.term()
            break
        

def sendCommandToSPI(thredName,delay,message):
    while True:
        try:
            speed, theta =message[7:].split()
            speed= int(speed)+128+30
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
    print("Hello")
    thread.start_new_thread(sendRealDataThread, ("sendRealDataThread",0.01))
    thread.start_new_thread(sendCamDataThread, ("sendCamDataThred",0.01))
    while True:        
        try:
            message=USERrep.recv()
            print message
            if message[:6] == "MODEAU":
                    LIDARpub.send_string("True")
            elif message[:6] == "MODEMA":
                    LIDARpub.send_string("False")
            elif message[:6] == "STYROR":
                thread.start_new_thread(sendCommandToSPI, ("%s thread" % message,0.01,message))
            elif message[:6] == "ARCCAM":
                 CAMERApub.send(" %s \n" % message)
            USERrep.send(" %s  \n" % message)
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    CAMERAsub.close()
    CAMERApub.close()
    #SPIreq.close()
    LIDARpub.close()
    LIDARsub.close()
    context.term()
    
if __name__ == '__main__': main()
