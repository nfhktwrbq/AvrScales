#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <avr/io.h>

#define TIMES 3

void adc_init(void);
uint16_t adc_get(void);
uint16_t adc_get_avg(void);
void adc_off(void);
uint16_t adc_on_get_off(void);

#endif /*ADC_H_*/