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
void initAlarm(void);
void gsmPrepare(void);
int32_t average(int32_t val1, int32_t val2);

uint8_t reg;
int32_t zeroWeight;
int32_t targetWeight;
int32_t aliveCounter = 0;
int32_t averageWeight = 0;
int32_t averageVoltage = 0;
uint8_t counter = 0;
bool sleepFlag = false;
bool alarmFlag = false;
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

ISR(INT0_vect){
	alarmFlag = true;
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
		
	LCD_MESSAGE1(0, 1, "GETING ZWGHT");
	checkStatus(eemem_get_weight(WEIGHT_ZERO, (uint32_t & )zeroWeight), STAT_LED5);
			
	LCD_MESSAGE1(0, 1, "GETING TWGHT");	
	checkStatus(eemem_get_weight(WEIGHT_TARGET, (uint32_t & )targetWeight), STAT_LED9);
	
	wght_set_offset(zeroWeight);
		
    snprintf(gsmBuf, MAX_LEN_OF_STRING, "ZW = %li", zeroWeight);
    snprintf(tmpBuf, MAX_LEN_OF_STRING, "TW = %li", targetWeight);

    LCD_MESSAGE2(0, gsmBuf, 0, tmpBuf);
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;

    while(1)
    {

    	averageWeight = average(averageWeight, wght_get_value());
    	averageVoltage = average(averageVoltage, (uint32_t)adc_on_get_off());

    	LCD_MESSAGE1(0, 1, "ENTER TO ADC");
    	LCD_DELAY;

    	averageWeight = average(averageWeight, wght_get_value());
    	averageVoltage = average(averageVoltage, (uint32_t)adc_on_get_off());
		
		snprintf(gsmBuf, MAX_LEN_OF_STRING, "%.3f", averageWeight/double(targetWeight - zeroWeight));
		    	 
		memset(tmpBuf, 0, MAX_LEN_OF_STRING);

		if(alarmFlag){
			strcat(tmpBuf, "ALARM! ");
			alarmFlag = false;
		} 

		strcat(tmpBuf, "W:");
		strcat(tmpBuf, gsmBuf);
		
		strcat(tmpBuf, "; B:");
		memset(gsmBuf, 0, MAX_LEN_OF_STRING);		
    			
		snprintf(gsmBuf, MAX_LEN_OF_STRING, "%.2f",averageVoltage/double(ADC_ONE_VOLT));		
		strcat(gsmBuf, "V");		
		strcat(tmpBuf, gsmBuf);

		LCD_MESSAGE2(0, "SEND SMS:", 0, tmpBuf);

		gsmPrepare();
		if(gsm_send_sms(tmpBuf, gsmBuf)){
			LCD_MESSAGE1(0, 1,"SEND SMS OK");
		}
		else{
		 LCD_MESSAGE1(0, 1,"SEND SMS FAIL");
		}
		LCD_DELAY;


    	LCD_MESSAGE1(0, 1, "ENTER TO PSM");
    	LCD_DELAY;
    	LCD_OFF;
		
		sleep:
		enterInPowerSave();
		if(alarmFlag) {
			sleepFlag = false;
		}
		if(sleepFlag){			
			goto sleep;
		} 
		
		exitFromPowerSave();	

		averageWeight = wght_get_value();
		averageVoltage = (uint32_t)adc_on_get_off();

		LCD_MESSAGE1(0, 1, "EXIT FROM PSM");
		LCD_DELAY;		
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
	if(adc_on_get_off() > MIN_VOLTAGE){
		LCD_MESSAGE1(0, 1, "TRY RESET");
		LCD_DELAY;	
		tryReset();
	} 
	else {
		LCD_MESSAGE1(0, 1, "GO DOWN");
		LCD_DELAY;	
		LCD_OFF;
		PWR_OFF;
		cli();
		enterInPowerSave();
	}
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
				LCD_DELAY;				
				#endif
				enterInPowerDown();
			}
		}
	}
}

void scaleInit(void){
	INIT_PWR;
	PWR_ON;
	_delay_ms(500);
	
	LCD_INIT;
	initAlarm();

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

	if(gsm_init()){
		gsm_send_at("AT+CFUN=0", gsmBuf); //power minimal func
	}
				
	LCD_MESSAGE1(0, 1, "WEIGHT INIT");	
	wght_init();
	LCD_DELAY;
}

void gsmPrepare(void){
	gsm_send_at("AT+CFUN=1", gsmBuf); //power norm func	
	_delay_ms(1500);
	if(gsm_init()){
		LCD_MESSAGE1(0, 1, "GSM INIT OK");
	} else {
		LCD_MESSAGE1(0, 1, "GSM INIT FAIL");
		stat_led_set_reset(0,1,1,1);
		enterInPowerDown();
	}
	LCD_DELAY;

	LCD_MESSAGE1(0, 1, "GSM REG");
	if(gsm_check_reg()){
		LCD_MESSAGE1(0, 1, "GSM REG OK");
	} else {
		LCD_MESSAGE1(0, 1, "GSM REG FAIL");
		stat_led_set_reset(1,1,1,0);
		enterInPowerDown();
	}
	LCD_DELAY;
}


void initAlarm(void){
	DDR_ALARM &= ~ALARM_PIN; //PD2 to input
	PORT_ALARM &= ~ALARM_PIN; //ALARM_PIN to down
	EICRA = 0x03; //0x03 rising edge generate interrupt   //0x01; //logical change on pd2 generate intrrupt
	EIMSK = 0x01; //enable interrupt on pd2

}

int32_t average(int32_t val1, int32_t val2){
	return (val1 + val2) / 2;
}