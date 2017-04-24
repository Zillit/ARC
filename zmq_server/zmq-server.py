# Server to act as a bridge between PC and ARC
#!/usr/bin/env python3
#import sys 
import time
import zmq

context =  zmq.Context()

#XSUB from ARC
frontend = context.socket(zmq.SUB)
frontend.bind("tcp://*:5665")

#XPUB to PC
backend = context.socket(zmq.PUB)
backend.bind("tcp://*:8100")

#REP from PC
frontrep = context.socket(zmq.REP)
frontend.bind("tcp://*:5666")

#REQ to ARC
backreq = context.socket(zmq.REQ)
backreq.bind("tcp://*:5667")

#Subscribe on everything
frontend.setsockopt(zmq.SUBSCRIBE, b'')

#Forward data to PC and check for a pull
while True:
	try:
		message = frontend.recv_multipart(zmq.DONTWAIT)
		backend.send_multipart(message)
	except zmq.Again:
		None
	try:
		command = frontreq.recv_string(zmq.DONTWAIT)
		frontreq.send_string(command)
		backreq.send_string(command)
		check = backreq.recv_string()
		if command != check:
			#ERROR.JPG
			#SHUT SHIT DOWN
	except zmq.Again:
		None
