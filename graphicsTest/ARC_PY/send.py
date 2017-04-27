from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import sys

context = zmq.Context()

USERreq = context.socket(zmq.REQ)
USERreq.connect("tcp://localhost:5555")
ARCsub = context.socket(zmq.SUB)
ARCsub.connect("tcp://localhost:4555")

filter=b''
if isinstance(filter,bytes):
    filter=filet.decode('ascii')
ARCsub.setsockopt_string(zmq.SUBSCRIBE,filter)
ARCsub.setsockopt(zmq.RCVHWM,1000)

def main():
    for request in range(10):
        print("Sending request: %s" % request)
        USERreq.send(b"World")

        message=USERreq.recv()
        print("Received reply %s [%s]" % (request, message))
    while True:
        try:
            string=ARCsub.recv_string()
            angle,distance=string.split()
            print("Ladarpoint had angle: %f and distance: %i cm" % (angle,distance))
        except KeyboardInterrupt:
            break

    USERreq.close()
    ARCsub.close()
    context.term()

if __name__ == '__main__':main()