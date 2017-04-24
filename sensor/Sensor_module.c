/*
 * Sensor_module.c
 *
 * Created: 3/31/2017 8:48:54 AM
 *  Author: emida919
 */ 

#include <asf.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define 	F_CPU   8000000UL 
#include <util/delay.h>

//#include "spi_slave.h"


volatile uint16_t timer0_overflow = 0;
volatile uint16_t timer1_overflow = 0;
volatile uint16_t time_from0 = 0;
volatile uint16_t time_from1 = 0;
uint16_t time_sec0[] = {0, 0, 0};
uint16_t time_sec1[] = {0, 0, 0};
//uint8_t median_speed = 0;
//uint8_t median_time0[] = {0, 0};
//uint8_t median_time1[] = {0, 0};
uint16_t median_time0 = 0;
uint16_t median_time1 = 0;
//const uint16_t min_diff = 1000;	
//uint16_t sent_median_time0 = 0;
//uint16_t sent_median_time1 = 0;


uint16_t median(uint16_t time[]);
void shift(uint16_t array[], int n);
void set_zero(uint16_t time[], int n);
uint16_t average(uint16_t x, uint16_t y);
int cmpfunc (const void * a, const void * b);
void handleNewData(uint8_t* data, uint16_t length);
void spiInit(void); // enable SPI


ISR(TIMER0_OVF_vect)		//Tidsräkning för hjul 1
{
	timer0_overflow++;
	
	if(timer0_overflow >= 10000) //Om en mätning tar längre än tid ca 200ms så säger vi att bilen står stilla
	{
		set_zero(time_sec0, 2);
		timer0_overflow = 0;
	}	
}

ISR(TIMER2_OVF_vect)		 //Tidsräkning för hjul 1
{
	timer1_overflow++;
	
	if(timer1_overflow >= 10000)  //Om en mätning tar längre än tid ca 200ms så säger vi att bilen står stilla
	{
		set_zero(time_sec1, 2);
		timer1_overflow = 0;
	}
}

ISR(INT0_vect)			//Avbrott då magnetsensorn känner av en magnet
{
	shift(time_sec0, 2);			//vi shiftar tidsmätningarna (arrayen) 
	time_sec0[0] = timer0_overflow;	//lägger in nytt mätvärde i arrayen
	median_time0 = median(time_sec0);
	/*
	uint8_t tmp_median_time = median(time_sec0);
	median_speed = median_speed && 0xf0;

	if(tmp_median_time != 0)
	{
		median_speed += 2454 / tmp_median_time;
	}
	*/	/*
	uint16_t tmp_median0 = median(time_sec0);
	int diff = abs(tmp_median0 - sent_median_time0);	//nuvarande median - senaste median
	
	if (diff > min_diff)	//Om det nya mätvärdet skiljer sig mer än min_diff, så uppdaterar vi hastigheten
	{
		median_time0[0] = ((tmp_median0 >> 8) && 0xff) + 0x80;  //vi delar upp det nya medianvärdet, från
		median_time0[1] = (tmp_median0 >> 0) && 0xff;			//ett 16-bitarsvärde till 8-bitarsarray
		sent_median_time0 = tmp_median0;				
		spiSend(SEND_DATA, DYNMAP, median_time0, 16);
	}
	*/

	timer0_overflow = 0;	
}


ISR(INT1_vect)			//samma som ovan fast för andra hjulet
{
	shift(time_sec1, 2);
	time_sec1[0] = timer1_overflow;
	median_time1 = median(time_sec1);

	/*
	uint8_t tmp_median_time = median(time_sec0);
	median_speed =  median_speed && 0x0f;

	if(tmp_median_time != 0)
	{
		median_speed += (2454 / tmp_median_time) << 4;
	}
	*/
	/*
	uint16_t tmp_median1 = median(time_sec1);
	int diff = abs(tmp_median1 - sent_median_time1);

	if (diff > min_diff)
	{
	median_time1[0] = (tmp_median1 >> 8) && 0xff;
	median_time1[1] = (tmp_median1 >> 0) && 0xff;
	sent_median_time1 = tmp_median1;		
	spiSend(SEND_DATA, DYNMAP, median_time1, 16);
	}
	*/

	timer1_overflow = 0;
}

int cmpfunc (const void * a, const void * b)	//hjälpfunk för att sortera värden i storleksordning
{
	return ( *(uint16_t*)a - *(uint16_t*)b );
}

uint16_t median(uint16_t time[])				//beräkning av median
{
	uint16_t time_copy[] = {time[0], time[1], time[2]};
	qsort(time_copy, 3, sizeof(uint16_t), cmpfunc);
	return time_copy[1];
}


void shift(uint16_t array[], int n)				//skiftar en array med n element EN gång
{
	for(int i = n; i > 0; i--)
	{
		array[i] = array[i - 1];
	}
}

void set_zero(uint16_t time[], int n)			//nollställer en array med n element
{
	for(int i = 0; i <= n; i ++)
	{
		time[i] = 0;
	}
}

/*
void handleNewData(uint8_t* data, uint16_t length)
{
	// Do something with the data received from the bus.
}
*/
/*
void spiInit(void) // enable SPI
{	
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(1<<SPIE);
}

ISR (SPI_STC_vect)
{
	SPDR = median_speed;
}
*/
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
				int32_t send = (median_time1 << 16) + median_time0;
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

int main(void)
{

	//Start clock
	TCNT0 = 0;			//speciell klocka som går långsamt
	TIMSK0 = (1 << TOIE0);		//enable overflow_interrupts 
	TCCR0B = (1 << CS00);		//startar timer_counter0
	
	TCNT2 = 0;			//speciell klocka som går långsamt
	TIMSK2 = (1 << TOIE2);		//enable overflow_interrupts 
	TCCR2B =  (1 << CS20);		//startar timer_counter0
	
	DDRD = 0x00;			//datadirection till insignal D-reg
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);	//interrupts vid endast högflank
	EIMSK = (1 << INT1) | (1 << INT0);	//externa interrupts sker på INT1 & INT0
	spiInit();
	//spiSlaveInit(DYNMAP, &handleNewData);
	sei();				//enable interrupts
	
    while(1)
    {}
}

// 9,4 ms är minsta tiden mellan två mätningar
