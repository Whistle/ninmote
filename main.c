/*
 * ninmote.c
 *
 * Created: 23.10.2018 17:58:39
 * Author : Dom The Pom Johnson
 */ 
#define F_CPU 1e6

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define LENGTH 12

volatile uint8_t t_elapsed = 0;

uint8_t one[] = {4, 0, 0, 4, 0, 1, 0, 0, 0, 8, 0, 9};

ISR (TIMER1_COMPA_vect) {
	t_elapsed += 1;
}

void setup_t1_for_t() {
	TCCR1B &= ~(1<<CS10);
	cli();
	t_elapsed = 0;
	TCNT1 = 0;
	OCR1A = 400 - 1;
	TIMSK = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS10);
	sei();
}

void wait_for_x_t(uint8_t x) {
	setup_t1_for_t();
	while(t_elapsed < x);
}

void setup_t0_38khz() {
	DDRB |= (1<<DDB2);
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
	TCCR0A &= ~(1<<COM0A0);
}

void send(uint8_t * data)
{
	int i;
	int b;
	enable_t0_38khz();
	wait_for_x_t(8);
	disable_t0_38khz();
	wait_for_x_t(4);
	for(i = 0; i < LENGTH; i++) {
		for(b = 0; b < 4; b++) {
			if((data[i] << b) & 0b1000) {
				enable_t0_38khz();
				wait_for_x_t(1);
				disable_t0_38khz();
				wait_for_x_t(3);
			} else {
				enable_t0_38khz();
				wait_for_x_t(1);
				disable_t0_38khz();
				wait_for_x_t(1);
			}
		}
	}
	enable_t0_38khz();
	wait_for_x_t(1);
	disable_t0_38khz();
}

int main(void)
{
    /* Replace with your application code */
	setup_t0_38khz();
	
	
    while (1) 
    {
			send(one);
			wait_for_x_t(100);
    }
}

