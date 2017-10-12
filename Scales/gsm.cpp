/*
 * gsm.cpp
 *
 * Created: 28.01.2017 12:02:17
 *  Author: Andrew
 */ 
#include "global.h"
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

#include "usart.h"
#include "gsm.h"
#include "lcd.h"



bool gsm_init(void)
{
	uint8_t i = 0;
	char buf[MAX_LEN_OF_STRING];
	memset(buf, 0, MAX_LEN_OF_STRING);

	usart_init();

	while(buf[5] != 'O'&& buf[6] != 'K')
	{
		gsm_send_at("AT", buf);		
		_delay_ms(500);
		i++;
		if(i > TRY_TIMES) return false;
	}
	return true;
}

void gsm_send_at(const char * cmd, char * resp)
{
	usart_interrupt_dis();
	usart_send_string(cmd);
	usart_receive_string(resp);
	usart_interrupt_en();
}

bool gsm_check_reg(void)
{
	uint8_t i = 0;
	char buf[MAX_LEN_OF_STRING];	
	memset(buf, 0, MAX_LEN_OF_STRING);	
	
	while(1)
	{
		gsm_send_at("AT+CREG?", buf);	
		if(buf[N_POS] == NETWORK_STAT_OK && buf[STAT_POS] == REG_STAT_OK)	return  true;
		else 
		{
			_delay_ms(1000);
		}
		i++;
		if(i > TRY_TIMES) return false;
	}
	
}

bool gsm_send_sms(const char * msg, char * resp)
{

	char  buf[MAX_LEN_OF_STRING];
	memset(buf, 0, MAX_LEN_OF_STRING);
	if(!gsm_check_reg()) return false;

	memset(buf, 0, MAX_LEN_OF_STRING);
	gsm_send_at("AT+CMGF=1", buf);
	usart_interrupt_dis();

	usart_send_string(SMS_CMD, false);
	usart_send_byte(0x0d); //cr
	usart_send_byte(0x0a); 
	usart_receive_string(buf);

	usart_send_string(msg, false);
	usart_send_byte(0x1a); //send SUB for commit
	usart_receive_string(buf);
	
	usart_interrupt_en();
	return true;
	
}