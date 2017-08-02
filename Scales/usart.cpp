/*
 * usart.cpp
 *
 * Created: 28.01.2017 12:19:55
 *  Author: Andrew
 */ 

#include <avr/io.h>
#include "usart.h"
#include <avr/interrupt.h>


ISR(USART_RX_vect)
{	
	uint8_t dummy = UDR0;
}


void usart_init(void)
{
	UBRR0 = 51; //9600 for 8 mhz
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (3<<UCSZ00);
	sei();
}

void usart_interrupt_en(void)
{
	sei();
	UCSR0B |= (1<<RXCIE0);
}

void usart_interrupt_dis(void)
{
	cli();
	UCSR0B &= ~(1<<RXCIE0);
}

bool usart_send_byte(const char data)
{
	uint32_t timeout = 0;
	while(!(UCSR0A & (1<<UDRE0)))
	{
		timeout++;
		if(timeout>SEND_BYTE_TIMEOUT)
		return false;
	}
	UDR0 = data;
	return true;	
}

uint8_t usart_receive_byte(void)
{
	uint32_t timeout = 0;
	while(!(UCSR0A & (1<<RXC0)))
	{
		timeout++;
		if(timeout>RECEIVE_BYTE_TIMEOUT)
		return END_OF_TEXT;
	}
	return UDR0;	
}

void usart_send_string(const char * a, bool crlf)
{
	while(*a!=0)
	{
		usart_send_byte(* a);
		a++;
	}
	if(crlf)
	{
		usart_send_byte(0x0d);
		usart_send_byte(0x0a);
	}	
}

void usart_receive_string(char * buf)
{
	uint8_t i = 0;
	while(*buf!=END_OF_TEXT && i < (MAX_LEN_OF_STRING-1))
	{
		*buf = usart_receive_byte();
		buf++;
		i++;
	}	
	*buf = 0;
}

