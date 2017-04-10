/** Protocol
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  2  | len  |
 * 
 * Full duplex SPI-master
 * By Kim
 */

 // TODO: Check main-loop time

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <fcntl.h> // File control, used by SPI and FIFO
#include <linux/spi/spidev.h> // SPI
#include <linux/stat.h> // used by FIFO
#include <linux/types.h> // used by FIFO
#include <netinet/in.h> // ntohl, htonl, convert between host specific and network endians.
#include <pthread.h> // used for yield
#include <stdbool.h>
#include <stdio.h> // Standard input/output
#include <stdlib.h> // Standard library
#include <string.h>
#include <sys/ioctl.h> // Input/output control, used by SPI
#include <unistd.h> // POSIX API
#include <wiringPi.h> // Raspberry Pi helper library ( -lwiringPi flag needs to be used when linking. )

// Macro defined max function
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

// Type define bool, since it does not exist in the C-language
/*
typedef int bool;
#define true 1
#define false 0
//*/

#define HEADER_SIZE 6

// FIFO
#define FIFO_COUNT 7
#define FIFO_TO 4
int fds[FIFO_COUNT];
bool fdsOpened[FIFO_COUNT];
char fifo[][255] = 
{
    "/tmp/fifo_to_control\0",
    "/tmp/fifo_to_sensor\0",
    "/tmp/fifo_to_dynmap\0",
    "/tmp/fifo_to_ladar\0",
    "/tmp/fifo_from_dbug\0",
    "/tmp/fifo_from_dynmap\0",
    "/tmp/fifo_from_ladar\0",
};

// SPI settings etc.
int spiCs0Fd;				//file descriptor for the SPI device
int spiCs1Fd;				//file descriptor for the SPI device
int spiFd;
unsigned char spiMode;
unsigned char spiBitsPerWord;
unsigned int spiSpeed;

// Interrupt flags for SPI interrupt pins
volatile bool dev0;
volatile bool dev1;

// Send/receive buffers
#define MAX_SIZE 4096
uint8_t txBuf[MAX_SIZE];
uint8_t rxBuf[MAX_SIZE];
uint8_t FifoBuf[MAX_SIZE];

struct spi_ioc_transfer spiHeader;
struct spi_ioc_transfer spiData;

struct Packet
{
    uint8_t cmd;
    uint8_t from;
    uint8_t to;
    uint8_t checksum;
    uint16_t len;
    uint8_t* data;
} packet;

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

void forwardPacket(uint8_t* buf);

// Print buffer for debugging
void printBuf(uint8_t* buf)
{
    buf[MAX_SIZE - 1] = '\0';
    fprintf(stderr, buf);
    fprintf(stderr, "\nPacket:\nCmd:%d\nFrom:%d\nTo:%d\nChecksum:%d\nLength:%d\nData:%s\n",
        packet.cmd, packet.from, packet.to, packet.checksum, packet.len, packet.data);
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
    packet->cmd                = buf[0];
    packet->from               = buf[1];
    packet->to                 = buf[2];
    packet->checksum           = buf[3];
    int netLen                 = buf[4] << 8 | buf[5];
    packet->len                = ntohs(netLen);
    packet->data               = &buf[6];
}

// SPI open port
// spiDevice 0=CS0, 1=CS1
int spiOpenPort (int spiDevice)
{
	int statusValue;

	spiMode = SPI_MODE_0;  // Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	spiBitsPerWord = 8;    // Set bits per word
	spiSpeed = 1000000;    // Set bus speed

    if (spiDevice == 0)
        spiFd = open("/dev/spidev0.0", O_RDWR);
    else
        spiFd = open("/dev/spidev0.1", O_RDWR);

    if(spiFd < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, couldn't open SPI device'");
        exit(1);
    }
    
    statusValue = ioctl(spiFd, SPI_IOC_WR_MODE, &spiMode);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI MODE (WR)");
        exit(1);
    }

    statusValue = ioctl(spiFd, SPI_IOC_RD_MODE, &spiMode);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI MODE (RD)");
        exit(1);  
    }

    statusValue = ioctl(spiFd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI BITS PER WORD (WR)");
        exit(1);
    }
    statusValue = ioctl(spiFd, SPI_IOC_RD_BITS_PER_WORD, &spiBitsPerWord);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI BITS PER WORD (RD)");
        exit(1);
    }

    statusValue = ioctl(spiFd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }
    statusValue = ioctl(spiFd, SPI_IOC_RD_MAX_SPEED_HZ, &spiSpeed);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }

    return(statusValue);
}

// SPI close port
int spiClosePort (int spiDevice)
{
    int statusValue;
    statusValue = close(spiFd);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, couldn't close SPI device'");
        exit(1);
    }

    return(statusValue);
}

// SPI write and read data
void spiWriteAndRead (int spiDevice)
{
    int ret;
    spiOpenPort(spiDevice);

    // Set receive buffers to 0
    memset(rxBuf, 0, MAX_SIZE);

    // Exchange protocol headers
    ret = ioctl(spiFd, SPI_IOC_MESSAGE(1), &spiHeader);

    if (ret < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, protocol header exchange failed.");
        exit(1);
    }

    struct Packet rxPacket;
    bufferToPacket(rxBuf, &rxPacket);

    if(rxPacket.len > (MAX_SIZE - HEADER_SIZE))
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, the packet is too big.");
        exit(1);   
    }

    spiData.len = max(packet.len, rxPacket.len);

    if(spiData.len > 0)
    {
        // Do buffer communication
        ret = ioctl(spiFd, SPI_IOC_MESSAGE(1), &spiData);

        if (ret < 0)
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, problem transmittning SPI data");
            exit(1);
        }
    }

    // Set transmitt buffer to 0
    memset(txBuf, 0, MAX_SIZE);

    if(!checkPacket(&rxPacket))
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, packet error.");
        exit(1);
    }

    spiClosePort(spiFd);

    if(rxPacket.cmd != NUL)
    {
        forwardPacket(rxBuf);
    }
}

void spiInterrupt0 (void) 
{
    dev0 = true;
}

void spiInterrupt1 (void) 
{
    dev1 = true;
}

// Init
void spiInit()
{
    wiringPiSetup();
    wiringPiISR (21, INT_EDGE_FALLING, &spiInterrupt0); // pin 29
    wiringPiISR (22, INT_EDGE_FALLING, &spiInterrupt1); // pin 31    

    dev0 = false;
    dev1 = false;

    memset(txBuf, 0, MAX_SIZE);
    memset(rxBuf, 0, MAX_SIZE);

    spiHeader.tx_buf        = (unsigned long) txBuf;
    spiHeader.rx_buf        = (unsigned long) rxBuf;
    spiHeader.len           = HEADER_SIZE;
    spiHeader.delay_usecs   = 0;
    spiHeader.speed_hz      = spiSpeed;
    spiHeader.bits_per_word = spiBitsPerWord;
    spiHeader.cs_change     = 0;

    spiData.tx_buf          = (unsigned long) (txBuf + HEADER_SIZE);
    spiData.rx_buf          = (unsigned long) (rxBuf + HEADER_SIZE);
    spiData.len             = 0;
    spiData.delay_usecs     = 0;
    spiData.speed_hz        = spiSpeed;
    spiData.bits_per_word   = spiBitsPerWord;
    spiData.cs_change       = 0;
}

// FIFO init
void fifoInit()
{
    int i, ret;
    for(i = 0; i < FIFO_COUNT; ++i)
    {
        ret = mkfifo(fifo[i], 0777);

        if (ret < 0 && errno != EEXIST)
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't create FIFO");
            exit(1);
        }
    }

    for(i = 0; i < FIFO_COUNT; ++i)
    {
        fdsOpened[i] = true;
    }

    for(i = 0; i < FIFO_TO; ++i)
    {
        fds[i] = open(fifo[i], O_RDONLY | O_NONBLOCK);
        if(fds[i] < 0)
        {
            if(errno == ENXIO)
            {
                fdsOpened[i] = false;
            }
            else
            {
                // TODO: Better error handling, e.g. do not force quit the program.
                perror("Error, can't open FIFO");
                exit(1);
            }
        }
    }

    for(i = FIFO_TO; i < FIFO_COUNT; ++i)
    {
        fds[i] = open(fifo[i], O_WRONLY | O_NONBLOCK);
        if(fds[i] < 0)
        {
            if(errno == ENXIO)
            {
                fdsOpened[i] = false;
            }
            else
            {
                // TODO: Better error handling, e.g. do not force quit the program.
                perror("Error, can't open FIFO");
                exit(1);
            }
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

    fds[fd] = open(fifo[fd], O_WRONLY | O_NONBLOCK);

    if(fds[fd] >= 0)
    {
        fdsOpened[fd] = true;
        return true;
    }

    return false;
}

void forwardPacket(uint8_t* buf)
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
    
    switch((Addr) packet.to)
    {
        case CONTROL:
        case SENSOR:
            if(buf != txBuf)
                memcpy(txBuf, buf, totalSize);
            spiWriteAndRead((int) packet.to);
            break;
        case DYNMAP:
            if(verifyFifo(2))
                write(fds[2], buf, totalSize);
            break;
        case LADAR:
            if(verifyFifo(3))
                write(fds[3], buf, totalSize);
            break;
        case DBUG:
            if(verifyFifo(4))
                write(fds[4], buf, totalSize);
            break;
        default:
            // TODO: error handling
            fprintf(stderr, "Unknow address: %d, can not forward packet\n", packet.to);
    }
}

int main(int argc, char *argv[])
{
    fprintf(stderr, "Start\n");

    spiInit();
    fifoInit();

    // Main loop
    fprintf(stderr, "Entering main loop\n");

    int ret;
    for(;;)
    {
        // FIFO Control
        ret = read(fds[0], FifoBuf, MAX_SIZE); 
        FifoBuf[ret] = '\0';

        if(ret < 0 && errno != EAGAIN) // Something is wrong and unhandled!
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't read FIFO");
            exit(1);
        }
        else if (ret > 0) // We have some data to forward
        {
            forwardPacket(FifoBuf);
        }

        // FIFO Sensor
        ret = read(fds[1], FifoBuf, MAX_SIZE); 
        FifoBuf[ret] = '\0';

        if(ret < 0 && errno != EAGAIN) // Something is wrong and unhandled!
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't read FIFO");
            exit(1);
        }
        else if (ret > 0) // We have some data to forward
        {
            forwardPacket(FifoBuf);
        }

        // SPI
        if(dev0)
        {
            memset(txBuf, 0, MAX_SIZE);
            spiWriteAndRead(0);
            dev0 = false;
        }
        
        if(dev1)
        {
            memset(txBuf, 0, MAX_SIZE);
            spiWriteAndRead(0);
            dev1 = false;
        }

        pthread_yield();
    }

    fprintf(stderr, "Exiting program\n");

    return -1;
}

#ifdef __cplusplus
}
#endif
