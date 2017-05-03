from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
from random import randrange
import thread

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:2555")
socket.setsockopt(zmq.SNDHWM,1000)
sockRep = context.socket(zmq.REP)
zmq.ssh.tunnel_connection(sockRep,"tcp://localhost:5550","arc@nhkim91.ddns.net:4444",password = "stavarett")

def ladarThread(threadName,delay):
    print("Started thread %s" % threadName)
    while True:
        try:
            angle = randrange(30, 90)
            distance = randrange(20, 300)
            sleeptime = randrange(0, 100)/100
            socket.send_string("%i %i" % (angle, distance))
            sleep(delay)
        except KeyboardInterrupt:
            break
    socket.close()
    sockRep.close()
    context.term()

def main():
    # thread.start_new_thread(ladarThread,("FakeLadarPointsThread",0.01))
    while True:
        try:
            message=sockRep.recv_multipart()
            print("Recived request: %s" % message)
            sockRep.send_string(b"World")
        except KeyboardInterrupt:
            break
    socket.close()
    sockRep.close()
    context.term()

if __name__ == '__main__':main()
