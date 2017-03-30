#ifndef SPI_H
#define SPI_H
#include <iostream>
#include <string>


using namespace std;

class SpiCom
{
public:
    SpiCom(){};
    ~SpiCom(){};
    void readAndWrite(int device, char* data, unsigned int length);
private:
    int slaveFeedback(unsigned char command);
    int commandInterperter(unsigned char command);
};

#endif