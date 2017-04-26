#import sys 
import time
import zmq

context =  zmq.Context()

#XSUB from ARC
frontend = context.socket(zmq.SUB)
frontend.bind("tcp://*:2223")

#XPUB to PC
backend = context.socket(zmq.PUB)
backend.bind("tcp://*:2224")

#REP from PC
backrep = context.socket(zmq.REP)
backrep.bind("tcp://*:2225")

#REQ to ARC
frontreq = context.socket(zmq.REQ)
frontreq.bind("tcp://*:2226")

#Subscribe on everything
frontend.setsockopt_string(zmq.SUBSCRIBE, '10001'.decode('ascii'))


def main():
        print("MAIN")
        #Forward data to PC and check for a pull
        while True:
                try:
                        message = frontend.recv_string(zmq.DONTWAIT)
                        print message
                        backend.send_string(message)
                        #frontreq.send_string(message)
                        #mess = frontrex.recv_string(zmq.DONTWAIT)
                        #print mess
                except zmq.Again:
                        pass

                try:
                        message = frontend.recv_string(zmq.DONTWAIT)
                        print message
                        backend.send_string(message)
                        #frontreq.send_string(message)
                        #mess = frontrex.recv_string(zmq.DONTWAIT)
                        #print mess
                except zmq.Again:
                        pass
                #try:
                        #command = backrep.recv_string(zmq.DONTWAIT)
                        #backrep.send_string(command)
                        #frontreq.send_string(command)
                        #check = frontreq.recv_string()
                        #if command != check:
                                #ERROR.JPG
                                #SHUT SHIT DOWN
                #except zmq.Again:
                        #pass
                        #command = backrep.recv_string(zmq.DONTWAIT)
                        #backrep.send_string(command)
                        #frontreq.send_string(command)
                        #check = frontreq.recv_string()
                        #if command != check:
                                #ERROR.JPG
                                #SHUT SHIT DOWN
                #except zmq.Again:
                        #pass

if __name__ == '__main__': main()


