#include "spi.h"

using namespace std;

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