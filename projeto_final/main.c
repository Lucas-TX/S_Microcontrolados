// main.c
// Runs on LM4F120/TM4C123
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 23, 2014

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
 
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 
 Arquivo base para o projeto final da disciplina de Sistemas Microcontrolado
 Professor Amauri Amorin Assef
 
 */
/*---- Especificação ----
Data: 23/11/2022
Autor: Lucas Teixeira & Patrick Zilz
----- */


#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "ADCSWTrigger.h"
#include "portb.h"
#include "portf.h"
#include "portc.h"
#include "UART.h"
#include "porta.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Timer2.h"
#include "tm4c123gh6pm.h"
#include "systick.h"

void Output_Init(void);

#define PERIOD 16000000/100		// 100 Hz @ 16 MHz PLL

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

uint8_t dezena = 0, unidade = 0; // dados para os displays
uint8_t display=0;	             // seleção dos displays
const unsigned char tabela_display[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // Caracteres para mostrar no display
uint8_t i=10; // index usado no vetor tabela_display
uint8_t liga_PWM = 0; // Flag de identificação do PWM

void Output_Init(void);

void Timer0A_Handler(void){									// a cada 10ms cai nessa interrupção 100Hz
	 
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;			// acknowledge TIMER0A timeout
	// Verifica se o PWM deve ser ligado
	if(liga_PWM == 1){
		 	GPIO_PORTC_DATA_R |= 0x20;						  // Liga PWM
	}
	
	TIMER1_CTL_R = 0x00000001;    					// enable TIMER1A
	
	dezena = i%10;
	unidade = i/10;
	
	if(display ==0){
		    display = 1;
		    GPIO_PORTA_DATA_R = 0x20;// habilita o pino PA5 e desabilita o PA6
		    GPIO_PORTB_DATA_R = tabela_display[dezena]; //envia o valor da dezena do estado para o display
	} else{
		    display = 0;
		    GPIO_PORTA_DATA_R = 0x40; // habilita o pino PA6 e desabilita o PA5
		   //envia o valor da unidade do estado para o display
		   GPIO_PORTB_DATA_R = tabela_display[unidade];
	}
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;		// acknowledge TIMER1A timeout
	GPIO_PORTC_DATA_R &= ~0x20;						// LED off
	TIMER1_CTL_R = 0x00000000;    				// desable TIMER1A
	
	// Configuração da razão de trabalho. 4 possibilidades com chaves
	//GPIO_PORTF_DATA_R = 0x0E;
	if (GPIO_PORTF_DATA_R == 0x00){
		TIMER1_TAILR_R = PERIOD * 0.25;/// de 25%
	}else if (GPIO_PORTF_DATA_R == 0x01){
		TIMER1_TAILR_R = PERIOD * 0.50;/// de 50%
	}else if(GPIO_PORTF_DATA_R == 0x10){
		TIMER1_TAILR_R = PERIOD * 0.75;/// de 75%
	}else if(GPIO_PORTF_DATA_R == 0x11){
		TIMER1_TAILR_R = PERIOD * 0.90;/// de 90%
	}	
}

void Timer2A_Handler(void){
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;		// acknowledge TIMER2A timeout
	TIMER2_CTL_R = 0x00000000;    				// desable TIMER2A
	liga_PWM = 0;                         //  desliga PWM
	
	
}



int main(void){ 
	int32_t data;
	int32_t contadorLiberacaoAlcool = 0;
	int32_t tempoMaximo = 16000000;
	char UART_data;
  uint8_t estado = 1;   // variável da maáquina de estado
	Output_Init();              // initialize output device
	PortA_Init();
	PortB_Init();
	PortC_Init();
  PortF_Init();
	SysTick_Init(); // inicialização do Systick timer
	// Canal 9 para simulação e canal 1 para teste na placa
	ADC0_InitSWTriggerSeq3(9);					// Sequencer 3, canal 9 (PE4)
	Timer0_Init(PERIOD); 							 	// initialize timer0 (100 Hz)
	Timer1_One_Shot_Init(PERIOD/2); 		// initialize timer1 (50%)
	EnableInterrupts();
	
	printf("\n=====================");
	printf("\nEquipe = Lucas Teixeira & Patrick Zilz");
	printf("\n=====================");
		
	while(1){
		switch(estado){
			case 1:
				GPIO_PORTF_DATA_R = 0x0E; // acende todos os LED's
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				GPIO_PORTF_DATA_R = 0x0; // desliga todos os LED's
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				printf("\nProjeto Final de Sistemas MicroControlados");
				printf("\nLucas Teixeira & Patrick Zilz");
				printf("\nSistema em espera");
				estado =2; // muda para o estado 2
				break;
			case 2:
				GPIO_PORTF_DATA_R = 0x04; // acende o LED azul
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				GPIO_PORTF_DATA_R = 0x0; // desliga todos os LED's
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				estado = 3; // muda para o estado 3
				break;
			case 3:
				printf("\nSistema em operacao");
				estado = 4; // muda para o estado 4
				break;
			
			case 4:
				GPIO_PORTF_DATA_R = 0x02; // acende o LED vermelho
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				GPIO_PORTF_DATA_R = 0x0; // desliga todos os LED's
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				estado = 4; // muda para o estado 4
				// Leitura da porta Serial
				UART_data = UART_InChar1();
				if (UART_data == 'a'){
					//testar limite 1.4 e zerar pwm 
					tempoMaximo += 16000000*0.1;
					printf("\nADC Aumentar o limite de tempo: %d ms", tempoMaximo/16000);
					
					//Timer1_One_Shot_Init(tempoMaximo+PERIOD); 		// initialize timer com + 100 Hz
				}else if (UART_data == 'd'){
					tempoMaximo -= 16000000*0.1;
					printf("\nADC Diminuir o limite de tempo: %d ms", tempoMaximo/16000);
					
					//Timer1_One_Shot_Init(tempoMaximo-PERIOD); 		// initialize timer com - 100 Hz
				}else if (UART_data == 'r'){
					tempoMaximo = 16000000;
					printf("\nADC Resear o tempo limite %d ms", tempoMaximo/16000);
					//Timer1_One_Shot_Init(PERIOD/2); 		// initialize timer1 (50%)
				}
				estado = 5; // muda para o estado 5
				break;
				
			case 5:
				//Leitura do ADC
				data = ADC0_InSeq3();
				if(data > 2047){ // Testa se a tensão > 1,65V
					Timer2_Init(tempoMaximo);
					liga_PWM = 1;
					estado = 6; // muda para o para o estado 6
				}else{
					liga_PWM = 0;
					estado = 4; // volta para o para o estado 4. Leitura do sensor
				}
				break;
				
			case 6:
				GPIO_PORTF_DATA_R = 0x0E; // acende todos os LED's
			  //Timer1_Init(tempoMaximo); 							 	// initialize timer1 (tempoMaximo definido no estado 4)
				estado = 7;               // Vai para o para o estado 7. Contador de acionamento			
				//Ligar o temporizador no tempo limite
				//Ligar o temporizador
				//Se chegar o tempo limite, desligar o PWM
				
				break;
			case 7:
				contadorLiberacaoAlcool++;
				printf("\nLiberacao do alcool em gel habilitada");
				printf("\nNumero de acionamentos com liberacao de alcool em gel = %d",contadorLiberacaoAlcool);
				estado = 8;               // Vai para o para o estado 8. 
				
				
				break;
			case 8:				
					GPIO_PORTF_DATA_R = 0x0E; // desliga todos os LED's		
					SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
					GPIO_PORTF_DATA_R = 0x0; // acende todos os LED's	
					SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
					//Leitura do ADC
					data = ADC0_InSeq3();
					if(data > 2047){ // Testa se a tensão > 1,65V
						//Verifica tempo limite no timer2
						if(liga_PWM == 0){
							estado = 9; // vai para o para o estado 9. Desliga PWM e temporizador
						}else{
						// PWM ligado
							estado = 8; // vai para o para o estado 8. Repete verificação
						}						
					}else{
						//não está detectando presença
						estado = 11; // vai para o para o estado 11. Desligar o temporizador e o PWM
					}
			break;
					
			case 9:
				liga_PWM = 0;
			  TIMER2_CTL_R = 0x00000000;
			  //Timer2_Init(0);
				printf("\nLiberacao do alcool em gel desabilitada devido ao tempo limite");
				estado = 10;
			break;
			
			case 10:
				
				GPIO_PORTF_DATA_R = 0x08; // acende o LED verde
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
				GPIO_PORTF_DATA_R = 0x0; // desliga todos os LED's
				SysTick_Wait10ms(10); // 10 x Atraso de 100 ms
			  data = ADC0_InSeq3();
				if(data > 2047){ // Testa se a tensão > 1,65V
					estado = 10; // Volta para o estado 10. Repete verificação
				}else{
					estado = 11; // Volta para o estado 11. Desliga sistema
				}
			break;
			
			case 11:
				liga_PWM = 0;
			  TIMER2_CTL_R = 0x00000000;
				printf("\nLiberacao do alcool em gel desabilitada");
				estado = 4;
			break;
			
			default:
				
				break;
		}
		
		
		
		
		
		
		
    
		
  }
}
