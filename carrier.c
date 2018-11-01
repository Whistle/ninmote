#include <avr/io.h>

void setup_carrier_frequency() {
	TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
	TCCR0B = (1<<WGM02);
	
	/* OC0B to output */
	DDRD |= (1<<DDD5);
	/* 1MHz / 26 = ~38461 Hz */
	OCR0A = 26;
	/* 20% duty cycle */
	OCR0B = OCR0A / 5;
}

void enable_carrier_frequency() {
	TCNT0 = 0;
	PORTD |= (1<<PORTD5);
	TCCR0A |= (1<<COM0A1) | (1<<COM0B1);
	TCCR0B |= (1<<CS00);
}

void disable_carrier_frequency() {
	TCCR0B &= ~(1<<CS00);
	TCCR0A &= ~(1<<COM0A1 | 1<<COM0B1);
	PORTD &= ~(1<<PORTD5);
}
