//---------------------------------------------------------------
// Descrição dos pinos do PortC usados na placa MicroTB_UTFPR
// Amauri Amorin Assef
// 09/06/22
//---------------------------------------------------------------
//PortC: 0bxxxx_xxxx
//         ||||-----> Entrada do echo do sensor de ultrassom 
//         |||------> Saida PWM
//         ||-------> Saída 1 para sentido do motor usando o driver L293D
//         |--------> Saída 2 para sentido do motor usando o driver L293D
//---------------------------------------------------------------

#include "tm4c123gh6pm.h"

void PortC_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000004; 		// activate clock for port C
	SYSCTL_RCGCGPIO_R |= 0x04;        // activate port C
  delay=SYSCTL_RCGC2_R;           	// delay
  GPIO_PORTC_AMSEL_R &= ~0xF0;     	// disable analog functionality on PC
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0x0000FFFF);// configure PC4:PC7 as GPIO
  GPIO_PORTC_DIR_R |= 0xE0;    			// make PC5:PC7 out
	GPIO_PORTC_DIR_R &= ~0x10;    		// make PC4 in
  GPIO_PORTC_AFSEL_R &= ~0xF0; 			// disable alt funct on PC4:PC7
  GPIO_PORTC_DEN_R |= 0xF0;  	   		// enable digital I/O on PC4:PC7
	GPIO_PORTC_DATA_R &= ~0xE0;  	   	// PC5:PC7 = 0
}

