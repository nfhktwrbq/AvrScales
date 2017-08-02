/*
 * statusLed.cpp
 *
 * Created: 05.03.2017 21:22:15
 *  Author: Andrew
 */ 

#include "global.h"
#include "statusLed.h"
#include <avr/io.h>
#include <util/delay.h>


//#include <avr/interrupt.h>

/*
ISR (TIMER0_OVF_vect)
{
	
}*/

void stat_led_init(void)
{
	DDRSL1 |= PINSL1;
	DDRSL2 |= PINSL2;
	DDRSL3 |= PINSL3;
	DDRSL4 |= PINSL4;
	
	PORTSL1 &= ~ PINSL1;
	PORTSL2 &= ~ PINSL2;
	PORTSL3 &= ~ PINSL3;
	PORTSL4 &= ~ PINSL4;		
}

void stat_led_set_reset(bool one, bool two,bool three,bool four)
{
	if(one) PORTSL1 |= PINSL1;
	else PORTSL1 &= ~ PINSL1;
	
	if(two) PORTSL2 |= PINSL2;
	else PORTSL2 &= ~ PINSL2;
	
	if(three) PORTSL3 |= PINSL3;
	else PORTSL3 &= ~ PINSL3;
	
	if(four) PORTSL4 |= PINSL4;
	else PORTSL4 &= ~ PINSL4;
}

void stat_led_set_reset(uint8_t stat)
{
	if(stat & 0x01) PORTSL1 |= PINSL1;
	else PORTSL1 &= ~ PINSL1;
	
	if(stat & 0x02) PORTSL2 |= PINSL2;
	else PORTSL2 &= ~ PINSL2;
	
	if(stat & 0x04) PORTSL3 |= PINSL3;
	else PORTSL3 &= ~ PINSL3;
	
	if(stat & 0x08) PORTSL4 |= PINSL4;
	else PORTSL4 &= ~ PINSL4;
}

void stat_led_check(void)
{
	stat_led_set_reset(1,0,0,0);
	_delay_ms(CHECK_TIME_MS);	
	stat_led_set_reset(0,1,0,0);
	_delay_ms(CHECK_TIME_MS);
	stat_led_set_reset(0,0,1,0);
	_delay_ms(CHECK_TIME_MS);
	stat_led_set_reset(0,0,0,1);
	_delay_ms(CHECK_TIME_MS);
	stat_led_set_reset(0,0,0,0);
}

void stat_led_blink(bool one, bool two,bool three,bool four, uint8_t n)
{
	for(uint8_t i=0;i<n;i++)
	{
		stat_led_set_reset(one, two, three, four);
		_delay_ms(200);
		stat_led_set_reset(0, 0, 0, 0);
		_delay_ms(200);
	}	
}