#include "spi.h"

using namespace std;

#include <fcntl.h>				//Needed for SPI port
#include <sys/ioctl.h>			//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#include <unistd.h>			    //Needed for SPI port
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>

int spi_cs0_fd;				//file descriptor for the SPI device
int spi_cs1_fd;				//file descriptor for the SPI device
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

//SPI open port
//spi_device 0=CS0, 1=CS1

int SpiOpenPort (int spi_device)
{
	int status_value = -1;
	int *spi_cs_fd;
	
	spi_mode = SPI_MODE_0;  // Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	spi_bitsPerWord = 8;    //Set bits per word
	spi_speed = 1000000;    //set bus speed (1MHz)

    if (spi_device == 0) 
        spi_cs_fd = &spi_cs0_fd;
    else
        spi_cs_fd = &spi_cs1_fd;
    
    if (spi_device ==0)
        *spi_cs_fd = open(("/dev/spidev0.0").c_str(), O_RDWR);
    else
        *spi_cs_fd = open(("/dev/spidev0.1").c_str(), O_RDWR);
    
    if (*spi_cs_fd < 0)
    {
        perror("Error, couldn't open SPI device'");
        exit(1);
    }
    
    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);

    if (status_value < 0)
    {
        perror("Couldn't set SPI MODE (WR)");
        exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);

    if (status_value < 0)
    {
        perror("Couldn't set SPI MODE (RD)");
        exit(1);  
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);

    if (status_value <0)
    {
        perror("Couldn't set SPI BITS PER WORD (WR)");
        exit(1);
    }
    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits);

    if (status_value <0)
    {
        perror("Couldn't set SPI BITS PER WORD (RD)");
        exit(1);
    }

    status_value = ioctl(*spi_cs_fd, SPI_IOC_WR_MAS_SPEED_HZ, &spi_speed);

    if (status_value < 0)
    {
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }
    status_value = ioctl(*spi_cs_fd, SPI_IOC_RD_MAS_SPEED_HZ, &spi_speed);

    if (status_value < 0)
    {
        perror("Couldn't set SPI speed (WR)");
        exit(1);
    }

    return(status_value);
}

//SPI close port
int SpiClosePort (int spi_device)
{
    int status_value = -1;
    int *spi_cs_fd;

    if (spi_device == 0)
        spi_cs_fd = &spi_cs0_fd;
    else
        spi_cs_fd = &spi_cs1_fd;
    
    status_value = close(*spi_cs_fd);

    if (status_value <0)
    {
        perror("Error, couldn't close SPI device'");
        exit(1);
    }

    return(status_value)

}

//SPI write and read data
int SpiWriteAndRead (int spi_device, unsigned char *data, int length)
{
    struct spi_ioc_transfer spi[length];
    int i = 0;
    int retVal = -1;
    int *spi_cs_fd;

    if (spi_device == 0)
        spi_cs_fd = &spi_cs0_fd;
    else
        spi_cs_fd = &spi_cs1_fd;
    
    //One byte per transfer
    for (i =0 ; i < length; i++)
    {
        memset(&spi[i], 0, sizeof (spi[i]));

        spi[i].tx_buf = (unsigned long) (data + i); // trasmit data
        spi[i].rx_buf = (unsigned long) (data + i); // receive data
        spi[i].len           = sizeof(*(data + i)) ;
		spi[i].delay_usecs   = 0 ;
		spi[i].speed_hz      = spi_speed ;
		spi[i].bits_per_word = spi_bitsPerWord ;
		spi[i].cs_change = 0;
    }

    retVal = ioctl(*spi_cs_fd, SPI_IOC_MESSAGE(length), &spi);

    if (retVal < 0)
    {
        perror("Error, problem transmittning SPI data");
        exit(1);
    }

    return retVal;
}

int SpiInterrupt ()
{
    
    // TODO: activate interrupt listener on gpio port with support for interrupts. Must be done before slaves can tell the master it has data to transmit.
}

//SPI-protocol

/** 
 * structure of msg:    | cmd | adr | len | data | checksum |
 * size in bytes:       |  1  |  1  |  4  | len  |    1     |
 */

enum class Addr : uint8_t
{
    Control = 0x00,
    Sensor = 0x01,
};

enum class Cmd : uint8_t
{
    Nul = 0x00,
    CheckFail = 0x01,

};

int send( Cmd c, Addr a, int length, uint8_t* data )
{
    switch(c)
    {
        case Cmd::Nul:
        case Cmd::CheckFail:
        // Ok, cmd exists. Continue.
        break;
        default:
        // TODO: Error. Abort transmission.
        return -1;
    }

    switch(a)
    {
        case Addr::Control:
        case Addr::Sensor:
        // OK. Continue
        break;
        default:
        // TODO: Error: Abort transmission.
        return -1;
    }

    uint8_t checksum = 0; // TODO: Calculate checksum

    // TODO: create complete message and transmit.
    // cmd + addr + len + data + checksum
}

struct SpiDevice
{
    int value;
};

void SpiCom::readAndWrite(int device, char* data, unsigned int length)
{
    cout << "test" << endl;
    slaveFeedback(0x11);
    slaveFeedback(0xFF);
    slaveFeedback(0x20);
    slaveFeedback(0x12);
}

int SpiCom::slaveFeedback(unsigned char command)
{
    switch(command) {
    case 0x00 : 
                cout << "No return from device" << endl;
                return -1;
                break;
    case 0x01 : 
                cout << "Checksum failure" << endl;
                break;
    case 0x11 : 
                cout << "Sync test" << endl;
                break;
    case 0x12 : 
                cout << "Debugging" << endl;
                break;
    case 0x20 : 
                cout << "Send data" << endl;
                break;
    case 0xFF : 
                cout << "Ready to receive" << endl;
                break;
    default :
                cout << "Unknown command" << endl;
                return -1;
    }
    return 0;

}
<<<<<<< HEAD
=======
int SpiCom::commandInterperter(unsigned char command)
{
    switch(command) {
        case 0x00 :
                cout << "testcommandInterperter" << endl;
                break;
    }
    return 0;
}
>>>>>>> 786f2e47c03cb9fc46146b444e0c09a2d23c90a0
