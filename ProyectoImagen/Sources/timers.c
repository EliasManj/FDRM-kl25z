/*
 * timers.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Octavio Rojas
 */
#include "derivative.h"
#include "timers.h"
#include "global_variables.h"
#include "lcd.h"
#include "flags.h"

void LPTMR_init(void) {
	//Inicialización del LPTMR (Low Power Timer)

	SIM_SCGC5 |= (1 << 0); 		//Señal de reloj al módulo

	LPTMR0_PSR = 0b00000101; //Bypass del preescaler  y selección del oscilador de 1khz como fuente para el timer

	LPTMR0_CMR = 500;	//Escribe 500 en el registro de comparación = .5 secs

	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector

	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt

	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	

}

void TPM_init(void) {
	//Inicialización del timer con dual. Es un timer con 6 canales o varios timers con 6 canales?
	//Hay TPM0 a TPM2
	//Setting MCGIRCLK

	SIM_SCGC6 |= (1 << 24);		//CLK TMP0

	MCG_C1 |= (1 << 1);			//IRCLKEN

	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz

	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz

	SIM_SOPT2 |= (3 << 24);		//TPM clock source select MCGIRCLK clock

	TPM0_SC |= (1 << 6);		//TOIE enable interrupt

	TPM0_SC |= (1 << 8);		//DMA enable overflow

	PORTC_PCR3 = (1<<10);		//TMP0_CH2 usando el channel 2

	TPM0_C2SC = (5<<2);		//Output compare -> toggle mode FOR TMP0 CH2, lo que
	//entiendo es que el el pin PTC3 se va a ver la salida del clock con dual

	//TPM0_C2V =0x0000C350;		//50,000 -> 50000 clock cycles of 1MHz -> 50ms
	TPM0_MOD = 0x00002710;

	NVIC_ICPR |= (1 << 17);		//

	NVIC_ISER |= (1 << 17);		//

	TPM0_SC |= (1 << 3);		//CMOD select clock mode mux

}

void tmp_counter_50ms_tick(bufferType *lcd_bf) //Lo que hay en esta función ocurre cada 50ms
{
	tmp_counter_50ms++;

	if (tmp_counter_50ms >= 2) {
		tmp_counter_50ms = 0;
		
		if (returning_f == 0) {
			lcd_send_adc_data(lcd_bf, ADC_reading, adc_string);	
		}
		update_lcd_fsm(lcd_bf);
	}

}

void tmp_counter_1sec_tick() //Lo que hay en esta función ocurre cada segundo
{

	tmp_counter_1sec++;

	if (tmp_counter_1sec >= 5) {

		tmp_counter_1sec = 0;

		tmp_counter_5sec_tick();

	}

}

void tmp_counter_5sec_tick() //Lo que hay en esta función sucede cada 5 segundos
{

	/*if (ADC_reading >= temperature_limit)
	 {

	 uart_send_overtemperature_detected(tx_bf);

	 }
	 
	 else 
	 {

	 uart_send_temperature(tx_bf);

	 }*/

}
