#include "Pit.h"
#include "Uart.h"

extern int isOpen;
extern int direction;
int state = 0;


#define RED_LED_PIN (18)   // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1)   // PORT D

void PIT_Init(void) 
{
	
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	SIM->CLKDIV1 = 0;
	SIM->CLKDIV1 |= 0x10010000;
	
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 0.99 secunde
	PIT->CHANNEL[0].LDVAL = 0x9E6665;
	
  // Activarea întreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Activarea întreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_IRQHandler(void) 
{
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) 
	{
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
			GPIOB_PSOR |= (1<<RED_LED_PIN);
			GPIOB_PSOR |= (1<<GREEN_LED_PIN);
			GPIOD_PSOR |= (1<<BLUE_LED_PIN);
		
		
		if(isOpen == 1)
		{
			if(state == 0) 
			{
				if(direction == 1)
					GPIOB_PCOR |= (1<<RED_LED_PIN);
				
				else
				{
					GPIOB_PCOR |= (1<<RED_LED_PIN);
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				}
				
				state=1;
			} 
				else if (state == 1) 
			{
				if(direction == 1)
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				
				else
				{
					GPIOD_PCOR |= (1<<BLUE_LED_PIN);
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				}
				
				state=2;
			} 
			else if (state == 2) 
			{
				if(direction == 1)
				{
					GPIOD_PCOR |= (1<<BLUE_LED_PIN);
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				}
				
				else 
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				
				state=3;
			}
			else if (state == 3) 
			{
				if(direction == 1)
				{
					GPIOB_PCOR |= (1<<RED_LED_PIN);
					GPIOB_PCOR |= (1<<GREEN_LED_PIN);
				}
				
				else
					GPIOB_PCOR |= (1<<RED_LED_PIN);
					
				state=0;
			}
		} 		
	}
}