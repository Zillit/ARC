"""
    Bridge Test Server
    Goal:
        Every N seconds, publish the time & interval to localhost:3283(DAVE)
        terminate on ctrl+c
"""

from time import sleep
from time import ctime
import zmq
from zmq import ssh
import paramiko

class Server(object):

    def __init__(self, host = None, port = None, ssh_server = None):

        self.host = host or "127.0.0.1"
        self.port = port or "3283"
        self.conn = "tcp://%s:%s" % (self.host, self.port)
        #Kick off 0MQ build up
        self.ctx = zmq.Context()
        self.s = self.ctx.socket(zmq.PUB)
        self.s.bind(self.conn)
        self.tunnel=ssh.tunnel_connection(self.s, self.conn, ssh_server, password = "stavarett")

    def send(self, msg):
        """
            Ecapsulate send in case I want/need to overload it
        """
        return self.s.send(msg)


    def run(self):
        while True:
            try:
                msg = "%s" % ctime()
                self.send(msg)
                print msg
            except KeyboardInterrupt:
                print "Interupted!"
                break

            sleep(2)




def main():
    #Don't need a reference, so just instantiate and let it block.
    Server(None,None,"arc@nhkim91.ddns.net:2222").run()



if __name__ == '__main__': main()