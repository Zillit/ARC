/*
 * styrmodul.c
 *
 * Created: 4/5/2017 4:16:12 PM
 *  Author: antpe759
 */ 

#define F_CPU 1000000UL
#include <asf.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi_slave.h"

//lite variabler
//void handleNewData(uint8_t* data);

int degrees;
int speed; 
int turnTot = 0;
int speedTot = 0;


void enable_interupt(){
PCMSK0 |= 1<<PCINT12;  
PCICR |= 1<<PCIE1;
}


void init_pwm() {
DDRD |= 0xFF; //Alla D (PWM)
DDRA |= 0x00; //alla A ingångar
TCCR1A |= 1<<COM1A1 | 1<<COM1B1; //| 1<<COM1A0 | 1<<COM1B0;
TCCR1B |= 1<<WGM13 | 1<<CS11;  
ICR1 = 20000/2;
OCR1A =  1500/2;
OCR1B =  1500/2; //lek
_delay_ms(5000);
}

void turn(int degrees)
{
	if (degrees <= 10 && degrees >= -10){
		OCR1B = (1500 - degrees*500/22)/2;
	}
}

void forward(int speed){
	if(speed >= 0 && speed <= 30)
	OCR1A =  (1500 + speed*500/12)/2;
}

void mainloop(){
	while(1){
		}
}

int getSpeedData(uint8_t* data) {
	for (int i = 8; i < 16; i++) {
	speed += data[i]*2^(i-8);
	} 
    return speed;
}

int getDegreesData(uint8_t* data) {
	for (int i = 0; i < 8; i++) {
	degrees += data[i]*2^(i);
	}
    return degrees;
}

	
void handleNewData(uint8_t* data, uint16_t length){
	/*speed = getSpeedData(data);
	degrees = getDegreesData(data);
	forward(speed);
	turn(degrees);
	*/
	PORTD |= (1<<PD6);
	_delay_ms(1000);
	PORTD |= (0<<PD6);
}

int main(void)
	{
	//enable_interupt();
	init_pwm();
	
	//enable_interupt();
	
	spiSlaveInit(CONTROL, &handleNewData);
	sei();
	
	//uint8_t data[] = "random test data";
	//spiSend(SEND_DATA, DYNMAP, data, 16);
	mainloop();	
} 
