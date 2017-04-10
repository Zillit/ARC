/** Protocol
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  2  | len  |
 * 
 * Full duplex SPI-slave
 * By Kim
 */

#include <asf.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/portpins.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "spi_slave.h"

// Macro defined max function
#define max(a,b) \
({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

#define HEADER_SIZE 6
#define MAX_SIZE 4096

volatile uint32_t rxIndex;
volatile uint32_t txIndex;
uint8_t rxBuf[MAX_SIZE];
uint8_t txBuf[MAX_SIZE];
uint16_t transferSize;

Addr myAddr;

void (*packetCb)(uint8_t*, uint16_t);

struct Packet
{
	uint8_t cmd;
	uint8_t from;
	uint8_t to;
	uint8_t checksum;
	uint16_t len;
	uint8_t* data;
};

struct Packet rxPacket;
struct Packet txPacket;

void spiSlaveInit(Addr myAddress, void (*packetCallback)(uint8_t*, uint16_t))
{
	myAddr = myAddress;
	packetCb = packetCallback;
	
	DDRB = (1 << DDB6) | (1 << DDB3); // Set MISO and PORTB3 as output, the other ports are inputs
	SPCR = (1 << SPE); // Enable SPI
}

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
	packet->cmd             = buf[0];
	packet->from            = buf[1];
	packet->to              = buf[2];
	packet->checksum        = buf[3];
	packet->len	            = buf[4] << 8 | buf[5];
	packet->data            = &buf[6];
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

void spiSend(Cmd cmd, Addr to, uint8_t* data, uint16_t length)
{
	txPacket.cmd             = cmd;
	txPacket.from            = myAddr;   				
	txPacket.to              = to;
	txPacket.len	         = length;
	txPacket.data            = data;
	txPacket.checksum        = calcChecksum(&txPacket);

	packetToBuffer(&txPacket, txBuf);

	// Start spi transfer
	SPDR = txBuf[txIndex++];
	PORTB = (1 << PB3);
}

ISR(SPI_STC_vect, ISR_BLOCK)
{	
	PORTB = (0 << PB3);
	rxBuf[rxIndex++] = SPDR; // Read spi data register to rx buffer
	SPDR = txBuf[txIndex++]; // Write tx to spi data register
	
	// Extract packet header from buffer
	if(rxIndex == HEADER_SIZE)
	{
		bufferToPacket(rxBuf, &rxPacket);
		
		if(rxPacket.len > (MAX_SIZE - HEADER_SIZE))
		{
			// TODO: perror("Error, the packet is too big.");
		}
		
		if (txPacket.cmd != NUL && rxPacket.cmd != NUL)
		{
			transferSize = max(rxPacket.len, txPacket.len);
		}
		else if (txPacket.cmd != NUL)
		{
			transferSize = txPacket.len;
		}
		else if (rxPacket.cmd != NUL)
		{
			transferSize = rxPacket.len;
		}
		else
		{
			transferSize = 0;
		}
	}
	
	// The whole packet has been received, figure out what todo with it.
	if (rxIndex >= HEADER_SIZE && rxIndex == (HEADER_SIZE + transferSize))
	{
		if (rxPacket.cmd != NUL && checkPacket(&rxPacket))
		{
			packetCb(rxPacket.data, rxPacket.len);
		}
		
		rxIndex = 0;
		txIndex = 0;
	}
}
