#include "Buzzer.h"
#include "Uart.h"
#include <stdbool.h>

#define MASK(x) (1UL << (x))
#define buzzer (4) //PTD4
void init_buzzer(void){ 
	
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // activez portul D 	
		PORTD->PCR[buzzer] &= ~PORT_PCR_MUX_MASK;
		PORTD->PCR[buzzer] |= PORT_PCR_MUX(1); // GPIO
		PTD->PDDR |= MASK(buzzer); // buzzer ca output
	
}
void delay(volatile unsigned int time){
	while(time--){ //o simpla decrementare, pentru delay
		;
	}
}
void play_buzzer(volatile int duration, volatile int  frequency)
{	
	
	
	int cnt = 0;
	while(cnt != duration){ 
		PTD->PTOR = MASK(buzzer); // buzzerul va incepe sa bazaie
		delay(frequency); // delay-ul stabilit mai sus
		cnt++;
	}
	return;   
}
