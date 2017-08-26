/*
 * Scales.cpp
 *
 * Created: 22.01.2017 22:30:49
 *  Author: Andrew
 */ 

#include "global.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "raspam.h"
#include "usart.h"
#include "eemem.h"
#include "weight.h"
#include "lcd.h"
#include "gsm.h"
#include "statusLed.h"
#include "buttons.h"
#include "adc.h"



#define LCD

#ifdef LCD
	#define LCD_MESSAGE2(p1, s1, p2, s2) lcd_message((p1), (s1), (p2), (s2))
	#define LCD_MESSAGE1(sn, p, s) lcd_message((sn), (p), (s))
	#define LCD_IMESSAGE1(sn, p, l) lcd_iMessage((sn), (p), (l))
	#define LCD_INIT _delay_ms(500);\
					 lcd_init();\
					 _delay_ms(200);\
					 lcd_clear();\
					 _delay_ms(100)
	#define LCD_OFF  lcd_off();\
					 _delay_ms(100)
	#define LCD_DELAY  _delay_ms(1000)
#endif

#ifndef LCD
	#define LCD_MESSAGE2(p1, s1, p2, s2)
	#define LCD_MESSAGE1(sn, p, s)
	#define LCD_IMESSAGE1(sn, p, l) 
	#define LCD_INIT
	#define LCD_OFF 			 
#endif


uint8_t getModeByButtons(void);
bool checkButton(uint8_t button);
bool setCalibration(void);
void enterInPowerSave(void);
uint8_t exitFromPowerSave(void);
void wdtOff(void);
void wdtOn(void);
void enterInPowerDown(void);
void tryReset(void);
void checkStatus(uint8_t status, uint8_t ledStat);
void scaleInit(void);
const char * convertWeightToString(char * buf, uint32_t weight);
const char * convertADCtoVoltage(char * buf, uint8_t adc);
void strCat(char * acceptBuf, const char * catBuf);

uint8_t reg;
uint32_t zeroWeight;
uint32_t targetWeight;
uint32_t aliveCounter = 0;
uint8_t counter = 0;
bool sleepFlag = false;
char gsmBuf[MAX_LEN_OF_STRING];
char tmpBuf[MAX_LEN_OF_STRING];

ISR(TIMER2_OVF_vect){
	if(aliveCounter < ALIVE_TIME){
		aliveCounter++;
		sleepFlag = true;			
	} else {
		aliveCounter = 0;
		sleepFlag = false;		
	}
}


int main(void)
{
	INIT_PWR;
	PWR_ON;

	wdtOff();	

	scaleInit();

	//get mode from eeprom and check status
	LCD_MESSAGE1(0, 1, "GET MODE");		
	checkStatus(eemem_get_mode(reg), STAT_LED3);	
	
	LCD_MESSAGE1(0, 1, "CLBR MODE ?");
	stat_led_blink(1,0,0,0,10);
	reg = getModeByButtons();		
	if(reg == CALIBRATION_MODE) 
	{	
		LCD_MESSAGE1(0, 1, "PASSCODE OK");			
		_delay_ms(500);
			
		if(setCalibration())
		{
			if(eemem_set_mode(WORK_MODE)==SUCCESS)
			stat_led_set_reset(1,1,1,1);
			else
			{
				stat_led_set_reset(0,1,1,0);
				LCD_MESSAGE1(0, 1, "ERROR");
				_delay_ms(5000);
				enterInPowerDown();
			}			
		}
		else
		{			
			stat_led_set_reset(1,0,0,1);
			LCD_MESSAGE1(0, 1, "ERROR");
			_delay_ms(5000);
			enterInPowerDown();
		}		
	}
	
	//get zero weight from eeprom and checking staus
	LCD_MESSAGE1(0, 1, "GETING ZWGHT");
	checkStatus(eemem_get_weight(WEIGHT_ZERO, zeroWeight), STAT_LED5);
		
	//get target weight from eeprom and checking status
	LCD_MESSAGE1(0, 1, "GETING ZWGHT");	
	checkStatus(eemem_get_weight(WEIGHT_TARGET, targetWeight), STAT_LED9);
	
	//set ofset for correct measurements
	wght_set_offset(zeroWeight);


	if(reg == TEST_SMS)
	{
		//send sms(wght_get_value()-zeroWeight)*1000/(targetWeight-zeroWeight)
	}
	
	/*if(reg==TEST_MODE)
	{
		
	}
	else if(reg==WORK_MODE)
	{
		
	}*/

	LCD_MESSAGE1(0, 1, "R = ");
	#ifdef	LCD
	lcd_setXY(1,10);
	lcd_WrLong((uint32_t)reg,1);
	_delay_ms(5000);
	lcd_setXY(1,15);
	lcd_WrLong(zeroWeight,1);		
	lcd_setXY(0,15);
	lcd_WrLong(targetWeight,1);
	_delay_ms(5000);		
	stat_led_set_reset(0,0,0,0);
	lcd_clear();
	#endif
	
    while(1)
    {
    	LCD_MESSAGE1(0, 1, "ENTER TO PSM");
    	LCD_DELAY;
    	LCD_OFF;
		
		sleep:
		enterInPowerSave();
		if(sleepFlag){			
			goto sleep;
		} 
		
		exitFromPowerSave();	

		LCD_MESSAGE1(0, 1, "EXIT FROM PSM");
		LCD_DELAY;
		
		convertWeightToString(gsmBuf, wght_get_value());
		LCD_MESSAGE1(0, 0, gsmBuf);	
		LCD_DELAY;

		//form report string for sms message
		memset(tmpBuf, 0, MAX_LEN_OF_STRING);
		strCat(tmpBuf, "W:");
		strCat(tmpBuf, gsmBuf);
		strCat(tmpBuf, "; B:");
		memset(gsmBuf, 0, MAX_LEN_OF_STRING);
		convertADCtoVoltage(gsmBuf, adc_on_get_off());
		strCat(tmpBuf, gsmBuf);

		LCD_MESSAGE2(0, "SEND SMS:", 0, tmpBuf);
		LCD_DELAY;

		//_delay_ms(5000);
	//	weight = wght_get_value();
		// #ifdef LCD		
		// lcd_clear();
		// lcd_setXY(1,12);		
		//  lcd_WrLong( wght_get_value(),1);
		//  _delay_ms(5000);
		//  #endif	
    }
}

void scaleShutDown(void)
{
	
}

uint8_t getModeByButtons(void)
{
	stat_led_set_reset(0,0,0,0);
	if(reg != CALIBRATION_MODE)
	{	
		stat_led_set_reset(1,0,0,0);	
		if(checkButton(BUTTON1))
		{
			stat_led_set_reset(0,1,0,0);
			if(checkButton(BUTTON2))
			{
				stat_led_set_reset(1,0,0,0);
				if(checkButton(BUTTON1))
				{
					return CALIBRATION_MODE;					
				}
				else
				{
					return TEST_SMS;
				}
				
			}			
		}				
	}
	stat_led_set_reset(0,0,0,0);
	return reg;
}

bool checkButton(uint8_t button)
{	
	uint8_t counter = 0;
	if(!button_get_press(button))
	{
		while(!button_get_press(button))
		{
			counter++;
			_delay_ms(ENTER_TO_CAL_MODE_DELAY);
			if(counter>ENTER_TO_CAL_MODE_TIMEOUT) return false;
		}
		return true;
	}	
	return false;
		
}

bool setCalibration(void)
{
	uint8_t led = 1;
	if(!button_get_press(BUTTON1))
	{
		while(!button_get_press(BUTTON1))
		{
			stat_led_set_reset(led);
			_delay_ms(500);
			led <<= 1;
			if(led > 0x08) led = 1;
		}
		if(eemem_set_weight(WEIGHT_ZERO, wght_get_average())!=SUCCESS) return false;
		while(button_get_press(BUTTON1))
		{
			stat_led_set_reset(1,1,1,1);
			_delay_ms(200);
			stat_led_set_reset(0,0,0,0);
			_delay_ms(200);
		}
	}
	else
	{
		return false;
	}
	if(!button_get_press(BUTTON2))
	{
		while(!button_get_press(BUTTON2))
		{
			stat_led_set_reset(led);
			_delay_ms(500);
			led >>= 1;
			if(led == 0) led = 0x08;
		}
		if(eemem_set_weight(WEIGHT_TARGET, wght_get_average())!=SUCCESS) return false;
		while(button_get_press(BUTTON2))
		{
			stat_led_set_reset(1,1,1,1);
			_delay_ms(200);
			stat_led_set_reset(0,0,0,0);
			_delay_ms(200);
		}
	}
	else
	{
		return false;
	}
	return true;
}

void enterInPowerSave(void){
	cli();	
	PWR_OFF; //disable power fron step-up converter
	TCCR2A = 0;
	TCNT2 = 0;
	TIMSK2 = 0x01; //enable interrupt
	TCCR2B = 0x07; //start timer with prescaler 1024
	sleepFlag = true;		
	/*PRR = 0xff;
	MCUCR |= 0x60; //disable BOD in sleep
	SMCR = 0x02; //goto power-save mode	*/
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sleep_enable();
	clock_prescale_set(clock_div_256);
	sei();
	sleep_cpu();
}

uint8_t exitFromPowerSave(void){
	cli();
	sleep_disable();	
	clock_prescale_set(clock_div_1);
	aliveCounter = 0;
	TCNT2 = 0;
	TIMSK2 = 0x00; //disble interrupt
	TCCR2B = 0x00; //stop timer

	scaleInit();
	sleepFlag = false;	
	//get mode from eeprom and check status
	LCD_MESSAGE1(0, 1, "GET MODE");		
	checkStatus(eemem_get_mode(reg), STAT_LED3);	

	if(reg != WORK_MODE) 
	{
		LCD_MESSAGE1(0, 1, "NOT WORK MODE!");
		stat_led_blink(0,1,0,0,15);
		tryReset();			
	}	
	sei();
	return SUCCESS;
}

void enterInPowerDown(void){
	
}

void tryReset(void){
	wdtOn();
	while(1);
}

void wdtOff(void){
	cli();
	MCUSR &= ~(1<<WDRF);
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = 0x00;	
}

void wdtOn(void){
	cli();
	WDTCSR = 0x0f;
}

void checkStatus(uint8_t status, uint8_t ledStat){
	if(status!=SUCCESS)
	{
		uint8_t counter = 0;
		while(!button_get_press(BUTTON1))
		{
			stat_led_set_reset(ledStat);
			_delay_ms(500);
			if(++counter > COUNTER_MAX)
			{				
				#ifdef LCD				
				lcd_setXY(1,5);
				lcd_WrStr("Error");
				lcd_setXY(1,15);
				lcd_WrLong(status, 0);				
				#endif
				enterInPowerDown();
			}
		}
	}
}

void scaleInit(void){
	INIT_PWR;
	PWR_ON;
	
	LCD_INIT;

	LCD_MESSAGE1(0, 1, "LED INIT");	
	stat_led_init();
	LCD_DELAY;
	
	LCD_MESSAGE1(0, 1, "BUTTONS_INIT");	
	buttons_init();
	LCD_DELAY;

	LCD_MESSAGE1(0, 1, "CHECK LED");		
	stat_led_check();
	LCD_DELAY;

	LCD_MESSAGE1(0, 1, "GSM INIT");
	gsm_init();
	LCD_DELAY;

	while(gsmBuf[5] != 'O'&& gsmBuf[6] != 'K')
	{
		gsm_send_at("AT", gsmBuf);		
		LCD_MESSAGE1(0, 1, gsmBuf);	
		LCD_DELAY;
	}
	
	LCD_MESSAGE1(0, 1, "WEIGHT INIT");	
	wght_init();
	LCD_DELAY;
}

const char * convertWeightToString(char * buf, uint32_t weight){
	bool negative = false;
	uint8_t counter = 0;
	if(weight >= 0x10000000){
		weight = ~weight - 1;
		negative = true;
	}
	uint32_t unit = (targetWeight > zeroWeight) ? (targetWeight - zeroWeight) : (zeroWeight - targetWeight);
	uint16_t modWeight = (uint16_t)(weight / unit);
	uint16_t divWeight = (uint16_t)((weight*1000/unit)%1000);
	for(uint8_t i  = 0; i < 3; i++){
		buf[counter] =  divWeight % 10 + '0';
		divWeight /= 10;
		counter++;
	} 
	buf[counter] = '.';
	counter++;
	do{
		buf[counter] =  modWeight % 10 + '0';
		modWeight /= 10;
		counter++;
	}while(modWeight > 0);
	if(negative){
		buf[counter] = '-';
	}	
	char *begin = buf;
	char *end = buf + counter - 1;
	while (begin < end) {
      *begin ^= *end ^= *begin ^= *end;
      begin++; end--;
   }
   buf[counter] = '\0';
	//sprintf(buf, "%lu.%lu", ((weight)*1000/(targetWeight-zeroWeight))/1000, ((weight)*1000/(targetWeight-zeroWeight))%1000);
	return buf;
}

const char * convertADCtoVoltage(char * buf, uint8_t adc){
	buf[0] = '0' + adc / ADC_ONE_VOLT;
	buf[1] = '.';
	buf[2] = '0' + ((adc % ADC_ONE_VOLT) * 10) / ADC_ONE_VOLT;
	buf[3] = 'V';
	return buf;
}

void initAlarmPin(void){
	DDR_ALARM &= ~ALARM_PIN; //PD2 to input
}

void strCat(char * acceptBuf, const char * catBuf){
	while(*acceptBuf)
		acceptBuf++;
	while(*catBuf){
		* acceptBuf = * catBuf; 
		acceptBuf++;
		catBuf++;
	}
}