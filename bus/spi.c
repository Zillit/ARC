/** Protocol
 *
 * structure of msg:    | cmd | from | to | checksum | len | data |
 * size in bytes:       |  1  |  1   | 1  |    1     |  4  | len  |
 *
 * By Kim
 */

#include <errno.h>
#include <fcntl.h> // File control, used by SPI and FIFO
#include <linux/spi/spidev.h> // SPI
#include <linux/stat.h> // used by FIFO
#include <linux/types.h> // used by FIFO
#include <netinet/in.h> // ntohl, htonl, convert between host specific and network endians.
#include <pthread.h> // used for yield
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
typedef int bool;
#define true 1
#define false 0

#define NUM_ENDPOINTS 5
#define HEADER_SIZE 8

// FIFO
int fds[NUM_ENDPOINTS];
bool fdsOpened[NUM_ENDPOINTS];
char fifo[][255] = 
{
    "/tmp/fifocontrol\0",
    "/tmp/fifosensor\0",
    "/tmp/fifodynmap\0",
    "/tmp/fifoladar\0",
    "/tmp/fifodebug\0",
};


// SPI settings etc.
int spiCs0Fd;				//file descriptor for the SPI device
int spiCs1Fd;				//file descriptor for the SPI device
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

struct spi_ioc_transfer spi[MAX_SIZE];

struct Packet
{
    uint8_t cmd;
    uint8_t from;
    uint8_t to;
    uint8_t checksum;
    uint32_t len;
    uint8_t* data;
} packet;

// Things that are addressable over the bus
typedef enum
{
    CONTROL = 0x00,
    SENSOR = 0x01,
    DYNMAP = 0x02,
    LADAR = 0x03,
    DEBUG = 0x41, // 'A' in ascii. TODO: remove this?
} Addr;

// Commands that can be sent over the bus
typedef enum
{
    NUL = 0x00,         // Nothing to say
    CHECK_FAIL = 0x01,  // Check sum
    SEND_DATA = 0x20,
} Cmd;

// SPI open port
// spiDevice 0=CS0, 1=CS1
int SpiOpenPort (int spiDevice)
{
	int statusValue = -1;
	int *spiCsFd;
	
	spiMode = SPI_MODE_0;  // Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	spiBitsPerWord = 8;    // Set bits per word
	spiSpeed = 1000000;    // Set bus speed (1MHz)

    if (spiDevice == 0) 
        spiCsFd = &spiCs0Fd;
    else
        spiCsFd = &spiCs1Fd;
    
    if (spiDevice ==0)
        *spiCsFd = open("/dev/spidev0.0", O_RDWR);
    else
        *spiCsFd = open("/dev/spidev0.1", O_RDWR);
    
    if (*spiCsFd < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, couldn't open SPI device'");
        exit(1);
    }
    
    statusValue = ioctl(*spiCsFd, SPI_IOC_WR_MODE, &spiMode);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI MODE (WR)");
        exit(1);
    }

    statusValue = ioctl(*spiCsFd, SPI_IOC_RD_MODE, &spiMode);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI MODE (RD)");
        exit(1);  
    }

    statusValue = ioctl(*spiCsFd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI BITS PER WORD (WR)");
        exit(1);
    }
    statusValue = ioctl(*spiCsFd, SPI_IOC_RD_BITS_PER_WORD, &spiBitsPerWord);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI BITS PER WORD (RD)");
        exit(1);
    }

    statusValue = ioctl(*spiCsFd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }
    statusValue = ioctl(*spiCsFd, SPI_IOC_RD_MAX_SPEED_HZ, &spiSpeed);

    if (statusValue < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }

    return(statusValue);
}

// SPI close port
int SpiClosePort (int spiDevice)
{
    int statusValue = -1;
    int *spiCsFd;

    if (spiDevice == 0)
        spiCsFd = &spiCs0Fd;
    else
        spiCsFd = &spiCs1Fd;
    
    statusValue = close(*spiCsFd);

    if (statusValue <0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, couldn't close SPI device'");
        exit(1);
    }

    return(statusValue);
}

// SPI write and read data
int SpiWriteAndRead (int spiDevice, int length)
{
    int retVal = -1;
    int *spiCsFd;

    if (spiDevice == 0)
        spiCsFd = &spiCs0Fd;
    else
        spiCsFd = &spiCs1Fd;

    SpiOpenPort(*spiCsFd);

    // Set receive buffers to 0
    memset(rxBuf, 0, MAX_SIZE);

    // Exchange protocol headers
    retVal = ioctl(*spiCsFd, SPI_IOC_MESSAGE(HEADER_SIZE), &spi);

    if (retVal < 0)
    {
        // TODO: Better error handling, e.g. do not force quit the program.
        perror("Error, protocol header exchange failed.");
        exit(1);
    }

    // Max of send or receive.
    int rxSize;
    memcpy(&rxSize, &spi[2], 4); // Copy length from receive buffer
    rxSize = ntohl(rxSize); // Convert from network to host int
    int transferSize = max(rxSize, length); // perform check

    if(transferSize > 0)
    {
        // Do buffer communication
        struct spi_ioc_transfer* data = &spi[HEADER_SIZE];
        retVal = ioctl(*spiCsFd, SPI_IOC_MESSAGE(transferSize), &data);

        if (retVal < 0)
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, problem transmittning SPI data");
            exit(1);
        }
    }
    
    // Set transmitt buffer to 0
    memset(txBuf, 0, MAX_SIZE);

    SpiClosePort(*spiCsFd);

    return retVal;
}

void SpiInterrupt0 (void) 
{
    dev0 = true;
}

void SpiInterrupt1 (void) 
{
    dev1 = true;
}

//int SpiSend(Cmd c, Addr a, int length)
int SpiSend()
{
    switch(packet.to)
    {
        case CONTROL:
        case SENSOR:
        // OK, the device is on the SPI bus.
        break;
        default:
        // TODO: Error: Abort transmission.
        perror("Address not on SPI bus.");
        exit(1);
    }
}

// Init
void SpiInit()
{
    wiringPiSetup();
    wiringPiISR (21, INT_EDGE_FALLING, &SpiInterrupt0);
    wiringPiISR (22, INT_EDGE_FALLING, &SpiInterrupt1);    

    dev0 = false;
    dev1 = false;

    memset(txBuf, 0, MAX_SIZE);
    memset(rxBuf, 0, MAX_SIZE);

    int i;
    for(i = 0; i < MAX_SIZE; ++i)
    {
        spi[i].tx_buf        = (unsigned long) (txBuf + i); // trasmit data
        spi[i].rx_buf        = (unsigned long) (rxBuf + i); // receive data
        spi[i].len           = sizeof(*(txBuf + i));
		spi[i].delay_usecs   = 0;
		spi[i].speed_hz      = spiSpeed;
		spi[i].bits_per_word = spiBitsPerWord;
		spi[i].cs_change     = 0;
    }
}

// FIFO init
void FifoInit()
{
    int i, ret;

    fprintf(stderr, "Make fifo\n");
    for(i = 0; i < NUM_ENDPOINTS; ++i)
    {
        ret = mkfifo(fifo[i], 0777);

        if (ret < 0 && errno != EEXIST)
        {
            // TODO: Better error handling, e.g. do not force quit the program.
            perror("Error, can't create FIFO");
            exit(1);
        }
    }

    fprintf(stderr, "Open fifos\n");

    for(i = 0; i < NUM_ENDPOINTS; ++i)
    {
        fdsOpened[i] = true;
    }

    for(i = 0; i < 2; ++i)
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

    for(i = 2; i < NUM_ENDPOINTS; ++i)
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
    if(fd >= NUM_ENDPOINTS)
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

// Print buffer for debugging
void printBuf(uint8_t* buf)
{
    fprintf(stderr, "Buffer:\n");
    buf[MAX_SIZE - 1] = '\0';
    fprintf(stderr, buf);
    fprintf(stderr, "Packet:\nCmd:%d\nFrom:%d\nTo:%d\nChecksum:%d\nLength:%d\nData:%s",
        packet.cmd, packet.from, packet.to, packet.checksum, packet.len, packet.data);
}

bool checkPacket()
{
    // TODO: Verify the checksum
    if(packet.len > MAX_SIZE)
    {
        return false;
    }

    return true;
}

void extractPacket(uint8_t* buf)
{
    packet.cmd                  = buf[0];
    packet.from                 = buf[1];
    packet.to                   = buf[2];
    packet.checksum             = buf[3];
    int netLen                  = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];
    packet.len                  = ntohl(netLen);
    packet.data                 = &buf[8];
}

void forwardPacket(uint8_t* buf)
{
    extractPacket(buf);
    printBuf(buf); // TODO: remove
    checkPacket();

    int totalSize = HEADER_SIZE + packet.len;
    
    switch((Addr) packet.to)
    {
        case CONTROL:
        case SENSOR:
            if(buf != txBuf)
                memcpy(txBuf, buf, totalSize);
            txBuf[totalSize] = '\0';
            SpiWriteAndRead((int) packet.to, totalSize);
            break;
        case DYNMAP:
            if(verifyFifo(2))
                write(fds[2], buf, totalSize);
            break;
        case LADAR:
            if(verifyFifo(3))
                write(fds[3], buf, totalSize);
            break;
        case DEBUG:
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

    // SPI
    SpiInit();

    // FIFO
    int ret;
    FifoInit();

    // Main loop
    fprintf(stderr, "Entering main loop\n");
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
            forwardPacket(rxBuf);
            dev0 = false;
        }
        else if(dev1)
        {
            forwardPacket(rxBuf);
            dev1 = false;
        }

        pthread_yield();
    }

    fprintf(stderr, "Exiting program\n");

    return -1;
}
