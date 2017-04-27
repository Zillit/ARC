
from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko
"""
    Bridge test client:
       So this one's going to be a doozy.
       Connect to target via SSH and then connect to target:localhost:8283(DAVE) and listen
        for the time messages.
"""

import zmq
from zmq import ssh
import paramiko #Ensure paramiko is installed ( and therefore pyCrypto as well )


class Client(object):

    def __init__(self, host = None, port = None, ssh_server = None):

        self.host = host or "127.0.0.1"
        self.port = port or "3283"
        self.conn = "tcp://%s:%s" % (self.host, self.port)
        #Kick off 0MQ build up
        self.ctx = zmq.Context()
        self.s = self.ctx.socket(zmq.REQ)
        #self.s.setsockopt(zmq.SUBSCRIBE,'') #For now, subscribe to everything
        #To lazy to setup a new ssh pub/priv key so setting a throwaway password
        self.tunnel = ssh.tunnel_connection(self.s, self.conn, ssh_server, password = "password")

    def receive(self):
        return self.s.recv()

    def run(self):

        while True:
            try:
                msg = self.receive()
                print msg
            except KeyboardInterrupt:
                print "Interupt"
                break


def main():
    #Don't need a reference, so just instantiate and let it block.
    Client(None,None,"arc@nhkim91.ddns.net:2224").run()



if __name__ == '__main__': main()