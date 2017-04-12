#!/usr/bin/env python3

import stat, os

fifo_name = '/tmp/fifo_from_ladar\0'
MAX_SIZE = 4096
HEADER_SIZE = 6

def get_checksum(packet):
    if packet[4] > (MAX_SIZE - HEADER_SIZE):
        return 0

    checksum = packet[0] + packet[1] + packet[2] + packet[4]
    checksum += packet[5]//65536+packet[5]%65536 

    return checksum



def send_data(packet):
    with open(fifo_name, 'w') as f:
        for i in range(0,6):
            f.write(packet[i])
        #f.write(packet)

    

def main():
    angular = int(123.4125*100)
    data = hex(angular)
    for i in range(0,6-len(hex(angular))):
        data = data[:2] + str(0) + data[2:]
    distant = int(12425.1243)
    data = hex(distant) + data[2:]
    #try:
    #    os.mkfifo(fifo_name)
    #except FileExistsError:
    #    pass
    #with open(fifo_name, 'wb') as f:
        # b for binary mode
    #    f.write('{}\n'.format(len(data)).encode())
    #    f.write(data)
    packet = [int('0x20', 16), int('0x03', 16), int('0x02', 16), 0, len(data)-2, int(data, 16)]
    packet[3] = get_checksum(packet)
    if stat.S_ISFIFO(os.stat(fifo_name).st_mode):
        send_data(packet)
    




if __name__ == '__main__':
    main()
