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


ISR(TIMER0_OVF_vect)
{
	timer0_overflow++;
	timer1_overflow++;
}

ISR(INT0_vect)
{
	time_from0 = timer0_overflow;
	time_sec0[2] = time_sec0[1];
	time_sec0[1] = time_sec0[0];
	time_sec0[0] = time_from0*15/34/14;
	median_time0 = median(time_sec0);
	average_time = (median_time0 + median_time1) / 2;
	timer0_overflow = 0;	
	k = 0;
}

ISR(INT1_vect)
{
	time_from1 = timer1_overflow;
	time_sec1[2] = time_sec1[1];
	time_sec1[1] = time_sec1[0];
	time_sec1[0] = time_from1*15/34/14;
	median_time1 = median(time_sec1);
	average_time = (median_time0 + median_time1) / 2;
	timer1_overflow = 0;
	k = 0;
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



int main(void)
{

	//Start clock
	TCCR0B =  (1 << CS00);		//startar timer_counter0
	TCNT0 = 0;					//speciell klocka som går långsamt
	TIMSK0 = (1 << TOIE0);		//enable overflow_interrupts 
	
	DDRD = 0x00;				//datadirection till insignal D-reg
	EICRA = (1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00); //interrupts vid endast högflank
	EIMSK = (1 << INT1) | (1 << INT0);								   //externa interrupts sker på INT1 & INT0
	sei();								//enable interrupts
	
	//unsigned int time1 = 0;
	//unsigned int time0 = 0;
	
	//unsigned int pre_tim1 = 0;
	//unsigned int pre_tim0 = 0;
	
	//set_sleep_mode(idle);  
	
    while(1)
    {	
       k = 1;
	   
	   _delay_ms(200);
	   _delay_ms(200);
	   
	   if(k == 1) 
	   {
		   average_time = 0; 
		   median_time0 = 0;
		   median_time1 = 0;
		   time_sec0[0] = 0;
		   time_sec0[1] = 0;
		   time_sec0[2] = 0;
			time_sec1[0] = 0;
			time_sec1[1] = 0;
			time_sec1[2] = 0;			
	   }
	   
    }
}

