/*
 * lcd.h
 *
 * Created: 04.02.2015 23:12:02
 *  Author: Андрей
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <inttypes.h>

#define E		0b00100000
#define RS		0b00010000
#define RW		0b00010000
#define PORT_Data	PORTC
#define PIN_Data	PINC
#define PORT_E		PORTB
#define PORT_RS		PORTB
#define PORT_RW		PORTD

#define setDDR_RW		DDRD |= RW; //PD4 to r/w
#define setDDR_Data_Out		DDRC |= 0b00001111;//D4-7 output	
#define setDDR_Data_In		DDRC &= ~0b00001111;//D4-7 output	
#define setDDR_E		DDRB |= E;//E output
#define setDDR_RS		DDRB |= RS;//RS output
#define WriteDisp		PORT_RW &= ~RW; //PD4 to 0
#define ReadDisp		PORT_RW |= RW; //PD4 to 0
#define E_DOWN			PORT_E  &= ~E;
#define E_UP			PORT_E  |= E;
#define RS_DOWN			PORT_RS  &= ~RS;
#define RS_UP			PORT_RS  |= RS;
#define RW_DOWN			PORT_RW  &= ~RW;
#define RW_UP			PORT_RW  |= RW;

#define STROBE			PORT_E |= E;\
						PORT_E  &= ~E;\
						_delay_ms(1);

#define MAX_LEN 20

void lcd_init(void);
void lcd_WrCmd(uint8_t Data);
void lcd_WrChr(uint8_t Data);
void lcd_setXY(bool Row, char Col);
void lcd_WrInt(uint16_t num, char q, uint8_t fill);
uint8_t lcd_readChr(void);
void lcd_WrTemp(uint16_t tp);
void lcd_WrLong(uint32_t num, bool q);
void lcd_clear(void);
bool lcd_busy(void);
void lcd_WrStr(const char *data, uint8_t len = MAX_LEN);

#endif /* LCD_H_ */