#ifndef SPI_H
#define SPI_H
#include <iostream>
#include <string>

class SpiCom
{
public:
    SpiCom(){};
    ~SpiCom(){};
    void readAndWrite(int device, char* data, unsigned int length);
private:
    int slaveFeedback(unsigned char command);
};

#endif