#include "spi.h"

using namespace std;

struct SpiDevice
{
    int value;
};
void SpiCom::readAndWrite(int device, char* data, unsigned int length)
{
    cout << "test" << endl;
}
int SpiCom::slaveFeedback(unsigned char command)
{
    switch(command)
    case 0x00 : 
                cout << "No return from device" << endl;
                return -1;
                break;
    case 0x01 : 
                cout << "Checksum failure" << endl;
                break;
    default :
                cout << "Unknown command" << endl;
                return -1;

}