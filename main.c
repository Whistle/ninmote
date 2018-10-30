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

#define OFFSET 100
#define PANASONIC_HDR_MARK    (3502 - OFFSET)
#define PANASONIC_HDR_SPACE   (1750 - OFFSET)
#define PANASONIC_BIT_MARK    ( 502 - OFFSET)
#define PANASONIC_ONE_SPACE   (1244 - OFFSET)
#define PANASONIC_ZERO_SPACE  ( 400 - OFFSET)

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
	
const uint32_t btn_mute[]	= {0x4004, 0x1004C4D};


ISR (TIMER1_COMPA_vect) {
	elapsed = 1;
}

inline void setup_t1_for_t(uint16_t t) {
	TCCR1B &= ~(1<<CS10);
	cli();
	TCNT1 = 0;
	OCR1A = t - 1;
	TIMSK = (1<<OCIE1A);
	TCCR1B = (1<<WGM12) | (1<<CS10);
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
	TCCR0B |= (1<<CS00);
	TCCR0A |= (1<<COM0A0);
}

inline void disable_t0_38khz() {
	TCCR0B &= ~(1<<CS00);
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

int main(void)
{
    /* Replace with your application code */
	setup_t0_38khz();
    while (1) 
    {
			send(btn_off);
			wait_for_x_t(50000);
    }
}

