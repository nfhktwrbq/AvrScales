/*
 * raspam.h
 *
 * Created: 04.02.2015 23:13:25
 *  Author: Андрей
 */ 


#ifndef RASPAM_H_
#define RASPAM_H_

#define TEST_MODE 0
#define WORK_MODE 1
#define CALIBRATION_MODE 2



#define ENTER_TO_CAL_MODE_DELAY 30 
#define ENTER_TO_CAL_MODE_TIMEOUT 100


#define COUNTER_MAX 10

uint8_t reg;
uint32_t zeroWeight;
uint32_t targetWeight;
uint32_t aliveCounter = 0;
uint8_t counter = 0;
char gsmBuf[MAX_LEN_OF_STRING];

#endif /* RASPAM_H_ */