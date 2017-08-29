/*
 * gsm.h
 *
 * Created: 28.01.2017 12:02:35
 *  Author: Andrew
 */ 


#ifndef GSM_H_
#define GSM_H_

#define SMS_CMD "AT+CMGS=\"+79255195222\""


#define NETWORK_STAT_OK '0'
#define REG_STAT_OK '1'
#define N_POS 18
#define STAT_POS 20
#define TRY_TIMES 4

bool gsm_init(void);
bool gsm_check_reg(void);
void gsm_send_at(const char * cmd, char * resp);
bool gsm_send_sms(const char * cmd, char * resp);


#endif /* GSM_H_ */