#!/usr/bin/env python3

import sys
import zmq

context =  zmq.Context()
socket = context.socket(zmq.SUB)

print("Collecting lidar data")
socket.connect("tcp://localhost:5556")
print('1')

lidar_id = sys.argv[1] if len(sys.argv) > 1 else  "10001"

print('2')

if isinstance(lidar_id, bytes):
	lidar_id = lidar_id.decode('ascii')
socket.setsockopt_string(zmq.SUBSCRIBE, lidar_id)

print('3')

for i in range(0,10):
        #print('4')
	stringer = socket.recv_string()
	print('5')
	lidar_id, distance, angle = stringer.split()
	print("Distance: '%s', Angle: '%s'" % (distance, angle))
