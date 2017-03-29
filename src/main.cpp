// #include <iostream>
// #include <string>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <chrono>

//#include "graphics.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "test.h"
#include "defines.h"


using namespace std;


int main(int argc, char *argv[])
{
    SpiCom* spi0 = new SpiCom;
char* test=NULL;
    spi0->readAndWrite(0,test, 5);
    // graphicsInit(argc, argv);
    // glutMainLoop();
    // glutTimerFunc(10,&timer,0);
    return 0;
}
