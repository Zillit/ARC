#
#   Request-reply service in Python
#   Connects REP socket to tcp://localhost:5560
#   Expects "Hello" from client, replies with "World"
#
# import zmq
#
# context = zmq.Context()
# socket = context.socket(zmq.REP)
# socket.connect("tcp://localhost:5560")
#

from time import sleep
from time import ctime
import zmq
from zmq import ssh
#import paramiko

class Server(object):

    def __init__(self, host = None, port = None, ssh_server = None):

        self.host = host or "127.0.0.1"
        self.port = port or "3284"
        self.conn = "tcp://%s:%s" % (self.host, self.port)
        #Kick off 0MQ build up
        self.ctx = zmq.Context()
        self.s = self.ctx.socket(zmq.REP)
        self.tunnel=ssh.tunnel_connection(self.s, self.conn, ssh_server, password = "stavarett")
        self.s.connect(self.conn)

    def send(self, msg):
        """
            Ecapsulate send in case I want/need to overload it
        """
        return self.s.send(msg)


    def run(self):
        while True:
            message = socket.recv()
            print("Received request: %s" % message)
            socket.send(b"World")
            sleep(2)




def main():
    Server(None,None,"arc@nhkim91.ddns.net:2223").run()



if __name__ == '__main__': main()
