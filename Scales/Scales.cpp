/*
 * Scales.cpp
 *
 * Created: 22.01.2017 22:30:49
 *  Author: Andrew
 */ 




#include "global.h"
#include "raspam.h"
#include "usart.h"
#include "eemem.h"
#include "weight.h"
#include "lcd.h"
#include "gsm.h"
#include "statusLed.h"
#include "buttons.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

#define LCD


uint8_t getModeByButtons(void);
bool checkButton(uint8_t button);
bool setCalibration(void);
void enterInPowerSave(void);
void wdtOff(void);
void wdtOn(void);



ISR(TIMER2_OVF_vect){
	if(aliveCounter < ALIVE_TIME){
		aliveCounter++;
		enterInPowerSave();
	} else {
		aliveCounter = 0;
		cli();
	}
}


int main(void)
{
wdtOff();	
	
#ifdef LCD	 
	lcd_init();	
	lcd_clear();
	
	lcd_setXY(0,1);
	lcd_WrStr("LED INIT",8);
#endif
	
	stat_led_init();
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("BUTTONS INIT",12);
#endif	

	buttons_init();
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("CHECK LED",9);
#endif
	
	stat_led_check();
	
	
	gsm_init();
	while(gsmBuf[5] != 'O'&& gsmBuf[6] != 'K')
	{
		lcd_clear();
		_delay_ms(1000);
		gsm_send_at("AT", gsmBuf);
		lcd_setXY(1,0);
		lcd_WrStr(gsmBuf);
		_delay_ms(1000);
	}
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("WEIGHT INIT",12);
#endif	
	
	wght_init();
	
#ifdef LCD		
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("GETING MODE",11);
#endif

	//get mode from eeprom and check status
	checkStatus(eemem_get_mode(reg), "GET MODE", 0x03);	
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("CLBR MODE ?",11);
#endif

	stat_led_blink(1,0,0,0,10);
	reg = getModeByButtons();
		
	if(reg == CALIBRATION_MODE) 
	{
		
		#ifdef LCD
			lcd_clear();
			lcd_setXY(0,1);
			lcd_WrStr("PASSCODE OK",11);
			_delay_ms(500);
		#endif		
		if(setCalibration())
		{
			if(eemem_set_mode(WORK_MODE)==SUCCESS)
			stat_led_set_reset(1,1,1,1);
			else
			{
				stat_led_set_reset(0,1,1,0);
				_delay_ms(5000);
				enterInPowerDown();
			}			
		}
		else
		{			
			stat_led_set_reset(1,0,0,1);
			_delay_ms(5000);
			enterInPowerDown();
		}		
	}
	
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("GETING ZWGHT",12);
#endif

	//get zero weight from eeprom and checking staus
	checkStatus(eemem_get_weight(WEIGHT_ZERO, zeroWeight), "GET ZWEIGHT", 0x05);
		
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("GETING TWGHT",12);
#endif	
	
	//get target weight from eeprom and checking status
	checkStatus(eemem_get_weight(WEIGHT_ZERO, zeroWeight), "GET TWEIGHT", 0x09);
	
		
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
	lcd_clear();
	lcd_setXY(0,0);
	lcd_WrChr('R');
	lcd_setXY(1,10);
	lcd_WrLong((uint32_t)reg,1);
	_delay_ms(5000);
	lcd_setXY(1,15);
	lcd_WrLong(zeroWeight,1);	
	wght_set_offset(zeroWeight);
	lcd_setXY(0,15);
	lcd_WrLong(targetWeight,1);
	_delay_ms(5000);
	//if(eemem_set_mode(reg)!=SUCCESS) stat_led_set_reset(0,0,1,1);
	_delay_ms(2000);
	stat_led_set_reset(0,0,0,0);
	lcd_clear();
	DDRB |= 0x01;
	uint32_t weight;
	uint8_t ii=0;
	
    while(1)
    {
		enterInPowerSave();
		exitFromPowerSave();
		
		lcd_clear();
		lcd_setXY(0,0);
		 lcd_WrStr(" offset: ");
		lcd_setXY(1,12);
		lcd_WrLong( getOffset(),1);
        _delay_ms(5000);
       // PORTB |= 0x01;
		 _delay_ms(500);
	//	 PORTB &= ~0x01;
		
		ii++;
		
	//	weight = wght_get_value();
		lcd_clear();
		lcd_setXY(1,12);
		//weight = wght_get_average()-zeroWeight;
		/*weight *= 1000;
		weight /= (targetWeight-zeroWeight);*/
		 lcd_WrLong( wght_get_value(),1);
		 _delay_ms(2000);
		 
		 
		/* lcd_clear();
		 lcd_setXY(0,0);
		 lcd_WrStr("Cur - Zero = ");
		 lcd_setXY(1,12);
		 weight = wght_get_value()-zeroWeight;
		 lcd_WrLong(weight,1);
		 _delay_ms(2000);
		 lcd_clear();
		 lcd_setXY(0,0);
		 lcd_WrStr(" * 1000 = ");
		 lcd_setXY(1,12);
		 weight *=1000;
		 lcd_WrLong(weight,1);
		 _delay_ms(2000);
		 
		 lcd_clear();
		 lcd_setXY(0,0);
		 lcd_WrStr(" / unitts = ");
		 lcd_setXY(1,12);
		 weight /= (targetWeight-zeroWeight);
		 lcd_WrLong(weight,1);
		 _delay_ms(2000);*/
		 //lcd_WrStr("norm",4);
		
	//	lcd_WrLong(weight,1);
		
		// gsm_send_at("AT", buf);
		// lcd_setXY(1,11);
		 //lcd_WrStr(buf);
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
	TCCR2A = 0;
	TCNT2 = 0;
	TIMSK2 = 0x01; //enable interrupt
	TCCR2B = 0x07; //start timer with prescaler 1024
	sei();
	SMCR = 0x03; //goto power-save mode	
}

uint8_t exitFromPowerSave(void){

#ifdef LCD	
	lcd_init();	
	lcd_clear();
	
	lcd_setXY(0,1);
	lcd_WrStr("LED INIT",8);
#endif
	
	stat_led_init();
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("BUTTONS INIT",12);
#endif	

	buttons_init();
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("CHECK LED",9);
#endif
	
	stat_led_check();
	
	
	gsm_init();
	while(gsmBuf[5] != 'O'&& gsmBuf[6] != 'K')
	{
		gsm_send_at("AT", gsmBuf);
		#ifdef LCD	
		lcd_clear();
		_delay_ms(1000);		
		lcd_setXY(1,0);
		lcd_WrStr(gsmBuf);
		_delay_ms(1000);
		#endif
	}
	
#ifdef LCD	
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("WEIGHT INIT",12);
#endif	
	
	wght_init();
	
#ifdef LCD		
	lcd_clear();
	lcd_setXY(0,1);
	lcd_WrStr("GETING MODE",11);
#endif

	uint8_t status;
	status = eemem_get_mode(reg);
	if(status!=SUCCESS)
	{		
		#ifdef LCD
		lcd_setXY(1,5);
		lcd_WrStr("ErReg");
		lcd_setXY(0,5);
		lcd_WrLong((uint32_t)status,1);
		#endif
		while(1);			
	}

	stat_led_blink(1,0,0,0,10);
			
	if(reg != WORK_MODE) 
	{
		
#ifdef LCD
		lcd_clear();
		lcd_setXY(0,1);
		lcd_WrStr("NOT WORK MODE!",11);
		_delay_ms(500);
#endif		
		stat_led_blink(0,1,0,0,15);
		tryReset();
		//gsm_send_at(sms, gsmBuf);
		//enterInPowerDown();
		
	}
	
	
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

void checkStatus(uint8_t status, const char * message, uint8_t ledStat){
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
				lcd_clear();
				lcd_setXY(1,5);
				lcd_WrStr("Error");
				lcd_setXY(1,15);
				lcd_WrLong(status, 0);
				lcd_setXY(0,5);
				lcd_WrStr(message);
				#endif
				enterInPowerDown();
			}
		}
	}
}

void lcd_message()