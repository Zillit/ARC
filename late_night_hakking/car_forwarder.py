# Server to act as a bridge between PC and ARC
#!/usr/bin/env python3
#import sys 
import time
import zmq

context =  zmq.Context()

#PUB to PC
frontend = context.socket(zmq.PUB)
frontend.connect("tcp://nhkim91.ddns.net:2223")

#SUB from ARC
#Implement autonomous part later
#arc_sub = context.socket(zmq.SUB)
#arc_sub.bind("tcp://*:8100")

#REP to PC
frontrep = context.socket(zmq.REP)
frontrep.connect("tcp://nhkim91.ddns.net:2226")

#REP to ARC
#Implement autonomous part later
#arc_rep = context.socket(zmq.REQ)
#arc_rep.bind("tcp://*:5667")

#REQ to SPI
spi_req = context.socket(zmq.REQ)
spi_req.bind("tcp://*:5558")


#Subscribe on everything
#arc_sub.setsockopt_string(zmq.SUBSCRIBE, '10001'.decode('ascii'))


def main():
<<<<<<< HEAD
        while True:
                try:
                        command = frontrep.recv_string(zmq.DONTWAIT)
                        if command[:8] == 't_STYROR':
                                spi_req.send_string(command[8:])
                                data =  spi_req.recv_string()
                                print(data)
                                frontrep.send_string(command)
                                #wasd
                        elif command[:8] == 't_ARCCAR':
                                #wasd
                                print('Error')
                        else:
                                frontrep.send_string("Bad command mate")
                except zmq.Again:
                        pass


=======
	print("MAIN")
	while True:
		try:
			frontend.send_string("test string")
			command = frontrep.recv_string(zmq.DONTWAIT)
			if command[:8] == 't_STYROR':
				spi_req.send_string(command[8:])
				data =  spi_req.recv_string()
				print(data)
				frontrep.send_string(command)
				#wasd
			elif command[:8] == 't_ARCCAR':
				#wasd
				print('Error')
			else:
				frontrep.send_string("Bad command mate")
		except zmq.Again:
			pass
		
		
>>>>>>> 9e9936c6e4a34bf4a390f95626c2db6b18af49be
if __name__ == '__main__': main()
