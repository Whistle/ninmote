#include "carrier.h"
#include "delay.h"
#include "panasonic.h"

void send(const uint32_t * data) {
	enable_carrier_frequency();
	wait_us(PANASONIC_HDR_MARK);
	disable_carrier_frequency();
	wait_us(PANASONIC_HDR_SPACE);
	for (uint32_t  mask = 1UL << (16 - 1);  mask;  mask >>= 1) {
		enable_carrier_frequency();
		wait_us(PANASONIC_BIT_MARK);
		disable_carrier_frequency();
		if ((uint32_t)0x4004 & mask)  wait_us(PANASONIC_ONE_SPACE) ;
		else                 wait_us(PANASONIC_ZERO_SPACE) ;
	}
	for (uint32_t  mask = 1UL << (32 - 1);  mask;  mask >>= 1) {
		enable_carrier_frequency();
		wait_us(PANASONIC_BIT_MARK);
		disable_carrier_frequency();
		if (*data & mask)  wait_us(PANASONIC_ONE_SPACE) ;
		else                 wait_us(PANASONIC_ZERO_SPACE) ;
	}
	enable_carrier_frequency();
	wait_us(PANASONIC_BIT_MARK);
	disable_carrier_frequency();
}
