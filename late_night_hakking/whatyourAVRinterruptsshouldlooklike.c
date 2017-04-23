//Checka så er init är identisk till denna också
void spiInit(void) // enable SPI
{	
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(0<<CPHA)|(1<<SPIE);
	DDRA |= 0xFF;
	PORTA = SPDR;



//Interrupt för sensormodul, byt ut variabeln send mot de 4 bytes som ska skickas [fyll i med något om 4 bytes är för mycket]
ISR(SPI_STC_vect)
{
	static uint8_t data[4];
	static int8_t dataindex;
	static unsigned char state = 'W';
	uint8_t inbyte;
	//read Data Register
	inbyte = SPDR;
	switch (state)
	{
		case 'W':
			if (inbyte == 0xFF)
			{
				//int32_t send = (count/4);
				int32_t send = 0x0B0C0D0E;
				//convert to an array of bytes
				SPDR = (uint8_t)(send & 0xff);
				data[0] = (uint8_t)(send & 0xff);
				data[1] = (uint8_t)((send >> 8) & 0xff);
				data[2] = (uint8_t)((send >> 16) & 0xff);
				data[3] = (uint8_t)((send >> 24) & 0xff);
				state = 'S';
				dataindex = 1;
				//SPDR = data[0]; //Can be removed(or not?)
			}
			break;
		case 'S':
			SPDR = data[dataindex];
			dataindex += 1;
				if (dataindex == 4)
				{
					state = 'W';
				}
		break;
	}
}




//Interrupt för styrmodul, bör funka att skicka in [speed, angle] direkt, kräver ev. lite längre paus i xfer
ISR (SPI_STC_vect)
{
	// om SPDR => så kör vi, annars sväng. 
	if (SPDR >= 0x80)
	{
		forward((SPDR-0x80));
	}
	else
	{
		turn((SPDR)-15);
	}
	
}