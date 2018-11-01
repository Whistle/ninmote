#include <avr/io.h>
#include <avr/interrupt.h>
#include "delay.h"

volatile uint8_t elapsed = 0;

ISR (TIMER1_COMPA_vect) {
	elapsed = 1;
}

static void setup_delay(uint16_t t) {
	TCCR1B &= ~(1<<CS10);
	cli();
	TCNT1 = 0;
	OCR1A = t - 1;
	TIMSK1 = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS10);
	sei();
}

void wait_us(uint16_t t) {
	elapsed = 0;
	setup_delay(t);
	while(!elapsed);
}
