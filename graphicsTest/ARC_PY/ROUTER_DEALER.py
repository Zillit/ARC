# import sys
import time
import zmq
import threading
import random
import time

context = zmq.Context()

# ARCrep = context.socket(zmq.DEALER)
# ARCrep.bind("tcp://*:2223")

# ARCreq = context.socket(zmq.ROUTER)
# ARCreq.bind("tcp://*:2226")

USERrep = context.socket(zmq.DEALER)
USERrep.bind("tcp://*:2225")

USERreq = context.socket(zmq.ROUTER)
USERreq.bind("tcp://*:2224")

class Thread_Proxy(threading.Thread):
    def __init__(self, req=None, rep=None):
        threading.Thread.__init__(self)
        self.req=req
        self.rep=rep
    def run(self):
        zmq.proxy(self.req,self.rep)


def main():
    print("MAIN")
    test=Tread_Proxy(USERreq,USERrep)
    test.start()
    # ARC_to_USER=Thread_Proxy(ARCreq,USERrep)
    # USER_to_ARC=Thread_Proxy(USERreq,ARCrep)
    # ARC_to_USER.start()
    # USER_to_ARC.start()
    # # Forward data to PC and check for a pull
    while True:
        try:
            print "random.randomrange(1,10)"
            sleep(1)
        except KeyboardInterrupt:
            break
    # ARCrep.close()
    # ARCreq.close()
    USERrep.close()
    USERreq.close()
    context.term()
if __name__ == '__main__': main()
