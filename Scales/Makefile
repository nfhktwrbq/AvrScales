# Задаем имя проекта, в результате получатся файлы test.hex test.bin
TARG=test
 
CC = avr-g++
OBJCOPY = avr-objcopy
 
# Задаем из каких файлов собирать проект, можно указать несколько файлов
SRCS = buttons.cpp  eemem.cpp  global.h  gsm.h    lcd.h     Scales.cpp     statusLed.h  usart.h     weight.h \
buttons.h    eemem.h    gsm.cpp   lcd.cpp  raspam.h  statusLed.cpp  usart.cpp    weight.cpp
 
OBJS = $(SRCS:.c=.o)
 
# Задаем для какого микроконтроллера будем компилировать (atmega8)
MCU=atmega328p
 
# Флаги компилятора, при помощи F_CPU определяем частоту на которой будет работать контроллер,
CFLAGS = -mmcu=$(MCU) -Wall  -Os -Werror -lm  -mcall-prologues -DF_CPU=8000000
LDFLAGS = -mmcu=$(MCU)  -Wall  -Os  -Werror 
 
all: $(TARG)
 
$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
 
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $&lt;
 
clean:
	rm -f *.elf *.bin *.hex  $(OBJS) *.map