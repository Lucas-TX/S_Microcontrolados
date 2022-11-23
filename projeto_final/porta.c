#include "tm4c123gh6pm.h"

void PortA_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000001; 		// activate clock for port A
	SYSCTL_RCGCGPIO_R |= 0x01;            // activate port A
  delay=SYSCTL_RCGC2_R;           	// delay
  GPIO_PORTA_AMSEL_R = 0x00;      	// disable analog functionality on PA
  GPIO_PORTA_PCTL_R |= 0x0000000; 		// configure PA as GPIO
  GPIO_PORTA_DIR_R |= 0x60;     			// make PA out PA5 e PA6
  GPIO_PORTA_AFSEL_R |= 0x00;  			// disable alt funct on PA
  GPIO_PORTA_DEN_R |= 0x60;  	   		// enable digital I/O on PA
  GPIO_PORTA_DATA_R &= ~0x60;   			// PA = 0x00
}
