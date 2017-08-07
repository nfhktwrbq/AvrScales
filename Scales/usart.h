/*
 * usart.h
 *
 * Created: 28.01.2017 12:20:23
 *  Author: Andrew
 */ 


#ifndef USART_H_
#define USART_H_

#include <stdint.h>

#define SEND_BYTE_TIMEOUT 10000
#define RECEIVE_BYTE_TIMEOUT 10000
#define END_OF_TEXT 0x03




void usart_init(void);
void usart_interrupt_en(void);
void usart_interrupt_dis(void);
bool usart_send_byte(const char data);
uint8_t usart_receive_byte(void);
void usart_send_string(const char * a, bool crlf = true);
void usart_receive_string(char * buf);
uint8_t read(void);


#endif /* USART_H_ */