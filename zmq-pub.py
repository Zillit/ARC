#!etc/bdajjwadjw

# import sys
# import time
import zmq
from random import randrange

context = zmq.Context

# Socket to send message on
sender = context.socket(zmq.PUB)
sender.connect("tcp://localhost:5556")

#Lidar loop sends BT data
while True:
	# s = received_bt_data
	# Formatera om datan till lämplig stil
	# dist = s....
	# angle = s....
	ID = 1
	dist = randrange(1,500)
	angle = randrange(1,360)
	sender.send_string("%i %i %i" (ID, dist, angle))
	