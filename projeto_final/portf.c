#include "tm4c123gh6pm.h"

// Pinos PF0 e PF4 são entradas e os pinos PF1 a PF3 são saídas
// Hailitado o pull-up dos pinos PF0 e PF4

void PortF_Init(void){
  volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;    // F clock
  delay = SYSCTL_RCGC2_R;          // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;  // unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;          // allow changes to PF4-0 
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PORTF
  GPIO_PORTF_PCTL_R = 0x00000000;  // configure PORTF as GPIO
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF0, PF4 input, PF1-PF3 output
  GPIO_PORTF_AFSEL_R &= ~0x1F;     // disable alt funct 
  GPIO_PORTF_PUR_R = 0x11;         // enable weak pull-up on PF4 and PF0
  GPIO_PORTF_DEN_R |= 0x1F;        // enable digital I/O
  GPIO_PORTF_DATA_R = 0x00;        // turn off LED
}
