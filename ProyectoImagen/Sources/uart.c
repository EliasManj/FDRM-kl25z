/*
 * uart.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#include "uart.h"
#include "buffer.h"
#include "constants.h"
#include "global_variables.h"
#include "command.h"

//Tengo que inicializar dos puertos UART
//PTE0 ALT4 TX UART2
//PTE1 ALT4 RX UART2

void UART_init(void)
{	
	//Inicialización del módulo UART0 y UART1
	
	SIM_SCGC4 |= (1 << 10);		//CLK UART0
	
	SIM_SCGC4 |= (1 << 11);		//CLK UART1

	//CLOCK for PORTA already activated

	SIM_SOPT2 |= (1 << 26);		//Enable UART0 clock with MCGFLLCLK clock or MCGPLLCLK/2 clock
	
	//Falta reloj de UART1
	MCG_C1 |= (1<<6);
	
	PORTA_PCR1 = (1<<9);		//Port control for UART_0

	PORTA_PCR2 = (1<<9);		//Port control for UART_0
	
	PORTE_PCR0 =0x0300;			//Port control for UART_1
	
	PORTE_PCR1 =0x0300;			//Port control for UART_1

	UART0_BDL = 137;			//clock=640*32768, baud rate 9600
	
	UART1_BDL = 137;

	UART0_C2 |= (1 << 5);		//reciver interrupt enable for RDRF

	UART0_C2 |= (1 << 2);		//RE reciver enable

	UART0_C2 |= (1 << 3);		//TE Transmiter enable

	UART1_C2 |= (1 << 5);		//reciver interrupt enable for RDRF
	
	//UART1_C2 |= (1 << 7);		//

	UART1_C2 |= (1 << 2);		//RE reciver enable

	UART1_C2 |= (1 << 3);		//TE Transmiter enable
	
	
	NVIC_ISER |= (1 << 12);     

	NVIC_ICPR |= (1 << 12);
	
	
	NVIC_ISER |= (1 << 13);     

	NVIC_ICPR |= (1 << 13);
}


