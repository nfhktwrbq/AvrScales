/*
 * gsm.cpp
 *
 * Created: 28.01.2017 12:02:17
 *  Author: Andrew
 */ 

#include <string.h>

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

void gsm_send_sms(const char * msg, char * resp)
{
	gsm_send_at(SMS_CMD, resp);
	gsm_send_at(msg, resp);
	usart_send_byte(0x1a); //send SUB for commit
}