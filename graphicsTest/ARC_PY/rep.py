from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import random
import thread

context = zmq.Context()

USERrep = context.socket(zmq.REP)
USERrep.connect("tcp://localhost:5550")
ARCpub = context.socket(zmq.PUB)
ARCpub.connect("tcp://localhost:4550")

ARCpub.setsockopt(zmq.SNDHWM,1000)

def generateFaceLadarThread(threadName,delay):
    print("Started thread %s" % threadName)
    while True:
        try:
            angle=random.randrange(0.0,3.14)
            distance=random.randrange(10,3000)
            ARCpub.send_string("%f %i \n" angle, distance)
            time.sleep(delay)
        except KeyboardInterrupt:
            break

        ARCpub.close()
        context.term()

def main():
    thread.start_new_thread("GenerateFakeLadarThread",0.01)
    while True:
        try:
            message=USERrep.recv()
            print("Recived request: %s" % message)
            USERrep.send(b"World")
        except KeyboardInterrupt:
            break
    ARCpub.close()
    USERrep.close()
    context.term()
if __name__ == '__main__':main()