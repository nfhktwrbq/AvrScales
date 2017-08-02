/*
 * buttons.h
 *
 * Created: 08.03.2017 13:08:50
 *  Author: Andrew
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON1 1
#define BUTTON2 2

#define BUTTON1PIN 0b00010000
#define BUTTON2PIN 0b00001000

#define DDRBUT1 DDRD
#define DDRBUT2 DDRD

#define PORTBUT1 PORTD
#define PORTBUT2 PORTD

#define PINBUT1 PIND
#define PINBUT2 PIND


bool button_get_press(char butnum);
void buttons_init(void);

#endif /* BUTTONS_H_ */