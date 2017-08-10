/*
 * lcd.cpp
 *
 * Created: 04.02.2015 23:10:52
 *  Author: Андрей
 */ 

#include "global.h"
#include "lcd.h"
#include <util/delay.h>
#include <avr/io.h>





void lcd_init(void)//4 bit
{
	_delay_ms(25);
	//setDDR_RW
	//WriteDisp
	setDDR_Data_Out
	setDDR_E
	setDDR_RS
	RS_DOWN
	E_DOWN	
	PORT_Data &= ~0b00001111;
	PORT_Data |= 0x03;
	STROBE
	PORT_Data &= ~0b00001111;
	PORT_Data |= 0x03;
	STROBE
	_delay_us(45);	
	PORT_Data |= 0x03;
	STROBE
	_delay_us(45);
	PORT_Data &= ~0b00001111;
	PORT_Data |= 0x02;
	STROBE
	_delay_us(45);
	lcd_WrCmd(0x28);
	lcd_WrCmd(0x08);
	lcd_WrCmd(0x0C);
	lcd_WrCmd(0x01);
	lcd_WrCmd(0x06);
	//lcd_WrCmd(0x0B);
}

void lcd_WrCmd(uint8_t Data)
{
	//while(lcd_busy());
	_delay_us(400);
	RS_DOWN
	E_DOWN
	PORT_Data &= ~0b00001111;	
	PORT_Data|=((Data & 0xf0)>>4 );	
	STROBE		
	PORT_Data &= ~0b00001111;		
	PORT_Data |=((Data & 0x0f));	
	STROBE	
}

void lcd_WrChr(uint8_t Data)
{
	
	//while (lcd_busy());
	_delay_us(400);
	E_DOWN		
	PORT_Data &= ~0b00001111;	
	PORT_Data |=((Data & 0xf0)>>4);	
	RS_UP
	STROBE
	PORT_Data &= ~0b00001111;	
	PORT_Data |=((Data & 0x0f));	
	STROBE	
}


void lcd_setXY(bool Row, char Col)
{
	char adr;
	if(!Row)
	{
		adr=0x80;
	}
	else
	{
		adr=0xC0;
	}
	adr |= Col;
	lcd_WrCmd(adr);
}

void lcd_WrLong(uint32_t num, bool q)
{
	bool minus = false;
	if(num & 0x80000000) 
	{
		minus = true;
		num = ~num;
		num++;
	}
	bool one=0;
	if	(num<10)	one=1;
	lcd_WrCmd(0x04);//move cursor to left after adding char
	if(num==0)
	{
		lcd_WrChr('0');
	}
	
	else
	{
		while((num)!=0)
		{
			lcd_WrChr((num)%10+'0');
			num /= 10;
		}
	}	
	if(one)lcd_WrChr('0');
	if(minus) lcd_WrChr('-');
	if(q)
	{
		lcd_WrChr(' ');
		lcd_WrChr(' ');
		lcd_WrChr(' ');
		lcd_WrChr(' ');
	}
	lcd_WrCmd(0x06);//move cursor to right after adding char
}


void lcd_WrInt(uint16_t num, char q, uint8_t fill)
{	
	char w = 0;
	lcd_WrCmd(0x04);//move cursor to left after adding char
	if(num==0)
	{
		lcd_WrChr('0');
		w++;
	}		
	else
	{
		while(num>0)
		{
			lcd_WrChr(num%10+'0');
			num /= 10;	
			w++;					
		}					
	}
	while((q-w)>0)
	{
		w++;
		
		lcd_WrChr(fill);
	}
	
	lcd_WrCmd(0x06);//move cursor to right after adding char
}

void lcd_WrTemp(uint16_t tp)
{
	
	bool hun=0, ten=0, subzero=0, one=0;
	uint8_t f=0;
	if(tp>0x0fff)
	{
		subzero=1;
		tp=~tp;
		tp++;
		tp=(uint16_t)(tp*6.25);
		//tp|=0xC000;
	}
	else
	tp=(uint16_t)(tp*6.25);/*КЗХэОВ*/	
	if(tp>=100 && tp<1000) one = 1;
	if(tp<100 && tp>=10) hun=1;
	if(tp<10) ten=1;
	lcd_WrCmd(0x04);//move cursor to left after adding char
	if(tp==0)
	{
		lcd_WrChr('0');
	}	
	else
	{
		while(tp>0)
		{
			if(f>0)	lcd_WrChr(tp%10+'0');
			tp /= 10;
			f++;
			if(f==2) lcd_WrChr('.');
		}
	}
	if(hun) lcd_WrChr('0');
	if(ten) 
	{
		lcd_WrChr('0');
		lcd_WrChr('.');
		lcd_WrChr('0');
	}
	
	if(subzero) lcd_WrChr('-');
	else lcd_WrChr('+');
	if(hun || ten || one) lcd_WrChr(' ');	
	lcd_WrCmd(0x06);//move cursor to right after adding char
}
	
uint8_t lcd_readChr(void)
{
	uint8_t a=0;
	while(lcd_busy());
	RS_DOWN
	E_DOWN	
	PORT_Data &= ~0b00001111;
	setDDR_Data_In 	
	ReadDisp	
	_delay_us(4);
	RS_UP	
	E_UP
	_delay_us(4);
	a = ((PIN_Data&0x0f)<<4)&0xf0;	
	STROBE
	a |= (PIN_Data&0x0f);
	WriteDisp
	setDDR_Data_Out
	return a;
}

void lcd_clear(void)
{
	lcd_WrCmd(0x01);//clear command
	_delay_ms(500);
	//while(lcd_busy());	
}

bool lcd_busy(void)
{		
	uint8_t a=0;
	E_DOWN
	RS_DOWN	
	PORT_Data &= ~0b00001111;
	setDDR_Data_In	
	ReadDisp	
	E_UP
	_delay_us(1);
	a = ((PIN_Data&0x0f)<<4)&0xf0;
	E_DOWN
	_delay_us(1);
	E_UP
	_delay_us(1);
	a |= (PIN_Data&0x0f);
	E_DOWN
	_delay_us(1);
	WriteDisp
	setDDR_Data_Out	
	if(a&0b10000000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void lcd_WrStr(const char *data, uint8_t len)
{
	for(uint8_t i=0; *data!=0 && i<len; i++)
	{
		lcd_WrChr(*(data+i));
		
	}
}

void lcd_message(uint8_t position1, const char * firsrStringMesage, uint8_t position2, const char * secondStringMessage){
	lcd_clear();
	lcd_setXY(0, position1);
	lcd_WrStr(firsrStringMesage);
	lcd_setXY(1, position2);
	lcd_WrStr(secondStringMessage);
}

void lcd_message(uint8_t stringNumber, uint8_t position, const char * stringMessage){
	lcd_clear();
	if(!stringNumber){
		lcd_setXY(1, position);
		lcd_WrStr(stringMessage);
	} else {
		lcd_setXY(0, position);
		lcd_WrStr(stringMessage);
	}	
}

void lcd_iMessage(uint8_t stringNumber, uint8_t position, uint32_t data){
	lcd_clear();
	if(!stringNumber){
		lcd_setXY(1, position);
		lcd_WrLong(data, 1);
	} else {
		lcd_setXY(0, position);
		lcd_WrLong(data, 1);
	}	
}