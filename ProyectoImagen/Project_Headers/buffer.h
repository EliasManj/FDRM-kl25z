/*
 * buffer.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Octavio Rojas
 */

#include "derivative.h"
#include "constants.h"

//#include "global_variables.h"

#ifndef BUFFER_H_

#define BUFFER_H_

//#define BUFLEN 	100


//Define Buffer aquí se define el buffer como tal 

struct Buffer 
{

	volatile uint8_t head;

	volatile uint8_t tail;

	volatile uint8_t size;

	volatile char data[BUFLEN];

};

typedef struct Buffer bufferType; //para no tener que escribir struct Buffer

//prototipo de todas las funciones que puede hacer el buffer

void buffer_push(bufferType *bf, char data); //empujar datos al buffer

char buffer_pop(bufferType *bf); //sacar datos del buffer

uint8_t buffer_inc(uint8_t pointer, uint8_t size); 

uint8_t buffer_isempty(bufferType *bf);

uint8_t buffer_len(bufferType *bf);

uint8_t buffer_isfull(bufferType *bf);

//////////////////////
//Funciones de texto//
//////////////////////

void uart_send_done(bufferType *bf);

void uart_send_ready(bufferType *bf);

void uart_send_notready(bufferType *bf);

void uart_send_startq(bufferType *bf);

void uart_send_sysverified(bufferType *bf);

void uart_send_sysalreadyon(bufferType *bf);

void uart_send_sysalreadyoff(bufferType *bf);

void uart_send_lightalreadyon(bufferType *bf);

void uart_send_lightalreadyoff(bufferType *bf);

void uart_send_sysisoff(bufferType *bf);

void uart_send_doorajar(bufferType *bf);

void uart_send_lightison(bufferType *bf);

void uart_send_stopscan(bufferType *bf);

void lcd_send_returning(bufferType *bf);

void send_Matlabisready(bufferType *tx_bf);

void dec2str(unsigned int number, unsigned char *data);


#endif /* BUFFER_H_ */
