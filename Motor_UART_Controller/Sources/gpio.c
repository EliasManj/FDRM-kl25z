/*
 * gpio.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */
#include "gpio.h"

void Ports_init(void) {
	SIM_SCGC5 |= (1 << 10);
	PORTB_PCR0 =(1<<8); 		//PTB0 GPIO
	PORTB_PCR1 =(1<<8); 		//PTB1 GPIO
	PORTB_PCR2 =(1<<8); 		//PTB2 GPIO
	PORTB_PCR3 =(1<<8); 		//PTB3 GPIO
	GPIOB_PDDR |= 0x0000000F;	//Set GPIOB as output
}

void shift_rgb_leds(void) {
	GPIOB_PDOR ^= (-LEDs[0] ^ GPIOB_PDOR ) & (1UL << 18);
	GPIOB_PDOR ^= (-LEDs[1] ^ GPIOB_PDOR ) & (1UL << 19);
	GPIOD_PDOR ^= (-LEDs[2] ^ GPIOD_PDOR ) & (1UL << 1);
	led_temp = LEDs[0];
	LEDs[0] = LEDs[1];
	LEDs[1] = LEDs[2];
	LEDs[2] = led_temp;
}

void Set_timer_signal_GPIO(void) {
	//Set PTC3 as a Test Point
	SIM_SCGC5 |= (1 << 11);		//Activate clock for port C
	PORTC_PCR3 = (1<<8);		//PTC3 set GPIO
	GPIOC_PDDR |= (1 << 3);		//PTC3 set output
	GPIOC_PDOR |= (1 << 3);		//Set PTC3 in LOW initially (negated logic)
}

void toggle_signal(void) {
	GPIOC_PTOR = (1 << 3);
}

void RGB_init(void) {
	//Activate SIM_SCGC5 (system control gating register 5) for port B and D
	SIM_SCGC5 |= (1 << 10);		//Port B
	SIM_SCGC5 |= (1 << 12);		//Port D
	PORTB_PCR18 = (1<<8);		//Set PTB18 as GPIO
	PORTB_PCR19 = (1<<8);		//set PTB19 as GPIO
	PORTD_PCR1 = (1<<8);		//Set PTD1 as GPIO
	GPIOB_PDDR = (1 << 18);		//Set PTB18 as output
	GPIOB_PDDR |= (1 << 19);	//set PTB19 as output
	GPIOD_PDDR = (1 << 1);		//Set PTD1 as output
}
