/** Bus Client example
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  2  | len  |
 * 
 * Full duplex SPI-master
 * By Kim
 */

#include <errno.h>
#include <fcntl.h> // File control, used by SPI and FIFO
#include <linux/stat.h> // used by FIFO
#include <linux/types.h> // used by FIFO
#include <netinet/in.h> // ntohl, htonl, convert between host specific and network endians.
#include <stdio.h> // Standard input/output
#include <stdlib.h> // Standard library
#include <string.h>
#include <unistd.h> // POSIX API

#include "bus_client.h"

#define HEADER_SIZE 6
#define MAX_SIZE 4096
#define FIFO_COUNT 2

#define FROM 0
#define TO 1

char fifo[FIFO_COUNT][255];

uint8_t fifoBuf[MAX_SIZE];
int fds[FIFO_COUNT];
bool fdsOpened[FIFO_COUNT];

struct Packet
{
    uint8_t cmd;
    uint8_t from;
    uint8_t to;
    uint8_t checksum;
    uint16_t len;
    uint8_t* data;
} rxPacket, txPacket;

Addr myAddr;
void (*dataCb)(uint8_t*, uint16_t);

uint8_t calcChecksum(struct Packet* packet)
{
	if (packet->len > (MAX_SIZE - HEADER_SIZE))
	{
		return 0;
	}
	
	uint8_t checksum = packet->cmd + packet->from + packet->to + packet->len;
	
	int i;
	
	for(i = 0; i < packet->len; ++i)
	{
		checksum = checksum + packet->data[i];
	}
	
	return checksum;
}

bool checkPacket(struct Packet* packet)
{
    if(packet->len > MAX_SIZE)
    {
        return false;
    }
    
    if(packet->checksum != calcChecksum(packet))
    {
        return false;
    }

    return true;
}

void bufferToPacket(uint8_t* buf, struct Packet* packet)
{
    packet->cmd                = buf[0];
    packet->from               = buf[1];
    packet->to                 = buf[2];
    packet->checksum           = buf[3];
    int netLen                 = buf[4] << 8 | buf[5];
    packet->len                = ntohs(netLen);
    packet->data               = &buf[6];
}

void packetToBuffer(struct Packet* packet, uint8_t* buf)
{
	memset(buf, 0, MAX_SIZE);
	
	buf[0] = packet->cmd;
	buf[1] = packet->from;
	buf[2] = packet->to;
	buf[3] = packet->checksum;
	buf[4] = packet->len >> 8;
	buf[5] = packet->len & 0xFF;
	
	memcpy(&buf[6], packet->data, packet->len);
}

void fifoInit(Addr myAddress, void(*dataCallback)(uint8_t*, uint16_t), char* fifoIn, char* fifoOut)
{
    myAddr = myAddress;
    dataCb = dataCallback;

    strcpy(fifo[TO], fifoIn);
    strcpy(fifo[FROM], fifoOut);

    memset(&rxPacket, 0, size_of(struct Packet));
    memset(&txPacket, 0, size_of(struct Packet));
    memset(&fifoBuf, 0, MAX_SIZE);

    int i;

    for(i = 0; i < FIFO_COUNT; ++i)
    {
        fdsOpened[i] = true;
    }

    fds[FROM] = open(fifo[FROM], O_WRONLY | O_NONBLOCK);
    if(fds[FROM] < 0)
    {
        if(errno == ENXIO)
        {
            fdsOpened[FROM] = false;
        }
        else
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't open FIFO");
            exit(1);
        }
    }

    fds[TO] = open(fifo[TO], O_RDONLY | O_NONBLOCK);
    if(fds[TO] < 0)
    {
        if(errno == ENXIO)
        {
            fdsOpened[TO] = false;
        }
        else
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't open FIFO");
            exit(1);
        }
    }
}

bool verifyFifo(int fd)
{
    if(fd >= FIFO_COUNT)
    {
        return false;
    }

    if(fdsOpened[fd])
    {
        return true;
    }

    if(TO == fd)
        fds[fd] = open(fifo[fd], O_RDONLY | O_NONBLOCK);
    else
        fds[fd] = open(fifo[fd], O_WRONLY | O_NONBLOCK);

    if(fds[fd] >= 0)
    {
        fdsOpened[fd] = true;
        return true;
    }

    return false;
}

void sendBuffer(uint8_t* buf)
{
    bufferToPacket(buf, &packet);

    if(!checkPacket(&packet))
    {
        buf[HEADER_SIZE] = '\0';
        printBuf(buf);
        perror("Error, faulty packet.\n");
        exit(1);
    }

    int totalSize = HEADER_SIZE + packet.len;
    buf[totalSize] = '\0';

    printBuf(buf); // TODO: remove
    
    if(verifyFifo(FROM))
        write(fds[FROM], buf, totalSize);
}

void sendPacket(struct Packet* packet)
{
    packetToBuffer(&txPacket, fifoBuf);

    if(!checkPacket(&packet))
    {
        buf[HEADER_SIZE] = '\0';
        printBuf(buf);
        perror("Error, faulty packet.\n");
        exit(1);
    }

    int totalSize = HEADER_SIZE + packet.len;
    buf[totalSize] = '\0';

    printBuf(buf); // TODO: remove
    
    if(verifyFifo(FROM))
        write(fds[FROM], buf, totalSize);
}

void sendFifo(Cmd cmd, Addr to, uint8_t* data, uint16_t length)
{
	txPacket.cmd             = cmd;
	txPacket.from            = myAddr;   				
	txPacket.to              = to;
	txPacket.len	         = length;
	txPacket.data            = data;
	txPacket.checksum        = calcChecksum(&txPacket);

    // Start fifo transfer
    sendPacket(&txPacket);
}

void busWorker()
{
    int value;

    for(;;)
    {
        value = read(fds[TO], fifoBuf, MAX_SIZE); 
        fifoBuf[value] = '\0';

        if(value < 0 && errno != EAGAIN) // Something is wrong and unhandled!
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't read FIFO");
            exit(1);
        }
        else if (value > 0) // We got a packet from the bus.
        {
            // Send received data to other part of the program.
            bufferToPacket(fifoBuf, &rxPacket);

            if(checkPacket(&rxPacket))
            {
                dataCb(rxPacket.data, rxPacket.len);
            }
        }
        pthread_yield();
    }
}

