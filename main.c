/*
 * ninmote.c
 *
 * Created: 23.10.2018 17:58:39
 * Author : Dom The Pom Johnson
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

#include "panasonic.h"
#include "bucket.h"
#include "carrier.h"
#include "delay.h"
#include "timeout.h"

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


#define PRESSED 1
#define RELEASED 2

ISR (PCINT0_vect) {
	
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

void setup_inputs()
{
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT1);
	
	DDRB &= ~(1<<DDB1);
	PORTB |= (1<<PORTB1);
}

int main(void)
{
	uint8_t state = 0;
	struct bucket_t pb;
	init_bucket(&pb,200); 
	
	setup_carrier_frequency();
	setup_inputs();
	setup_timeout();
    while (1) 
    {
			state = button_pressed(&pb, !(PINB & (1<<PINB1)));
			if(state == PRESSED) {
				send(btn_off);
			}
			
			if(timeout()) {
				set_sleep_mode(SLEEP_MODE_PWR_DOWN);
				cli();
				sleep_enable();
				sei();
				sleep_cpu();
				sleep_disable();
				sei();
				reset_timeout();
			}
    }
}

