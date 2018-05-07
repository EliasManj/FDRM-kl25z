/*
 * lcd.h
 *
 *  Created on: Apr 27, 2018
 *      Author: Octavio Rojas
 */

#include "derivative.h"
#include "constants.h"
#include "buffer.h"


#ifndef LCD_H_
#define LCD_H_

/*
 * Quiero poder enviar a la pantalla LCD los mensajes:
 * Encendido
 * Verificando
 * Verificado
 * Apagado
 * Milímetro actual:
 * Grado actual:
 * Terminado
 * Puerta abierta
 * Luz prendida
 * */

struct lcdState 
{

	unsigned char RS[2];

	unsigned int EN;

	unsigned int nextState[2];

};

typedef const struct lcdState LcdEncoding;

unsigned int current_lcd_state;

unsigned char lcd_en;

unsigned char lcd_rs;

unsigned char lcd_db7;

unsigned char lcd_db6;

unsigned char lcd_db5;

unsigned char lcd_db4;

unsigned char lcd_bf_empty;

char lcd_data;

void gpio_lcd_ports_init(void);

void update_lcd_fsm(bufferType *lcd_bf);


void lcd_initialize(bufferType *bf);
void write_to_lcd(bufferType *bf);
void refresh_lcd_50ms(void);
void LCD_send(unsigned char dato, unsigned char RS, unsigned char size);
void lcd_send_adc_data(bufferType *lcd_bf, unsigned int ADC_reading, unsigned char *adc_string);


#endif /* LCD_H_ */
