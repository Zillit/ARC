/** Bus Client example
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  2  | len  |
 * 
 * Full duplex SPI-master
 * By Kim
 */

#ifndef BUS_CLIENT_H
#define BUS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h> // File control, used by SPI and FIFO
#include <linux/stat.h> // used by FIFO
#include <linux/types.h> // used by FIFO
#include <netinet/in.h> // ntohl, htonl, convert between host specific and network endians.
#include <stdio.h> // Standard input/output
#include <stdlib.h> // Standard library
#include <string.h>
#include <unistd.h> // POSIX API

// Things that are addressable over the bus
typedef enum
{
    CONTROL = 0x00,
    SENSOR = 0x01,
    DYNMAP = 0x02,
    LADAR = 0x03,
    DBUG = 0x41, // 'A' in ascii. TODO: remove this?
} Addr;

// Commands that can be sent over the bus
typedef enum
{
    NUL = 0x00,         // Nothing to say
    CHECK_FAIL = 0x01,  // Check sum
    SEND_DATA = 0x20,
} Cmd;

/** Example
 * How to use this:
void handleData(uint8_t* data)
{
    // Do something with the data.
}

fifoInit(DYNMAP, &handleData, "/tmp/fifo_from_dynmap", "/tmp/fifo_to_dynmap");
//*/

void fifoInit(Addr myAddress, void(*dataCallback)(uint8_t*, uint16_t), char* fifoIn, char* fifoOut);
void sendFifo(Cmd cmd, Addr to, uint8_t* data, uint16_t length);
void busWorker();

#ifdef __cplusplus
}
#endif

#endif /* BUS_CLIENT_H */
