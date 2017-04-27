/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
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
void enable_clocks();
void enable_sensors();


ISR(TIMER0_OVF_vect)	//Tidsr�kning f�r hjul 1
{
	timer0_overflow++;
	
	if(timer0_overflow >= 200)	 //Om en m�tning tar l�ngre �n tid ca 200ms s� s�ger vi att bilen st�r stilla
	{
		set_zero(time_sec0, 2);
		median_time0 = 0;
		timer0_overflow = 0;
	}	
}


ISR(TIMER2_OVF_vect)	//Tidsr�kning f�r hjul 1
{
	timer1_overflow++;
		
	if(timer1_overflow >= 200)  //Om en m�tning tar l�ngre �n tid ca 200ms s� s�ger vi att bilen st�r stilla
	{
		set_zero(time_sec1, 2);
		median_time1 = 0;
		timer1_overflow = 0;
	}
}


ISR(INT0_vect)	//Avbrott d� magnetsensorn k�nner av en magnet
{
	shift(time_sec0, 2);	//shiftar tidsm�tningarna (arrayen) 
	time_sec0[0] = timer0_overflow;		//l�gger in nytt m�tv�rde i arrayen
	median_time0 = median(time_sec0);
	timer0_overflow = 0;	
}


ISR(INT1_vect)	//samma som ovan fast f�r andra hjulet
{
	shift(time_sec1, 2);
	time_sec1[0] = timer1_overflow;
	median_time1 = median(time_sec1);
	timer1_overflow = 0;
}


int cmpfunc (const void * a, const void * b)	//hj�lpfunk f�r att sortera v�rden i storleksordning
{
	return ( *(uint16_t*)a - *(uint16_t*)b );
}


uint16_t median(uint16_t time[])	//ber�kning av median
{
	uint16_t time_copy[] = {time[0], time[1], time[2]};
	qsort(time_copy, 3, sizeof(uint16_t), cmpfunc);
	return time_copy[1];
}


void shift(uint16_t array[], int n)		//skiftar en array med n element EN g�ng
{
	for(int i = n; i > 0; i--)
	{
		array[i] = array[i - 1];
	}
}


void set_zero(uint16_t time[], int n)	//nollst�ller en array med n element
{
	for(int i = 0; i <= n; i ++)
	{
		time[i] = 0;
	}
}


void spiInit(void) // enable SPI
{	
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(0<<CPHA)|(1<<SPIE)|(1<<SPR1);
	uint8_t tmp =  SPDR;
}


//Interrupt f�r sensormodul, byt ut variabeln send mot de 4 bytes som ska skickas [fyll i med n�got om 4 bytes �r f�r mycket]
ISR(SPI_STC_vect)
{
	uint8_t inbyte;
	//read Data Register
	inbyte = SPDR;
	switch (state)
	{
		case 'W':
			if (inbyte == 0xFF)
			{
				//convert to an array of bytes
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


void enable_clocks()
{
	//Start clock
	TCNT0 = 0;			//speciell klocka som g�r l�ngsamt
	TIMSK0 = (1 << TOIE0);		//enable overflow_interrupts
	TCCR0B = (1 << CS01) | (1 << CS00);		//startar timer_counter0
	
	TCNT2 = 0;			//speciell klocka som g�r l�ngsamt
	TIMSK2 = (1 << TOIE2);		//enable overflow_interrupts
	TCCR2B =  (1 << CS22);// | (1 << CS20);		//startar timer_counter0
}


void enable_sensors()
{
	DDRD = 0x00;			//datadirection till insignal D-reg
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00);	//interrupts vid endast h�gflank
	EIMSK = (1 << INT1) | (1 << INT0);	//externa interrupts sker p� INT1 & INT0
}


int main(void)
{
	enable_clocks();
	enable_sensors();
	spiInit();
	sei();				//enable interrupts
	
    while(1)
    {}
}