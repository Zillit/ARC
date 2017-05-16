from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
from random import randrange
import thread
import math

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:2555")
socket.setsockopt(zmq.SNDHWM,1000)
#sockRep = context.socket(zmq.REP)
#sockRep.bind("tcp://*:2550")
# zmq.ssh.tunnel_connection(sockRep,"tcp://localhost:5550","arc@nhkim91.ddns.net:4444",password = "stavarett")

def ladarThread(threadName,delay,socket):
    print("Started thread %s" % threadName)
    sleep(delay)
    while(True):
        try:
    # x=-64
    # z=-64
    # for x in range(-64,64):
    #     for z in range(0,128):
    #         angle=math.degrees(math.atan2(z,x))
    #         distance=math.hypot(x,z)
    #         socket.send_string("%i %i" % (angle, distance))
    #     sleep(0.1)
            x1=30
            for z1 in range(0,100):
                angle=math.degrees(math.atan2(z1,x1))
                distance1=math.hypot(x1,z1)
                socket.send_string("LADAR %i %i /n" % (angle, distance1))
            x2=-20
            for z2 in range(0,60):
                angle2=math.degrees(math.atan2(z2,x2))
                distance2=math.hypot(x2,z2)
                socket.send_string("LADAR %i %i /n" % (angle2, distance2))
                # angle = randrange(30, 90)
                # distance = randrange(20, 300)
                # socket.send_string("%i %i" % (angle, distance))
            z3=90
            for x3 in range(-60,30):
                angle3=math.degrees(math.atan2(z3,x3))
                distance3=math.hypot(x3,z3)
                socket.send_string("LADAR %i %i /n" % (angle3, distance3))
            for it in range(0,360):
                angle4=it
                distance4=50+it*3
                socket.send_string("LADAR %i %i /n" % (angle4, distance4))
            sleep(1)
        except KeyboardInterrupt:
            break
    # socket.close()        
    # sockRep.close()
    # context.term()
    
    # while True:
    #     try:
    #         angle = randrange(30, 90)
    #         distance = randrange(20, 300)
    #         sleeptime = randrange(0, 100)/100
    #         socket.send_string("%i %i" % (angle, distance))
    #         sleep(delay)
    #     except KeyboardInterrupt:
    #         break
    # socket.close()
    # sockRep.close()
    # context.term()

def main():
    thread.start_new_thread(ladarThread,("FakeLadarPointsThread",5,socket))
    while True:
        try:
            True
            #message=sockRep.recv_multipart()
            #print("Recived request: %s" % message)
            #sockRep.send_string(b"World")
        except KeyboardInterrupt:
            break
    socket.close()
    sockRep.close()
    context.term()

if __name__ == '__main__':main()
