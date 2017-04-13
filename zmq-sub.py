#!/usr/bin/env python3

#import sys
import zmq

context =  zmx.Context()
socket = context.socket(zmq.SUB)

print("Collecting lidar data")
socket.connect("tcp://localhost:5556")

lidar_id = 1

if ininstance(lidar_id, bytes):
	lidar_id = lidar_id.decode('ascii')
socket.setsockopt_string(zmq.SUBSCRIBE, lidar_id)

for update_nbr in range(100):
	string = socket.recv_string()
	lidar_id, distance, angle = string.split()
	print("Distance: '%s', Angle: '%s'" % (distance, angle))
