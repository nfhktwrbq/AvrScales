/*
 * weight.h
 *
 * Created: 22.01.2017 23:35:51
 *  Author: Andrew
 */ 


#ifndef WEIGHT_H_
#define WEIGHT_H_

#define W_SCK	0b00000001
#define W_DT	0b00000010
#define PORT_W	PORTB
#define DDR_W	DDRB
#define PIN_W	PINB



void wght_init(void);
bool wght_busy(void);
int32_t wght_get_data(void);
void wght_power_up(void);
void wght_power_down(void);
int32_t wght_get_average(uint8_t times = 3);
int32_t wght_get_value(uint8_t times = 3);
void wght_zeroing(uint8_t times = 3);
void wght_set_offset(uint32_t offset);
uint32_t getOffset();

#endif /* WEIGHT_H_ */