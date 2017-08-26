#include "adc.h"

void adc_init(void)
{
	ADMUX = 0xe5; //internal reference 1.1 V, 5 ADC channel 
	ADCSRA |= 0xc6; //enable ADC with prescale 64 (optimal 50 - 200 kHz)
	ADCSRB |= 0x00; //free running mode
	DIDR0 = 0x20; // disable digital output on 5 channel
}

uint8_t adc_get(void)
{
	ADCSRA |= 0x40; //start convertion
	while(ADCSRA & 0x40);
	return ADCH;
}

uint8_t adc_get_avg(void)
{
	uint8_t avg = 0;
	for(uint8_t i = 0; i < TIMES; i++)
	{
		avg += adc_get();
	}
	return avg/TIMES;
}

void adc_off(void)
{
	ADCSRA = 0x00;
}

uint8_t adc_on_get_off(void)
{
	uint8_t avg = 0;
	adc_init();
	avg = adc_get_avg();
	adc_off();
	return avg;
}