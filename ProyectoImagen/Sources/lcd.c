/*
 * lcd.c
 *
 *  Created on: Apr 27, 2018
 *      Author: Octavio Rojas
 */

#include "buffer.h"
#include "derivative.h"
#include "constants.h"
#include "lcd.h" 
#include "instructions.h"

unsigned int current_lcd_state 	= IDLE;

unsigned char lcd_en 			= 0;

unsigned char lcd_rs 			= 0;

unsigned char lcd_db7 			= 0;

unsigned char lcd_db6 			= 0;

unsigned char lcd_db5 			= 0;

unsigned char lcd_db4 			= 0;

unsigned char lcd_bf_empty 		= 1;




char lcd_data;



LcdEncoding lcdEncoding[5] = 
{ 
		{ { 0, 1 }, 0, { SET_RS, IDLE } },						//IDLE

		{ { 0, 1 }, 0, { DATA_ENABLE_SET, DATA_ENABLE_SET } },	//SET_RS

		{ { 0, 1 }, 1, { ENABLE_CLEAR, ENABLE_CLEAR } },		//DATA_ENABLE_SET

		{ { 0, 1 }, 0, { DATA_CLEAR, DATA_CLEAR } },			//ENABLE_CLEAR 

		{ { 0, 1 }, 0, { IDLE, IDLE } },						//DATA_CLEAR

};

void gpio_lcd_ports_init(void) {

	SIM_SCGC5 |= (1 << 9);

	SIM_SCGC5 |= (1 << 11);

	PORTA_PCR17 =(1<<8); 		//PTA17 GPIO = EN

	PORTA_PCR16 =(1<<8); 		//PTA16 GPIO = RS

	PORTC_PCR12 =(1<<8); 		//PTC12 GPIO = DB7

	PORTC_PCR13 =(1<<8); 		//PTC13 GPIO = DB6

	PORTC_PCR16 =(1<<8); 		//PTC16 GPIO  = DB5

	PORTC_PCR17 =(1<<8); 		//PTC17 GPIO  = DB4

	GPIOA_PDDR |= (1 << 17);		//Set GPIOA17 as output

	GPIOA_PDDR |= (1 << 16);		//Set GPIOA16 as output

	GPIOC_PDDR |= 0x00033000;	//Set GPIOC as output

}


void write_to_lcd(bufferType *bf) {

	lcd_db7 = (lcd_data & 0x08) >> 3;

	lcd_db6 = (lcd_data & 0x04) >> 2;

	lcd_db5 = (lcd_data & 0x02) >> 1;

	lcd_db4 = (lcd_data & 0x01) >> 0;

	GPIOC_PDOR ^= ((-(lcd_db7)) ^ GPIOC_PDOR ) & (1UL << 12); //PTC12 GPIO = DB7

	GPIOC_PDOR ^= ((-(lcd_db6)) ^ GPIOC_PDOR ) & (1UL << 13); //PTC13 GPIO = DB6

	GPIOC_PDOR ^= ((-(lcd_db5)) ^ GPIOC_PDOR ) & (1UL << 16); //PTC16 GPIO  = DB5

	GPIOC_PDOR ^= ((-(lcd_db4)) ^ GPIOC_PDOR ) & (1UL << 17); //PTC17 GPIO  = DB4

}



void lcd_initialize(bufferType *bf) {

	buffer_push(bf, 0x03);

	buffer_push(bf, 0x03);

	buffer_push(bf, 0x03);

	buffer_push(bf, 0x02);

	buffer_push(bf, 0x02);

	buffer_push(bf, 0x08);

	buffer_push(bf, 0x00);

	buffer_push(bf, 0x0E);

	buffer_push(bf, 0x00);

	buffer_push(bf, 0x06);

	buffer_push(bf, 0x00);

	buffer_push(bf, 0x01);

	//Characteres

	buffer_push(bf, 0x14);//B
	buffer_push(bf, 0x12);
	
	buffer_push(bf, 0x14);//I
	buffer_push(bf, 0x19);
	
	buffer_push(bf, 0x14);//E
	buffer_push(bf, 0x15);
	
	buffer_push(bf, 0x14);//N
	buffer_push(bf, 0x1E);
	
	buffer_push(bf, 0x15);//V
	buffer_push(bf, 0x16);
	
	buffer_push(bf, 0x14);//E
	buffer_push(bf, 0x15);
	
	buffer_push(bf, 0x14);//N
	buffer_push(bf, 0x1E);
	
	buffer_push(bf, 0x14);//I
	buffer_push(bf, 0x19);
	
	buffer_push(bf, 0x14);//D
	buffer_push(bf, 0x14);

	buffer_push(bf, 0x14);//O
	buffer_push(bf, 0x1F);

}

void lcd_send_adc_data(bufferType *lcd_bf, unsigned int ADC_reading, unsigned char *adc_string){
	buffer_push(lcd_bf, 0x00);
	buffer_push(lcd_bf, 0x01);
	buffer_push(lcd_bf, 0x00);
	buffer_push(lcd_bf, 0x02);
	dec2str(ADC_reading, adc_string);
	buffer_push(lcd_bf, 0x10 | ((adc_string[2]&0xF0)>>4));
	buffer_push(lcd_bf, 0x10 | ((adc_string[2]&0x0F)));
	buffer_push(lcd_bf, 0x10 | ((adc_string[1]&0xF0)>>4));
	buffer_push(lcd_bf, 0x10 | ((adc_string[1]&0x0F)));
	buffer_push(lcd_bf, 0x10 | ((adc_string[0]&0xF0)>>4));
	buffer_push(lcd_bf, 0x10 | ((adc_string[0]&0x0F)));
}

void update_lcd_fsm(bufferType *lcd_bf) {

	lcd_en = lcdEncoding[current_lcd_state].EN;

	GPIOA_PDOR ^= (-(lcd_en) ^ GPIOA_PDOR ) & (1UL << 17); //SET EN

if (!buffer_isempty(lcd_bf) && current_lcd_state != 0) {

		if (current_lcd_state == SET_RS) {

			lcd_data = buffer_pop(lcd_bf);

			lcd_rs = lcdEncoding[current_lcd_state].RS[(lcd_data & 0x10) >> 4];

			GPIOA_PDOR ^= (-(lcd_rs) ^ GPIOA_PDOR ) & (1UL << 16); //SET RS

		}

	}

	if (current_lcd_state == DATA_ENABLE_SET) {

		write_to_lcd(lcd_bf);

	}

	current_lcd_state = lcdEncoding[current_lcd_state].nextState[buffer_isempty(

			lcd_bf)];

}



/*void lcd_initialize(bufferType *bf)
{
	//Secuencia para inicializar la pantalla
	
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x03);
	buffer_push(bf, 0x02);
	buffer_push(bf, 0x02);
	buffer_push(bf, 0x08);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x0E);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x06);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x01);
	
	//Imprimir "Bienvenido"
	
	buffer_push(bf, 0x14);//B
	buffer_push(bf, 0x12);
	
	buffer_push(bf, 0x14);//I
	buffer_push(bf, 0x19);
	
	buffer_push(bf, 0x14);//E
	buffer_push(bf, 0x15);
	
	buffer_push(bf, 0x14);//N
	buffer_push(bf, 0x1E);
	
	buffer_push(bf, 0x15);//V
	buffer_push(bf, 0x16);
	
	buffer_push(bf, 0x14);//E
	buffer_push(bf, 0x15);
	
	buffer_push(bf, 0x14);//N
	buffer_push(bf, 0x1E);
	
	buffer_push(bf, 0x14);//I
	buffer_push(bf, 0x19);
	
	buffer_push(bf, 0x14);//D
	buffer_push(bf, 0x14);

	buffer_push(bf, 0x14);//O
	buffer_push(bf, 0x1F);
	
}

void write_to_lcd(bufferType *bf) 
{

	lcd_db7 = (lcd_data & 0x08) >> 3;

	lcd_db6 = (lcd_data & 0x04) >> 2;

	lcd_db5 = (lcd_data & 0x02) >> 1;

	lcd_db4 = (lcd_data & 0x01) >> 0;

	GPIOC_PDOR ^= ((-(lcd_db7)) ^ GPIOC_PDOR ) & (1UL << 6); //PTC6 GPIO = DB7

	GPIOC_PDOR ^= ((-(lcd_db6)) ^ GPIOC_PDOR ) & (1UL << 5); //PTC5 GPIO = DB6

	GPIOC_PDOR ^= ((-(lcd_db5)) ^ GPIOC_PDOR ) & (1UL << 4); //PTC4 GPIO  = DB5

	GPIOC_PDOR ^= ((-(lcd_db4)) ^ GPIOC_PDOR ) & (1UL << 3); //PTC3 GPIO  = DB4

}

void refresh_lcd_50ms(void) //esta función se pone en la función que se ejecuta cada 50ms
{

	lcd_en = lcdEncoding[current_lcd_state].EN;

	GPIOC_PDOR ^= (-(lcd_en) ^ GPIOC_PDOR ) & (1UL << 0); //SET EN

	if (!buffer_isempty(lcd_bf) && current_lcd_state != 0) 
	{

		if (current_lcd_state == SET_RS) 
		{

			lcd_data = buffer_pop(lcd_bf);
			

			lcd_rs = lcdEncoding[current_lcd_state].RS[(lcd_data & 0x10) >> 4];

			GPIOC_PDOR ^= (-(lcd_rs) ^ GPIOC_PDOR ) & (1UL << 7); //SET RS

		}

	}

	if (current_lcd_state == DATA_ENABLE_SET)
	{

		write_to_lcd(lcd_bf);

	}

	current_lcd_state = lcdEncoding[current_lcd_state].nextState[buffer_isempty(lcd_bf)];

}

void LCD_send(unsigned char dato, unsigned char RS, unsigned char size) 
{

	if(RS)
	{ 
		GPIOE_PSOR|=1<<3; //RS
	}
	
	else 
	{
		GPIOE_PCOR|=1<<3;
	}

	

	GPIOE_PCOR|=1<<2;	//E en cero como inicial

	delay(1);

	GPIOE_PSOR|=1<<2;	//E en uno para enviar comando/dato

	delay(1);

		if(dato&0x80) GPIOB_PSOR|=1<<8;

		else GPIOB_PCOR|=1<<8;

		if(dato&0x40) GPIOB_PSOR|=1<<9;

		else GPIOB_PCOR|=1<<9;

		if(dato&0x20) GPIOB_PSOR|=1<<10;

		else GPIOB_PCOR|=1<<10;

		if(dato&0x10) GPIOB_PSOR|=1<<11;

		else GPIOB_PCOR|=1<<11;

	delay(1);

	GPIOE_PCOR|=1<<2;	//E en cero para terminar de enviar comando/dato

	

	if(size==8){

		dato<<=4;			//Shift left dato cuatro a la izquierda

		GPIOE_PSOR|=1<<2;	//E en uno para enviar comando/dato

			delay(1);

				if(dato&0x80) GPIOB_PSOR|=1<<8;

				else GPIOB_PCOR|=1<<8;

				if(dato&0x40) GPIOB_PSOR|=1<<9;

				else GPIOB_PCOR|=1<<9;

				if(dato&0x20) GPIOB_PSOR|=1<<10;

				else GPIOB_PCOR|=1<<10;

				if(dato&0x10) GPIOB_PSOR|=1<<11;

				else GPIOB_PCOR|=1<<11;

			delay(1);

			GPIOE_PCOR|=1<<2;	//E en cero para terminar de enviar comando/dato

	}
}
*/
