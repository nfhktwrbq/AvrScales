/*
 * gsm.cpp
 *
 * Created: 28.01.2017 12:02:17
 *  Author: Andrew
 */ 

#include "usart.h"
#include "gsm.h"

void gsm_init(void)
{
	usart_init();
}

void gsm_send_at(const char * cmd, char * resp)
{
	usart_interrupt_dis();
	usart_send_string(cmd);
	usart_receive_string(resp);
	usart_interrupt_en();
}