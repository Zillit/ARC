#########################################################################
#									#					#
# 		   Receives Lidar data via bluetooth			#
#		and then transmits said data to the main		#
#		 loop via ZMQ publishing in the form of			#
#		(ID,distance,angle) with ID = 10001 and 		#
#				  socket port 5556			#			#
#									#					#
#########################################################################
#{Not tested but it should work, still need to implement threading though}

#!/usr/bin/env python3
import bluetooth
import time
import zmq

bd_addr = "00:06:66:03:A6:A5" #FireFly Bluetooth adress on Lidar tower
port = 1

context = zmq.Context()

# Socket to send message on
sender = context.socket(zmq.PUB)
sender.bind("tcp://*:5565")
ID = int(10001) # ID for the ZMQ publisher

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
sock.settimeout(5.0)
print("Connection Acquired")
lista =""
i=0

sender.send_string("%i %i %i" %(ID, 1337, 1337)) # First discarded transmission	
	
while True:
    data = sock.recv(1024).decode("utf-8") # Read Bluetooth buffer for Lidar data
    if data:
        lista += data # Append data to a string
        while lista.find("\n") != -1: # While there still is raw Lidar data to send in the string
            try:
                dist = int(lista[:lista.find(":")])	# Get distance from earliest Lidar reading not processed
                angle = int(lista[lista.find(":")+1:lista.find("\n")-1]) # Get angle from above mentioned Lidar reading
            except ValueError:
                lista = lista[lista.find("\n")+1:]
                continue
            sender.send_string("%i %i %i" % (ID, dist, angle)) # Send ID, distance and angle via ZMQ
            print("%i %i %i" % (ID, dist, angle)) # Temporary, remove later
            lista = lista[lista.find("\n")+1:] # Remove transmitted data from string


sock.close()
