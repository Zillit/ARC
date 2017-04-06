/*
 * Sensor_module.c
 *
 * Created: 3/31/2017 8:48:54 AM
 *  Author: emida919
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define 	F_CPU   1000000UL
#include <util/delay.h>

//time0 to INT0 (16)
//time1 to INT1 (17)

volatile uint32_t timer0_overflow = 0;
volatile uint32_t timer1_overflow = 0;
volatile uint32_t time_from0 = 0;
volatile uint32_t time_from1 = 0;
long time_sec0[] = {0, 0, 0};
long time_sec1[] = {0, 0, 0};
long median_time0 = 0;
long median_time1 = 0;
long average_time = 0;
char k = 0;

long median(long time[]);
void shift(long array[], int n);
void set_zero(long time[], int n);


ISR(TIMER0_OVF_vect)
{
	timer0_overflow++;
	
	if(timer0_overflow > 100000)
	{
		set_zero(time_sec0, 2);
		timer0_overflow = 0;
	}	
}

ISR(TIMER2_OVF_vect)
{
	timer1_overflow++;
	
	if(timer1_overflow > 100000)
	{
		set_zero(time_sec1, 2);
		timer1_overflow = 0;
	}
}

ISR(INT0_vect)
{
	shift(time_sec0, 2);
	time_sec0[0] = timer0_overflow;
	median_time0 = median(time_sec0);
	timer0_overflow = 0;	
}

//*15/34/14

ISR(INT1_vect)
{
	shift(time_sec1, 2);
	time_sec1[0] = timer1_overflow;
	median_time1 = median(time_sec1);
	timer1_overflow = 0;
}

int cmpfunc (const void * a, const void * b)
{
	return ( *(long*)a - *(long*)b );
}

long median(long time[])
{
	long time_copy[] = {time[0], time[1], time[2]};
	qsort(time_copy, 3, sizeof(long), cmpfunc);
	return time_copy[1];
}


long average(long x, long y)
{
	return (x + y)/2;
}

void shift(long array[], int n)
{
	for(int i = n; i > 0; i--)
	{
		array[i] = array[i - 1];
	}
}

void set_zero(long time[], int n)
{
	for(int i = 0; i <= n; i ++)
	{
		time[i] = 0;
	}
}


int main(void)
{

	//Start clock
	TCCR0B =  (1 << CS00);		//startar timer_counter0
	TCNT0 = 0;					//speciell klocka som går långsamt
	TIMSK0 = (1 << TOIE0);		//enable overflow_interrupts 
	
	TCCR2B =  (1 << CS20);		//startar timer_counter0
	TCNT2 = 0;					//speciell klocka som går långsamt
	TIMSK2 = (1 << TOIE2);		//enable overflow_interrupts 
	
	DDRD = 0x00;				//datadirection till insignal D-reg
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00); //interrupts vid endast högflank
	EIMSK = (1 << INT1) | (1 << INT0);								   //externa interrupts sker på INT1 & INT0
	sei();								//enable interrupts
	
	
    while(1)
    {	
	   sleep_cpu();
    }
}

