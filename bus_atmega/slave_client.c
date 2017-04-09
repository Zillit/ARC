#include "spi_slave.h"

void handleNewData(uint8_t* data)
{
	// Do something with the data received from the bus.
}

int main(void)
{
	spiSlaveInit(CONTROL, &handleNewData);
	uint8_t data[] = "random test data";
	spiSend(SEND_DATA, DYNMAP, data, 16);
}
