#include "Uart.h"

int isOpen = 1;
int direction = 1;
int isBuzzer = 1;

void UART0_Transmit(uint8_t data)
{
		//Acest loop asteapta pana cand bitul TDRE (Transmitter Data Register Empty) din registrul UART0->S1 devine 1.
    //Acest lucru indica faptul ca bufferul de transmitere este gol si poate accepta un nou octet pentru transmitere.
		//(Reference manual pag 729)
	while(!(UART0->S1 & UART0_S1_TDRE_MASK));
				UART0->D = data;
}

void UART0_IRQHandler(void) 
{
    if (UART0->S1 & UART_S1_RDRF_MASK) 
    {
        char receivedData = UART0->D;

        if (receivedData == 'a')
        {
            if (isOpen == 0)
                isOpen = 1;
            else 
                isOpen = 0;
        }

        if (receivedData == 'b')
        {
            if (direction == 0)
                direction = 1;
            else 
                direction = 0;
        }

        if (receivedData == 'c')
        {
            if (isBuzzer == 0)
                isBuzzer = 1;
            else 
                isBuzzer = 0;
        }
    }
}

void UART0_Init(uint32_t baud_rate)
{	
	//Setarea sursei de ceas pentru modulul UART (pag 196 Refrence Manual 01 MCGFLLCLK clock or MCGPLLCLK/2 clock)
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	
	//Activarea semnalului de ceas pentru modulul UART (pag 204-205 Refrence Manual)
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	//Activarea semnalului de ceas pentru portul A
	//intrucat dorim sa folosim pinii PTA1, respectiv PTA2 pentru comunicarea UART (pag 206-207 Refrence Manual)
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	//Fiecare pin pune la dispozitie mai multe functionalitati 
	//la care avem acces prin intermediul multiplexarii 
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare RX pentru UART0 (pag 162 Reference Manual)
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare TX pentru UART0 (pag 162 Reference Manual)
	
	
	//Oprire transmit and recive (pag 728 Reference Manual)
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 
	
	//Configurare Baud Rate
	uint32_t osr = 4; // Over-Sampling Rate (numarul de esantioane luate per bit-time)
	
	//SBR - vom retine valoarea baud rate-ului calculat pe baza frecventei ceasului de sistem
	// 	 SBR  -		b16 b15 b14 [b13 b12 b11 b10 b09		b08 b07 b06 b05 b04 b03 b02 b01] &
	// 0x1F00 -		0		0   0    1   1   1   1   1      0   0   0   0   0   0   0   0
	//            0   0   0    b13 b12 b11 b10 b09    0   0   0   0   0   0   0   0 >> 8
	//   BDH  -   0   0   0    b13 b12 b11 b10 b09
	//   BDL  -   b08 b07 b06  b05 b04 b03 b02 b01
	
	//DEFAULT_SYSTEM_CLOCK is 48000000UL
		uint32_t sbr = (48000000UL / ((osr*4)*baud_rate));
	
	
	//(pag 725-726 Reference Manual)
	
	uint8_t temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));
	UART0->BDH = temp | UART0_BDH_SBR(((sbr & 0x1F00)>> 8));
	UART0->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
	
	//(pag 737 Reference Manual)
	
	UART0->C4 |= UART0_C4_OSR(osr-1);
	
	
	//Setare numarul de biti de date la 8 si fara bit de paritate (pag 726-727 Reference Manual)
	UART0->C1 = 0;
	
	//Dezactivare intreruperi la transmisie (pag 728 Reference Manual)
	UART0->C2 |= UART0_C2_TIE(0);
	UART0->C2 |= UART0_C2_TCIE(0);
	
	//Activare intreruperi la receptie (pag 728 Reference Manual)
	UART0->C2 |= UART0_C2_RIE(1);
	
	//Pornire transmit and recive (pag 728 Reference Manual)
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
	// Transmisie incepand cu LSB  (pag 731-732 Reference Manual)
	UART0->S2 = UART0_S2_MSBF(0);
	
	//Setare prioritate sa fie mai mica decat cea a pit-ului
	NVIC_SetPriority(UART0_IRQn, 2); 
	
	//Activeaza intreruperea asociata modulului UART0 (pentru recive de la interfata)
	NVIC_EnableIRQ(UART0_IRQn);
}