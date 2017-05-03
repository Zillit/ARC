from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
import sys
import thread

context = zmq.Context()

USERreq = context.socket(zmq.ROUTER)
USERrep= context.socket(zmq.DEALER)
zmq.ssh.tunnel_connection(USERrep,"tcp://localhost:5555","arc@nhkim91.ddns.net:4444",None, password="stavarett")
USERreq.bind("tcp://*:2550")
ARCsub = context.socket(zmq.SUB)
zmq.ssh.tunnel_connection(ARCsub,"tcp://localhost:4555","arc@nhkim91.ddns.net:4444",None,password="stavarett")
ARCpub = context.socket(zmq.PUB)
ARCpub.bind("tcp://*:2555")

filter=b''
if isinstance(filter,bytes):
    filter=filter.decode('ascii')
ARCsub.setsockopt_string(zmq.SUBSCRIBE,filter)
ARCsub.setsockopt(zmq.RCVHWM,1000)
#Defines the different proxy services
def rep_req_proxy_thread(threadName,delay):
    print("Started thread %s" % threadName)
    sleep(delay)
    zmq.proxy(USERreq,USERrep)
    USERreq.close()
    USERrep.close()

def main():
    thread.start_new_thread(rep_req_proxy_thread,("rep_req_proxy",1))
    while True:
        msg=ARCsub.recv_string()
        ARCpub.send_string(msg)
        try:
            pass
            # angle,distance=msg.split()
            # print("Ladarpoint had angle: %s and distance: %s cm" % (angle,distance))
        except KeyboardInterrupt:
            break

    ARCsub.close()
    ARCpub.close()
    USERreq.close()
    USERrep.close()
    context.term()

if __name__ == '__main__':main()