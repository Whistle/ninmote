/*
 * ninmote.c
 *
 * Created: 23.10.2018 17:58:39
 * Author : Dom The Pom Johnson
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define LENGTH 12

volatile uint8_t elapsed = 0;

#define OFFSET 20
#define PANASONIC_HDR_MARK    (3502 - OFFSET)
#define PANASONIC_HDR_SPACE   (1750 - OFFSET)
#define PANASONIC_BIT_MARK    ( 502 - OFFSET)
#define PANASONIC_ONE_SPACE   (1244 - OFFSET)
#define PANASONIC_ZERO_SPACE  ( 400 - OFFSET)

//uint32_t one[] = {4, 0, 0, 4, 0, 1, 0, 0, 0, 8, 0, 9};
const uint32_t one[] = {0b0100000000000100, 0b00000001000000000000100000001001};

ISR (TIMER1_COMPA_vect) {
	elapsed = 1;
}

inline void setup_t1_for_t(uint16_t t) {
	TCCR1B &= ~(1<<CS11);
	cli();
	TCNT1 = 0;
	OCR1A = t - 1;
	TIMSK = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS11);
	sei();
}

inline void wait_for_x_t(uint16_t t) {
	elapsed = 0;
	setup_t1_for_t(t);
	while(!elapsed);
}

inline void setup_t0_38khz() {
	DDRB |= (1<<DDB2);
	TCCR0A = (1<<WGM01) | (1<<COM0A0);
	TCCR0B = 0; // disable timer0
	OCR0A = 10; // frequency
}

inline void enable_t0_38khz() {
	PORTB |= (1<<PORTB2);
	TCCR0B |= (1<<CS01);
	TCCR0A |= (1<<COM0A0);
}

inline void disable_t0_38khz() {
	TCCR0B &= ~(1<<CS01);
	TCCR0A &= ~(1<<COM0A0);
	PORTB &= ~(1<<PORTB2);
}

void send(const uint32_t * data)
{
	enable_t0_38khz();
	wait_for_x_t(PANASONIC_HDR_MARK);
	disable_t0_38khz();
	wait_for_x_t(PANASONIC_HDR_SPACE);
	for (uint32_t  mask = 1UL << (16 - 1);  mask;  mask >>= 1) {
		enable_t0_38khz();
		wait_for_x_t(PANASONIC_BIT_MARK);
		disable_t0_38khz();
		if (data[0] & mask)  wait_for_x_t(PANASONIC_ONE_SPACE) ;
		else                 wait_for_x_t(PANASONIC_ZERO_SPACE) ;
	}
	for (uint32_t  mask = 1UL << (32 - 1);  mask;  mask >>= 1) {
		enable_t0_38khz();
		wait_for_x_t(PANASONIC_BIT_MARK);
		disable_t0_38khz();
		if (data[1] & mask)  wait_for_x_t(PANASONIC_ONE_SPACE) ;
		else                 wait_for_x_t(PANASONIC_ZERO_SPACE) ;
	}
	enable_t0_38khz();
	wait_for_x_t(PANASONIC_BIT_MARK);
	disable_t0_38khz();
}

int main(void)
{
    /* Replace with your application code */
	setup_t0_38khz();
    while (1) 
    {
			send(one);
			wait_for_x_t(50000);
    }
}

