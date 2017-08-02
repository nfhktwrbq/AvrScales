/*
 * eemem.cpp
 *
 * Created: 08.03.2017 19:30:14
 *  Author: Andrew
 */ 
#include <avr/eeprom.h>
#include "eemem.h"

uint8_t mode EEMEM = 0;
uint8_t mode_check EEMEM = 0;

uint8_t testSms EEMEM = 0;
uint8_t testSms_check EEMEM = 0;

uint8_t alarm EEMEM = 0;
uint8_t alarm_check EEMEM = 0;

uint32_t zero_wght EEMEM = 0;
uint32_t zero_wght_check EEMEM = 0;

uint32_t target_wght EEMEM = 0;
uint32_t target_wght_check EEMEM = 0;

uint8_t eemem_get_weight(uint8_t wNum, uint32_t &weight)
{	
	switch(wNum)
	{
		case WEIGHT_ZERO:
		weight = eeprom_read_dword(&zero_wght);		
		if(weight == eeprom_read_dword(&zero_wght_check))
		{
			if(weight == 0xffffffff) return WEIGHT_READ_ZEROS;
			else return SUCCESS;
		}
		else
		{
			//eeprom_update_dword(&zero_wght,0);
			//eeprom_update_dword(&zero_wght_check,0);
			//weight = eeprom_read_dword(&zero_wght);
			//if(weight == eeprom_read_dword(&zero_wght_check))
			//{
			//	if(weight == 0) return WEIGHT_READ_FAILURE;
			//}			
			//else return WEIGHT_READWRITE_FAILURE;
			return WEIGHT_READ_FAILURE;
		}
		break;
		
		case WEIGHT_TARGET:
		weight = eeprom_read_dword(&target_wght);
		if(weight == eeprom_read_dword(&target_wght_check))
		if(weight == 0xffffffff) return WEIGHT_READ_ZEROS;
		else return SUCCESS;
		else
		{
			//eeprom_update_dword(&target_wght,0);
			//eeprom_update_dword(&target_wght_check,0);
			//weight = eeprom_read_dword(&target_wght);
			//if(weight == eeprom_read_dword(&target_wght_check))
			//if(weight == 0) return WEIGHT_READ_FAILURE;
			//else return WEIGHT_READWRITE_FAILURE;
			return WEIGHT_READ_FAILURE;
		}
		break;			
	}	
	return NOT_DEFINED;
}

uint8_t eemem_set_weight(uint8_t wNum, const uint32_t &weight)
{
	uint32_t w;
	switch(wNum)
	{
		case WEIGHT_ZERO:
			eeprom_update_dword(&zero_wght, weight);
			eeprom_update_dword(&zero_wght_check, weight);
			w = eeprom_read_dword(&zero_wght);
			if(w == eeprom_read_dword(&zero_wght_check)) return SUCCESS;	
			else return WEIGHT_WRITE_FAILURE;			
			break;
		
		case WEIGHT_TARGET:
			eeprom_update_dword(&target_wght,weight);
			eeprom_update_dword(&target_wght_check,weight);
			w = eeprom_read_dword(&target_wght);
			if(w == eeprom_read_dword(&target_wght_check)) return SUCCESS;
			else return WEIGHT_WRITE_FAILURE;
			break;			
	}
	return NOT_DEFINED;
}

uint8_t eemem_get_data(uint8_t &md, uint8_t type)
{
	uint8_t * dataP;
	uint8_t * data_checkP;
	switch(type)
	{
		case ALARM:
		dataP = &alarm;
		data_checkP = &alarm_check;
		break;
		
		case MODE:
		dataP = &mode;
		data_checkP = &mode_check;
		break;
		
		case TEST_SMS:
		dataP = &testSms;
		data_checkP = &testSms_check;
		break;
		
		default:
		return WEIGHT_READ_FAILURE;
	}
	
	md = eeprom_read_byte(dataP);
	if(md == eeprom_read_byte(data_checkP)) return SUCCESS;
	else
	{
		eeprom_update_byte(dataP,127);
		eeprom_update_byte(data_checkP,127);
		return WEIGHT_READ_FAILURE;
	}
}

uint8_t eemem_set_data(const uint8_t &md,  uint8_t type)
{
	uint8_t * dataP;
	uint8_t * data_checkP;
	switch(type)
	{
		case ALARM:
		dataP = &alarm;
		data_checkP = &alarm_check;
		break;
		
		case MODE:
		dataP = &mode;
		data_checkP = &mode_check;
		break;
		
		case TEST_SMS:
		dataP = &testSms;
		data_checkP = &testSms_check;
		break;
		
		default:
		return WEIGHT_WRITE_FAILURE;
	}
	uint8_t m;
	eeprom_update_byte(dataP,md);
	eeprom_update_byte(data_checkP,md);
	m = eeprom_read_byte(dataP);
	if(m == eeprom_read_byte(data_checkP)) return SUCCESS;
	else return WEIGHT_WRITE_FAILURE;
}

uint8_t eemem_get_mode(uint8_t &md)
{
	md = eeprom_read_byte(&mode);
	if(md == eeprom_read_byte(&mode_check)) return SUCCESS;
	else 
	{
		eeprom_update_byte(&mode,127);
		eeprom_update_byte(&mode_check,127);
		return WEIGHT_READ_FAILURE;	
	}		
}

uint8_t eemem_set_mode(const uint8_t &md)
{
	uint8_t m;
	eeprom_update_byte(&mode,md);
	eeprom_update_byte(&mode_check,md);
	m = eeprom_read_byte(&mode);
	if(m == eeprom_read_byte(&mode_check)) return SUCCESS;
	else return WEIGHT_WRITE_FAILURE;
}