/** Protocol
 *
 * structure of msg:    | cmd | adr | len | data | checksum |
 * size in bytes:       |  1  |  1  |  4  | len  |    1     |
 *
 * By Kim
 */

#include <fcntl.h>				//Needed for SPI port
#include <sys/ioctl.h>			//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#include <unistd.h>			    //Needed for SPI port
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>         // ntohl, htonl

#include <wiringPi.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef int bool;
#define true 1
#define false 0

// SPI settings etc.
int spiCs0Fd;				//file descriptor for the SPI device
int spiCs1Fd;				//file descriptor for the SPI device
unsigned char spiMode;
unsigned char spiBitsPerWord;
unsigned int spiSpeed;

// Interrupt flag
volatile bool dev0;
volatile bool dev1;

// Send/receive buffers
#define MAX_SIZE 4096
uint8_t txBuf[MAX_SIZE];
uint8_t rxBuf[MAX_SIZE];

struct spi_ioc_transfer spi[MAX_SIZE];

// Things that are addressable over the bus
typedef enum
{
    CONTROL = 0x00,
    SENSOR = 0x01,
} Addr;

// Commands that can be sent over the bus
typedef enum
{
    NUL = 0x00,         //Nothing to say
    CHECK_FAIL = 0x01,  //Check sum
    SEND_DATA = 0x20,

} Cmd;

//SPI open port
//spiDevice 0=CS0, 1=CS1
int SpiOpenPort (int spiDevice)
{
	int statusValue = -1;
	int *spiCsFd;
	
	spiMode = SPI_MODE_0;  // Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	spiBitsPerWord = 8;    //Set bits per word
	spiSpeed = 1000000;    //set bus speed (1MHz)

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
    const unsigned int headerSize = 6;
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
    retVal = ioctl(*spiCsFd, SPI_IOC_MESSAGE(headerSize), &spi);

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
        struct spi_ioc_transfer* data = &spi[headerSize];
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

int SpiSend(Cmd c, Addr a, int length)
{
    switch(a)
    {
        case CONTROL:
        case SENSOR:
        // OK. Continue
        break;
        default:
        // TODO: Error: Abort transmission.
        return -1;
    }

    switch(c)
    {
        case NUL:
        case CHECK_FAIL:
        // Ok, cmd exists. Continue.
        break;
        default:
        // TODO: Error. Abort transmission.
        return -1;
    }

    uint8_t checksum = 0; // TODO: Calculate checksum

    // TODO: create complete message and transmit.
    // cmd + addr + len + data + checksum
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

// TODO: FIFO

int main(int argc, char *argv[])
{
    SpiInit();
    // TODO: FIFO init

    for(;;)
    {
        // TODO: if fifo data, read fifo data
        if(dev0)
        {
            SpiSend(NUL, CONTROL, 0);
            // TODO: if received data, send to "Control program" ( the program that controls the car )
            dev0 = false;
        }
        else if(dev1)
        {
            SpiSend(NUL, SENSOR, 0);
            // TODO: if received data, send to "Control program" ( the program that controls the car )
            dev1 = false;
        }
        else
        {
            // TODO yeild
        }
    }

    return -1;
}
