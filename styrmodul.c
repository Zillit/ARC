/*
 * GccApplication2.c
 *
 * Created: 3/21/2017 3:29:59 PM
 *  Author: antpe759
 */ 
#define F_CPU 1000000UL 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int degrees;
int speed; 
int count = 0; 

void mainloop(){
	while(1)
	{
	_delay_ms(250);
	OCR1A =  1500;
	OCR3A =  1500;
	} 	
};


void enable_interupt(){
PCMSK0 |= 1<<PCINT0 | 1<<PCINT1 | 1<<PCINT2 | 1<<PCINT3;  
PCICR |= 1<<PCIE0;
PCIFR |= 1<<PCIE0;
sei();
}



void init_pwm() {
DDRD |= 0xFF; //PD5 output (PWM)
DDRB |= 0xFF; //PB6 output (PWM)
DDRA |= 0x00; //alla A ingångar
TCCR1A |= 1<<COM1A1 | 1<<COM1B1;
TCCR1B |= 1<<WGM13 | 1<<CS11;  
TCCR3A |= 1<<COM3A1 | 1<<COM3B1;
TCCR3B |= 1<<WGM33 | 1<<CS31;
ICR1 = 20000/2;
ICR3 = 20000/2;
OCR1A =  1500;
OCR3A =  1500;
//_delay_ms(5000);
}

 int main(void)
	{
	enable_interupt();
	init_pwm();
	mainloop(); 
}


void turn(int degrees)
{
	OCR3A =  degrees;
}


void forward(int speed){
	OCR1A =  speed;
}                                                                                                                                                                                                                                                                                                                                                                                                                                    


ISR(PCINT0_vect)
{
	//if(avbrott){
	//Hämta degrees från raspberry
	//Hämta speed från raspberry
	//turn(degrees);
	//forward(speed);
	//count = 0; 
	//return;
	//}
	return;
}