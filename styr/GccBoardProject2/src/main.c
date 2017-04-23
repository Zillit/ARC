#define F_CPU 8000000UL
#include <asf.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7
#define DDR_SPI DDRB


int degrees;
int speed;
int turnTot = 0;
int speedTot = 0;

void init_pwm() {
	DDRD |= 0xFF; //Alla D (PWM)
	TCCR1A |= 1<<COM1A1 | 1<<COM1B1; //| 1<<COM1A0 | 1<<COM1B0;
	TCCR1B |= 1<<WGM13 | 1<<CS11;
	TCCR1C |= 1<<WGM22 | 1<<CS11;
	ICR1 = 20000/2;
	OCR1A =  1500/2;
	OCR1B =  1500/2; //lek
	OCR2A = 200000;
	_delay_ms(5000);
	TCNT3=0x00;
}


void turn(int degrees)
{
	if (degrees <= 20 && degrees >= -20){
		OCR1B = (1500 - degrees*500/22)/2;
	}
	TCNT3=0x00;
}


void spiInit(void) // enable SPI
{	
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(1<<SPIE);
}

void forward(int speed){
	if(speed >= 0 && speed <= 30)
	OCR1A =  (1500 + speed*12)/2;
	TCNT3=0x00;
}


ISR (TIMER0_OVF_vect) //Timer0 interrupt processor. For stopping after no signal from SPI.
{
	OCR1A =  1500/2;
}

ISR (SPI_STC_vect)
{
	´// om SPDR => så kör vi, annars sväng. 
	if (SPDR >= 0x80)
	{
		forward((SPDR-0x80));
	}
	else
	{
		turn((SPDR)-15);
	}
	
}

int main()
{	init_pwm();
	spiInit();
	sei();	//Enable interrupts
	while(1)
	{
		
	}
	return 0;
}