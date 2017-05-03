from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import random
import thread

context = zmq.Context()

USERrep = context.socket(zmq.REP)
zmq.ssh.tunnel_connection(USERrep,"tcp://localhost:5550","arc@nhkim91.ddns.net:4444",None,"stavarett")
ARCpub = context.socket(zmq.PUB)
zmq.ssh.tunnel_connection(ARCpub,"tcp://localhost:4550","arc@nhkim91.ddns.net:4444",None,"stavarett")

ARCpub.setsockopt(zmq.SNDHWM,1000)
# USERrep.setsockopt_string(zmq.REPLY, encoding='ascii')

def generateFaceLadarThread(threadName,delay):
    print("Started thread %s" % threadName)
    while True:
        try:
            angle=random.randrange(0,360)
            distance=random.randrange(10,3000)
            ARCpub.send_string("l %i %i \n" %(angle, distance))
            sleep(delay)
        except KeyboardInterrupt:
            ARCpub.close()
            context.term()
            break


def main():
    thread.start_new_thread(generateFaceLadarThread, ("GenerateFakeLadarThread",0.01))
    while True:
        try:
            message=USERrep.recv_string()#encoding='ascii')
            print("Recived request: %s" % message)
            USERrep.send_string(b"World")
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    context.term()
if __name__ == '__main__':main()