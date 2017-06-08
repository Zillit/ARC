#include <asf.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

volatile uint16_t timer0_overflow = 0;
volatile uint16_t timer1_overflow = 0;
uint16_t time_sec0[] = {0, 0, 0};
uint16_t time_sec1[] = {0, 0, 0};
uint16_t median_time0 = 0;
uint16_t median_time1 = 0;

unsigned char state = 'W';
uint8_t data[4];
int8_t dataindex;

#define DDR_SPI DDRB
#define DD_MISO 6

uint16_t median(uint16_t time[]);
void shift(uint16_t array[], int n);
void set_zero(uint16_t time[], int n);
uint16_t average(uint16_t x, uint16_t y);
int cmpfunc (const void * a, const void * b);
void handleNewData(uint8_t* data, uint16_t length);
void spiInit(void);
void enable_clocks(void);
void enable_sensors(void);


ISR(TIMER2_OVF_vect)	//Tidsräkning tills nästa avbrott från halleffektsensorn
{
	timer0_overflow++;
		
	if(timer0_overflow >= 255)	 //Om en mätning tar längre än tid ca 260ms så säger vi att bilen står stilla
	{
		set_zero(time_sec0, 2);
		median_time0 = 0;
		timer0_overflow = 0;
		
	}
	
	timer1_overflow++;
		
	if(timer1_overflow >= 255)  //Om en mätning tar längre än tid ca 260ms så säger vi att bilen står stilla
	{
		set_zero(time_sec1, 2);
		median_time1 = 0;
		timer1_overflow = 0;
	}
}


ISR(INT0_vect)	//Avbrott då magnetsensorn känner av en magnet
{
	shift(time_sec0, 2);	//Skiftar tidsmätningarna (i arrayen)
	time_sec0[0] = timer0_overflow;
	median_time0 = median(time_sec0);
	timer0_overflow = 0;	
}


ISR(INT1_vect)	//Samma som ovan fast för andra hjulet
{
	shift(time_sec1, 2);
	time_sec1[0] = timer1_overflow;
	median_time1 = median(time_sec1);
	timer1_overflow = 0;
}


int cmpfunc (const void * a, const void * b)	//Hjälpfunktion för att sortera värden i storleksordning
{
	return ( *(uint16_t*)a - *(uint16_t*)b );
}


uint16_t median(uint16_t time[])	//Beräkning av median
{
	uint16_t time_copy[] = {time[0], time[1], time[2]};
	qsort(time_copy, 3, sizeof(uint16_t), cmpfunc);
	return time_copy[1];
}


void shift(uint16_t array[], int n)		//Skiftar en array med n element EN gång
{
	for(int i = n; i > 0; i--)
	{
		array[i] = array[i - 1];
	}
}


void set_zero(uint16_t time[], int n)	//Nollställer en array med n element
{
	for(int i = 0; i <= n; i ++)
	{
		time[i] = 0;
	}
}


void spiInit(void) //Aktiverar SPI
{	
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(0<<CPHA)|(1<<SPIE)|(1<<SPR1);
	uint8_t tmp =  SPDR;
}


//Avbrott för sensormodul, byt ut variabeln send mot de 4 bytes som ska skickas [fyll i med något om 4 bytes är för mycket]
ISR(SPI_STC_vect)
{
	uint8_t inbyte;
	//SPI-register
	inbyte = SPDR;
	switch (state)
	{
		case 'W':
			if (inbyte == 0xFF)
			{
				//Konverterar en array med bytes
				SPDR = (uint8_t)(median_time0  & 0xff);
				data[0] = (uint8_t)(median_time0  & 0xff);
				data[1] = (uint8_t)((median_time0 >> 8) & 0xff);
				data[2] = (uint8_t)((median_time1) & 0xff);
				data[3] = (uint8_t)((median_time1 >> 8) & 0xff);
				state = 'S';
				dataindex = 1;
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


void enable_clocks(void)
{
	TCNT2 = 0;				//Nollställer klockan
	TIMSK2 = (1 << TOIE2);			//Aktiverar overflow_interrupts
	TCCR2B = (1 << CS21) | (1 << CS20); 	//Startar timer_counter2
}


void enable_sensors(void)
{
	DDRD = 0x00;			//Datariktning till input D-reg
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);	//Sätter avbrott vid hög flank
	EIMSK = (1 << INT1) | (1 << INT0);	//Externa interrupts sker på INT1 & INT0
}


int main(void)
{
	enable_clocks();
	enable_sensors();
	spiInit();
	sei();				//Aktiverar avbrott
	
    while(1)
    {}
}
