/*
 * ninmote.c
 *
 * Created: 23.10.2018 17:58:39
 * Author : Dom The Pom Johnson
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "bucket.h"

#define PRESSED 1
#define RELEASED 2

volatile uint8_t elapsed = 0;

#define PANASONIC_HDR_MARK    3405
#define PANASONIC_HDR_SPACE   1700
#define PANASONIC_BIT_MARK     402
#define PANASONIC_ONE_SPACE   1204
#define PANASONIC_ZERO_SPACE   320


const uint32_t btn_menu[]	= {0x1004A4B};
const uint32_t btn_apps[]	= {0x190F160};
const uint32_t btn_exit[]	= {0x100CBCA};
const uint32_t btn_back[]	= {0x1002B2A};
	
const uint32_t btn_up[]		= {0x190A938};
const uint32_t btn_right[]	= {0x100F2F3};
const uint32_t btn_down[]	= {0x100D2D3};
const uint32_t btn_left[]	= {0x1007273};
	
const uint32_t btn_ok[]		= {0x1009293};
const uint32_t btn_off[]	= {0x100BCBD};

const uint32_t btn_p_up[]	= {0x1002C2D};
const uint32_t btn_p_down[]	= {0x100ACAD};
const uint32_t btn_v_up[]	= {0x1000405};
const uint32_t btn_v_down[]	= {0x1008485};
	
const uint32_t btn_mute[]	= {0x1004C4D};


ISR (TIMER1_COMPA_vect) {
	elapsed = 1;
}

static int button_pressed(struct bucket_t *button, uint8_t pin_state) {
	static uint8_t pressed = 0;

	// update bucket level
	update_bucket(button, pin_state);

	// button pressed long time
	if(bucket_empty(button) && !pressed) {
		pressed = 1;
		return PRESSED;
	}

	// button released
	if(bucket_full(button) && pressed) {
		pressed = 0;
		return RELEASED;
	}
	return 0;
}

inline void setup_t1_for_t(uint16_t t) {
	TCCR1B &= ~(1<<CS10);
	cli();
	TCNT1 = 0;
	OCR1A = t - 1;
	TIMSK1 = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS10);
	sei();
}

inline void wait_for_x_t(uint16_t t) {
	elapsed = 0;
	setup_t1_for_t(t);
	while(!elapsed);
}

inline void setup_t0_38khz() {
	DDRD |= (1<<DDD6);
	TCCR0A = (1<<WGM01) | (1<<COM0A0);
	TCCR0B = 0; // disable timer0
	OCR0A = 14; // frequency
}

inline void enable_t0_38khz() {
	PORTD |= (1<<PORTD6);
	TCCR0B |= (1<<CS00);
	TCCR0A |= (1<<COM0A0);
}

inline void disable_t0_38khz() {
	TCCR0B &= ~(1<<CS00);
	TCCR0A &= ~(1<<COM0A0);
	PORTD &= ~(1<<PORTD6);
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
		if ((uint32_t)0x4004 & mask)  wait_for_x_t(PANASONIC_ONE_SPACE) ;
		else                 wait_for_x_t(PANASONIC_ZERO_SPACE) ;
	}
	for (uint32_t  mask = 1UL << (32 - 1);  mask;  mask >>= 1) {
		enable_t0_38khz();
		wait_for_x_t(PANASONIC_BIT_MARK);
		disable_t0_38khz();
		if (*data & mask)  wait_for_x_t(PANASONIC_ONE_SPACE) ;
		else                 wait_for_x_t(PANASONIC_ZERO_SPACE) ;
	}
	enable_t0_38khz();
	wait_for_x_t(PANASONIC_BIT_MARK);
	disable_t0_38khz();
}

void setup_inputs()
{
	DDRB &= ~(1<<DDB1);
	PORTB |= (1<<PORTB1);
}

int main(void)
{
	uint8_t state = 0;
	struct bucket_t pb;
	init_bucket(&pb,200); 
    /* Replace with your application code */
	setup_t0_38khz();
	setup_inputs();
    while (1) 
    {
			state = button_pressed(&pb, !(PINB & (1<<PINB1)));
			if(state == PRESSED) {
				send(btn_off);
			}
    }
}

