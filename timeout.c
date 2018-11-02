#include <avr/io.h>
#include <avr/interrupt.h>
#include "timeout.h"

#define TIMEOUT_COUNT 1

volatile uint8_t timedout = 0;

ISR (TIMER2_OVF_vect) {
	timedout += 1;
}

void setup_timeout() {
	cli();
	TCNT2 = 0;
	TIMSK2 = (1<<TOIE2);
	TCCR2B = (1<<CS20) | (1<<CS21) | (1<<CS22);
	sei();
}

void reset_timeout() {
	setup_timeout();
	timedout = 0;
}

uint8_t timeout() {
	if(timedout > TIMEOUT_COUNT) {
		timedout = 0;
		return 1;
	}
	
	return 0;
}
