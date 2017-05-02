from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import random
import thread

context = zmq.Context()
#LIDARsub = context.socket(zmq.SUB)
#LIDARsub.connect("tcp://localhost:5565")
#SPIreq = context.socket(zmq.REQ)
#SPIreq.connect("tcp://localhost:5566")
USERrep = context.socket(zmq.REP)
zmq.ssh.tunnel_connection(USERrep,"tcp://localhost:5550","arc@nhkim91.ddns.net:4444",password = "stavarett")
ARCpub = context.socket(zmq.PUB)
zmq.ssh.tunnel_connection(ARCpub,"tcp://localhost:4550","arc@nhkim91.ddns.net:4444",password = "stavarett")

ARCpub.setsockopt(zmq.SNDHWM,1000)
#LIDARsub.setsockopt_string(zmq.SUBSCRIBE, "10001".decode('ascii'))

def generateFaceLadarThread(threadName,delay):
    print("Started thread %s" % threadName)
    while True:
        try:
            angle=random.randrange(0,360)
            distance=random.randrange(10,3000)
            ARCpub.send_string("%f %i \n" %(angle, distance))
            sleep(delay)
        except KeyboardInterrupt:
            ARCpub.close()
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
            context.term()
            break
    
def main():
    #thread.start_new_thread(sendRealDataThread, ("sendRealDataThread",0.01))
    while True:
        try:
            message=USERrep.recv()
            #SPIreq.send_string(message)
            print("Recived request: %s" % message)
            #reply = SPIreq.recv_string()
            #print("Received reply: %s" % reply)
            USERrep.send(b"World")
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    #SPIreq.close()
    #LIDARsub.close()
    context.term()
    
if __name__ == '__main__':
    main()
