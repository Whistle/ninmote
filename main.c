/*
 * ninmote.c
 *
 * Created: 23.10.2018 17:58:39
 * Author : Dom The Pom Johnson
 */ 
#define F_CPU 1e6

#include <avr/io.h>
#include <util/delay.h>


void setup_t0_38khz() {
	DDRB = (1<<DDB2);
	TCCR0A = (1<<WGM01) | (1<<COM0A0);
	TCCR0B = 0; // disable timer0
	OCR0A = 12; // frequency
}

void enable_t0_38khz() {
	TCCR0B |= (1<<CS00);
	TCCR0A |= (1<<COM0A0);
}

void disable_t0_38khz() {
	TCCR0B &= ~(1<<CS00);
	PORTB &= ~(1<<PORTB2);
	TCCR0A &= ~(1<<COM0A0);
}

int main(void)
{
    /* Replace with your application code */
	setup_t0_38khz();
	
    while (1) 
    {
		_delay_ms(20);
		enable_t0_38khz();
		_delay_ms(20);
		disable_t0_38khz();
    }
}

