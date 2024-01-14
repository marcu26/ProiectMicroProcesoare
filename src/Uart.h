#include "MKL25Z4.h"

void UART0_Transmit(uint8_t data);
void UART0_Init(uint32_t baud_rate); 
void UART0_IRQHandler(void);