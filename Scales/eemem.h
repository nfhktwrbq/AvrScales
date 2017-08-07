/*
 * eemem.h
 *
 * Created: 08.03.2017 19:30:55
 *  Author: Andrew
 */ 


#ifndef EEMEM_H_
#define EEMEM_H_

#include <stdint.h>

#define WEIGHT_ZERO 0
#define WEIGHT_TARGET 1

#define SUCCESS 0
#define WEIGHT_READ_ZEROS 1
#define WEIGHT_READ_FAILURE 2
#define WEIGHT_READWRITE_FAILURE 3
#define NOT_DEFINED 4
#define WEIGHT_WRITE_FAILURE 5
#define FAILED 0


#define ALARM 1
#define MODE 2
#define TEST_SMS 3

uint8_t eemem_get_weight(uint8_t wNum, uint32_t &weight);
uint8_t eemem_set_weight(uint8_t wNum, const uint32_t &weight);
uint8_t eemem_get_mode(uint8_t &md);
uint8_t eemem_set_mode(const uint8_t &md);

#endif /* EEMEM_H_ */