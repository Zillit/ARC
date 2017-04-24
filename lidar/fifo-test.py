#!/usr/bin/env python3

import stat, os

fifo_name = '/tmp/fifo_from_ladar'
MAX_SIZE = 4096
HEADER_SIZE = 6



def get_checksum(packet):
    if int(packet[4], 16) > (MAX_SIZE - HEADER_SIZE):
        return 0

    checksum = int(packet[0], 16) + int(packet[1], 16) + int(packet[2], 16) + int(packet[4], 16)
    checksum += int(packet[5], 16)//65536 + int(packet[5], 16)%65536 

    return hex(checksum)



def send_data(packet):
    buf = ''
    for i in range(6):
        buf += packet[i]
    with open(fifo_name, 'w') as f:
        f.write(buf)
        #f.write(packet)

    

def main():
    angular = int(123.4125*100)
    data = hex(angular)
    for i in range(6-len(hex(angular))):
        data = data[:2] + str(0) + data[2:]

    distant = int(12425.1243)
    data = hex(distant) + data[2:]
    
    packet = ['0x20', '0x03', '0x02', '', hex(len(data)-2), data]
    packet[3] = get_checksum(packet)
    if stat.S_ISFIFO(os.stat(fifo_name).st_mode):
        print('sending')
        send_data(packet)
        print('sent')
    else:
        print('error')
    




if __name__ == '__main__':
    main()
