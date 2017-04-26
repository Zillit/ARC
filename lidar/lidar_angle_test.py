#!/usr/bin/env python3
import bluetooth
import time

bd_addr = "00:06:66:03:A6:A5" #FireFly Bluetooth adress on Lidar tower
port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))
sock.settimeout(5.0)
print("Connection Acquired")
lista =""
lista2 = []
i=0


def get_target(lista):
	r = 0
	theta = 0
	for i in range(len(lista)):
		dist = lista[i][0]
		arg = lista[i][1]
		if (dist > r and 45 < arg < 135):
			r = dist
			theta = arg
	print(r,theta)


while True:
    data = sock.recv(1024).decode("utf-8") # Read Bluetooth buffer for Lidar data
    if data:
        lista += data 
    while lista.find("\n") != -1:
		dist = lista[:lista.find(":")]	
		angle = lista[lista.find(":")+1:lista.find("\n")-1] 
        lista2.append([dist, angle])
        lista = lista[lista.find("\n")+1:] 
    if i < 1000:
		i += 1
	else:
		get_target(lista)
		break
		