/*
 * global.h
 *
 * Created: 04.02.2015 23:12:36
 *  Author: Андрей
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

#define F_CPU 8000000UL
#define ALIVE_TIME 3500

#define STAT_LED3 0x03
#define STAT_LED5 0x05
#define STAT_LED9 0x09

#define PWR_PIN 0x10
#define ALARM_PIN 0x04

#define MAX_LEN_OF_STRING 40

#define DDR_PWR DDRC
#define PORT_PWR PORTC 

#define DDR_ALARM DDRD
#define PORT_ALARM PORTD

#define ADC_ONE_VOLT 180
#define MIN_VOLTAGE 520

#define INIT_PWR (DDR_PWR |= PWR_PIN) //PC4 to output
#define	PWR_ON (PORT_PWR |= PWR_PIN)
#define	PWR_OFF (PORT_PWR &= ~PWR_PIN)


#endif /* GLOBAL_H_ */