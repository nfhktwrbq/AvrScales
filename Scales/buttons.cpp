/*
 * buttons.cpp
 *
 * Created: 08.03.2017 13:08:34
 *  Author: Andrew
 */ 

#include "global.h"
#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>

void buttons_init(void)
{
	DDRBUT1 &= ~BUTTON1PIN;
	DDRBUT2 &= ~BUTTON2PIN;
	
	PORTBUT1 &= ~BUTTON1PIN;
	PORTBUT2 &= ~BUTTON2PIN;
}

bool button_get_press(char butnum)
{
	switch(butnum)
	{
		case 1:
		if((PINBUT1&BUTTON1PIN)==BUTTON1PIN) return true;
		else return false;
		break;
		
		case 2:
		if((PINBUT2&BUTTON2PIN)==BUTTON2PIN) return true;
		else return false;
		break;
		
		default:
		return false;
	}
}