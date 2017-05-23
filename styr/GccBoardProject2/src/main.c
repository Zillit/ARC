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
int timerClock = 0;
int iter = 0; 
// initiates the PWM functionality in the processor and sends an 1,5ms pwm-signal to port 18 and port 19. 
void init_pwm() {
	DDRD |= 0xFF;
	TCCR1A |= 1<<COM1A1 | 1<<COM1B1;
	TCCR1B |= 1<<WGM13 | 1<<CS11; 
	TCCR1C |= 1<<WGM22 | 1<<CS11;
	ICR1 = 20000/2;
	OCR1A =  1500/2;
	OCR1B =  1500/2;
	_delay_ms(2500);
}


void spiInit(void) // Enables SPI
{
	DDR_SPI = (1<<DD_MISO);
	SPCR = (1<<SPE)|(0<<CPOL)|(1<<SPIE);
}

//Enables the clock for the "emergency" function
void enable_clocks(void)
{
	TCNT2 = 0;								
	TIMSK2 = (1 << TOIE2);
	TCCR2B = (1 << CS21) | (1 << CS20) | (1 << CS22);
	}

// Executes turning. -50 corresponds to turning maximum left, +50 corresponds to turning maximum right
void turn(int degrees)
{
	if (degrees <= 50 && degrees >= -50){
		OCR1B = (1500 - degrees*500/(22*2.5))/2;
	}
}

// Executes "forward". speed = -30 corresponds to driving backwards in maximum speed. speed = 30 corresponds to triving forward maximum speed
void forward(int speed){
	if(speed <= 30 && speed >= -30)
	OCR1A =  (1500 + speed*12)/2;
}

// Checks if there is connection with RP, i.e. if data has been sent in the past 500ms. 
void connectionCheck() {
	if(timerClock > 15) {
		turn(0);
		forward(0);
		TCCR2B = (0 << CS21) | (0 << CS20) | (0 << CS22) ;
		TCNT2 = 0;
	}
}

ISR (TIMER2_OVF_vect) //Timer0 interrupt processor, for stopping after no signal from SPI.
{
	timerClock++;
}

/*	Interrupts from SPI. Sent value from RP > 80 (i.e. MSB = 1), indicates forward. Sent value from RP < 80 (i.e. MSB = 0), indicates turning.
	if speed, the desired speed + 30 + 128 is sent RP
	if turn, the desired turn + 50 + is sent from RP */
ISR (SPI_STC_vect)
{
	if (SPDR >= 0x80)	
	{	
		forward(SPDR-0x9E); // 0x9E = hex(128 + 30)
	}
	else
	{	
		turn(SPDR-50);	
	}
	enable_clocks(); //restart clock
	timerClock = 0;
}

int main()
{	init_pwm();
	DDRA = 0xFF;
	spiInit();
	enable_clocks();
	sei();	//Enable interrupts
	while(1){
	connectionCheck();
	}
	return 0;
}
