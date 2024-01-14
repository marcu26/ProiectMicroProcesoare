#include "MKL25Z4.h"

void init_buzzer(void);
void play_buzzer(volatile int duration,volatile int frequency);
void delay(volatile unsigned int time);
void init_sw(void);
void PORTA_IRQHandler(void);