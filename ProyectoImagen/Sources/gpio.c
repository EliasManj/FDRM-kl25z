/*
 * gpio.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "gpio.h"

//Inicialización de todos los pines que se necesitan 
//16 pines GPIO, 2 entradas analógicas y RX-TX.
//8 pines para los drivers (4 con drivers)
//4 para los sensores de posición
//2 para encender los láser
//1 para un boton de alto total
//1 para las luces internas
//2 ADC para los detectores
//RX PTA1 UART0RX ALT2, TX PTA2 UART0TX ALT2 se configuran junto con el módulo UART. 

void Pin_init(void)
{
	//Reloj a puertos: A,B,C,D,E
	SIM_SCGC5|=0x00003F00;
	
	//PTB 0 Y 1 motor 1 operando con drivers
	//Utilizando drivers sólo requerimos de dos pines.
	//DEGREE:
	PORTB_PCR0 =(1<<8); 		//PTB0 GPIO CW+

	PORTB_PCR1 =(1<<8); 		//PTB1 GPIO CLK+
		
	//PTB 8 Y 9 motor 2 operando con drivers
	//Utilizando drivers sólo requerimos de dos pines.
	PORTB_PCR8 =(1<<8);			//PTB8 GPIO CW+
	
	PORTB_PCR9 =(1<<8);			//PTB9 GPIO CLK+
	
	//Los siguientes pines siguientes serán para el LCD

	PORTC_PCR6 =(1<<8); 		//PTC6 GPIO =DB7

	PORTC_PCR5 =(1<<8); 		//PTC5 GPIO =DB6
	
	PORTC_PCR4 =(1<<8); 		//PTC4 GPIO =DB5

	PORTC_PCR3 =(1<<8); 		//PTC3 GPIO =DB4
	
	PORTC_PCR0 =(1<<8); 		//PTC0 GPIO =EN
	
	PORTC_PCR7 =(1<<8); 		//PTC7 GPIO =RS
		
	//PTE 20 21 22 Y 23 sensores de posición de motor 1 y 2
	//Son entradas por default
	PORTE_PCR20 =(1<<8);		//PTE20 GPIO posición de inicio rotación
		
	PORTE_PCR21 =(1<<8);		//PTB21 GPIO posición de fin rotación

	PORTE_PCR22 =(1<<8); 		//PTB22 GPIO posición de inicio traslación

	PORTE_PCR23 =(1<<8); 		//PTB23 GPIO posición de fin traslación
		
	//PTE 29 Y 30 salidas para activar los lásers. Hay dos disponibles en caso de usar dos emisores.
	
	PORTE_PCR29 = (1<<8); 		//PTE29 GPIO láser 1
	
	PORTE_PCR30 = (1<<8);		//PTE30 GPIO láser 2
	
	//Transistor de encendido y luces internas
	
	PORTD_PCR7 = (1<<8); 		//PTD07 GPIO Transistor de encendido y apagado
	
	PORTD_PCR6 = (1<<8);		//PTD06 GPIO Luces
	
	//Boton para mandar datos
	PORTA_PCR13 = (1<<8);		//PTA13 manda los datos a Matlab

	GPIOB_PDDR |= 0x00000F0F;	//Set GPIO pins as outputs
	GPIOD_PDDR |= 0x00000040;
	GPIOE_PDDR |= 0x60000000;
	GPIOC_PDDR |= 0x000000F9;   //GPIO LCD
	
	
	
	//Entradas del ADC0 PTC2 y PTC1. Hay dos disponibles en caso de usar dos receptores
	// ADC0_SE15 C1 default ADCH=01111
	// ADC0_SE11 C2 default ADCH=01011
	
	PORTC_PCR1 = (0<<8);  //PTC1
	
	PORTC_PCR2 = (0<<8);  //PTC2
}

