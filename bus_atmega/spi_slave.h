/** Protocol
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  2  | len  |
 * 
 * Full duplex SPI-slave
 * By Kim
 */

//sei(), set global interrupt, put this in the main program 
// clock frequency?

#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

#include <asf.h>

typedef enum
{
	NUL = 0x00,         // Nothing to say
	CHECK_FAIL = 0x01,  // Check sum
	SEND_DATA = 0x20,
} Cmd;

// Things that are addressable over the bus
typedef enum
{
	CONTROL = 0x00,
	SENSOR = 0x01,
	DYNMAP = 0x02,
	LADAR = 0x03,
	DBUG = 0x41, // 'A' in ascii. TODO: remove this?
} Addr;

void spiSlaveInit(Addr myAddress, void (*packetCallback)(uint8_t*, uint16_t)); 

void spiSend(Cmd cmd, Addr to, uint8_t* data, uint16_t length); 

#endif /* SPI_SLAVE_H */
