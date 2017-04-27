from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import sys

context = zmq.Context()

USERreq = context.socket(zmq.REQ)
zmq.ssh.tunnel_connection(USERreq,"tcp://localhost:5555","arc@nhkim91.ddns.net:4444")
ARCsub = context.socket(zmq.SUB)
zmq.ssh.tunnel_connection(ARCsub,"tcp://localhost:4555","arc@nhkim91.ddns.net:4444")

filter=b''
if isinstance(filter,bytes):
    filter=filter.decode('ascii')
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
            print("Ladarpoint had angle: %s and distance: %s cm" % (angle,distance))
        except KeyboardInterrupt:
            break

    USERreq.close()
    ARCsub.close()
    context.term()

if __name__ == '__main__':main()