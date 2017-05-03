import time
import zmq
import thread
import random
import time

context = zmq.Context()
#Setup the communication from user to car
USERreq=context.socket(zmq.ROUTER)
USERreq.bind("tcp://*:5555")
USERrep=context.socket(zmq.DEALER)
USERrep.bind("tcp://*:5550")
#Setup the communication from car to user
ARCsub=context.socket(zmq.SUB)
ARCsub.bind("tcp://*:4550")
ARCpub=context.socket(zmq.PUB)
ARCpub.bind("tcp://*:4555")

#Subscribe to everything so that all is passed on
ARCsub.setsockopt(zmq.SUBSCRIBE, b'')
#Set the maximum number of message in buffer to awaid overloead. Discard additional ones
ARCsub.setsockopt(zmq.RCVHWM,1000)
ARCpub.setsockopt(zmq.SNDHWM,1000)

#Defines the different proxy services
def rep_req_proxy_thread(threadName,delay):
    print("Started thread %s" % threadName)
    time.sleep(delay)
    zmq.proxy(USERreq,USERrep)
    USERreq.close()
    USERrep.close()

def pub_sub_proxy_thread():
    print("test")

def main():
    thread.start_new_thread(rep_req_proxy_thread,("rep_req_proxy",1))
    while True:
        message=ARCsub.recv_string()
        ARCpub.send_string(message)
        try:
            pass
        except KeyboardInterrupt:
            break

    ARCpub.close()
    ARCsub.close()
    USERreq.close()
    USERrep.close()
    context.term()

if __name__ == '__main__':main()