#include "tm4c123gh6pm.h"

void PortB_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000002; 		// activate clock for port B
  delay=SYSCTL_RCGC2_R;           	// delay
  GPIO_PORTB_AMSEL_R = 0x00;      	// disable analog functionality on PB
  GPIO_PORTB_PCTL_R = 0x0000000; 		// configure PB as GPIO
  GPIO_PORTB_DIR_R = 0xFF;     			// make PB out
  GPIO_PORTB_AFSEL_R = 0x00;  			// disable alt funct on PB
  GPIO_PORTB_DEN_R = 0xFF;  	   		// enable digital I/O on PB
  GPIO_PORTB_DATA_R = 0x00;   			// PB = 0x00
}
