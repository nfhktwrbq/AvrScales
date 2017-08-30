/*
 * weight.cpp
 *
 * Created: 22.01.2017 23:35:29
 *  Author: Andrew
 */ 
#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include "weight.h"

volatile int32_t wght_offset = 2200000;

void wght_init(void)
{
	DDR_W |= W_SCK;
	DDR_W &= ~W_DT;	
	PORT_W &= ~W_SCK;
	wght_get_value();
}

int32_t wght_get_data(void)
{
	while(wght_busy())
	{	}
	int32_t data = 0;
	for(uint8_t i = 0; i<24; i++)
	{
		PORT_W |= W_SCK;
		
		data <<= 1;
		if(wght_busy()) data++;
		
		PORT_W &= ~W_SCK;		
	}	
	PORT_W |= W_SCK;
	PORT_W &= ~W_SCK;	
	PORT_W |= W_SCK;
	PORT_W &= ~W_SCK;
	PORT_W |= W_SCK;
	PORT_W &= ~W_SCK;
	return data;		
}

int32_t wght_get_average(uint8_t times)
{
	if (times == 0) return 0;
	int32_t s = 0;
	for(uint8_t i = 0; i< times; i++)
	{		
		s += wght_get_data();
		_delay_us(1);
	}
	return s/times;
}

int32_t wght_get_value(uint8_t times)
{
	return (wght_get_average(times)-wght_offset);
}

void wght_zeroing(uint8_t times)
{
	wght_set_offset(wght_get_average(times));	
}

void wght_set_offset(int32_t offset)
{
	wght_offset = offset;
}


bool wght_busy(void)
{
	if((PIN_W & W_DT)==0)
	{
		return false;		
	}	
	else
	{
		return true;
	}		
}
void wght_power_up(void)
{
	PORT_W &= ~W_SCK;	
}
void wght_power_down(void)
{		
	PORT_W &= ~W_SCK;
	PORT_W |= W_SCK;
}

int32_t getOffset()
{	
	return wght_offset;
}