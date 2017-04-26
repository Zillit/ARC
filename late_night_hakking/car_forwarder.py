# Server to act as a bridge between PC and ARC
#!/usr/bin/env python3
#import sys 
import time
import zmq

context =  zmq.Context()

#PUB to PC
frontend = context.socket(zmq.PUB)
frontend.bind("tcp://*:5665")

#SUB from ARC
#Implement autonomous part later
#arc_sub = context.socket(zmq.SUB)
#arc_sub.bind("tcp://*:8100")

#REP to PC
frontrep = context.socket(zmq.REP)
frontrep.bind("tcp://*:5666")

#REP to ARC
#Implement autonomous part later
#arc_rep = context.socket(zmq.REQ)
#arc_rep.bind("tcp://*:5667")

#REQ to SPI
spi_req = context.socket(zmq.REQ)
spi_req.bind("tcp://localhost:5558")


#Subscribe on everything
#arc_sub.setsockopt_string(zmq.SUBSCRIBE, '10001'.decode('ascii'))


def main():
	while True:
		try:
			command = frontrep.recv_string(zmq.DONTWAIT)
			if command[:8] == 't_STYROR':
				spi_req.send_string(command[8:])
				data =  spi_req.recv_string(zmq.DONTWAIT)
				print(data)
				frontrep.send_string(command)
				#wasd
			elif command[:8] == 't_ARCCAR':
				#wasd
			else:
				frontrep.send_string("Bad command mate")
		except zmq.Again:
			pass
		
		
if __name__ == '__main__': main()