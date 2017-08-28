/*
 * gsm.h
 *
 * Created: 28.01.2017 12:02:35
 *  Author: Andrew
 */ 


#ifndef GSM_H_
#define GSM_H_

#define SMS_CMD "AT+CMGS=\"+79255195222\""

void gsm_init(void);
void gsm_send_at(const char * cmd, char * resp);
void gsm_send_sms(const char * cmd, char * resp);


#endif /* GSM_H_ */