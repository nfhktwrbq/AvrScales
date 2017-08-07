/*
 * statusLed.h
 *
 * Created: 05.03.2017 21:22:38
 *  Author: Andrew
 */ 


#ifndef STATUSLED_H_
#define STATUSLED_H_

#include <stdint.h>

#define CHECK_TIME_MS 1000

#define DDRSL1 DDRB
#define DDRSL2 DDRD
#define DDRSL3 DDRD
#define DDRSL4 DDRD

#define PORTSL1 PORTB
#define PORTSL2 PORTD
#define PORTSL3 PORTD
#define PORTSL4 PORTD

#define PINSL1 0b00000100
#define PINSL2 0b10000000
#define PINSL3 0b01000000
#define PINSL4 0b00100000

void stat_led_init(void);
void stat_led_set_reset(bool one, bool two,bool three,bool four);
void stat_led_check(void);
void stat_led_blink( bool one, bool two,bool three,bool four, uint8_t n);
void stat_led_set_reset(uint8_t stat);

#endif /* STATUSLED_H_ */