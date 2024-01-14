#include "Uart.h"
#include "Pit.h"
#include "Gpio.h"
#include "Adc.h"
#include "Uart.h"
#include "ClockSettings.h"
#include "Buzzer.h"


extern uint32_t timer_value;
extern uint8_t led_state;
extern int isBuzzer;

int main() 
{
	
	UART0_Init(9600);
	OutputPIN_Init();
	PIT_Init();
	RGBLed_Init();
	ADC0_Init();
	init_buzzer();
	
	SystemClock_Configure();
	SystemClockTick_Configure();
	
	uint8_t prev_parte_zecimala = 0;
	uint8_t prev_parte_fractionara1 = 0;
	
	for(;;) {
		if(adcFlag==1)
		{
			
			float measured_voltage = (analogInput * 3.3f) / 255;
			uint8_t parte_zecimala = (uint8_t) measured_voltage;
			uint8_t parte_fractionara1 = ((uint8_t)(measured_voltage * 10)) % 10;
			uint8_t parte_fractionara2 = ((uint8_t)(measured_voltage * 100)) % 10;
			
			UART0_Transmit(parte_zecimala + 0x30);
			UART0_Transmit('.');
			UART0_Transmit(parte_fractionara1 + 0x30);
			UART0_Transmit(parte_fractionara2 + 0x30);
			UART0_Transmit(0x0A);
			UART0_Transmit(0x0D);
		
			if(isBuzzer == 1)
		 {
			if((prev_parte_zecimala==0 && prev_parte_fractionara1>=5) && (parte_zecimala==0 && parte_fractionara1<5))
			{    		
				play_buzzer(500,1000);
			}
			else if((prev_parte_zecimala==0 && prev_parte_fractionara1<5)&& ((parte_zecimala==0 && parte_fractionara1>=5)||(parte_zecimala>=1 && parte_zecimala<3)))
			{
					play_buzzer(500,3000);
					play_buzzer(2000,0);
					play_buzzer(500,3000);
			}
			else if(prev_parte_zecimala<3 && parte_zecimala>=3)
			{
					play_buzzer(500,5000);
					play_buzzer(2000,0);
					play_buzzer(500,5000);
				play_buzzer(2000,0);
					play_buzzer(500,5000);
			}
			else if(prev_parte_zecimala>=3 && parte_zecimala<3)
			{
					play_buzzer(500,3000);
					play_buzzer(2000,0);
					play_buzzer(500,3000);
			}
			
				prev_parte_zecimala = parte_zecimala;
				prev_parte_fractionara1 = parte_fractionara1;
			}
		}
	}
}