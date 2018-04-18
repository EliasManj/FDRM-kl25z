/*
 * buffer.h
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */

#include "derivative.h"
#include "global_variables.h"
#ifndef BUFFER_H_
#define BUFFER_H_

//Define Buffer
struct Buffer {
	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t size;
	volatile char data[BUFLEN];
};

typedef struct Buffer bufferType;

void buffer_push(bufferType *bf, char data);
char buffer_pop(bufferType *bf);
uint8_t buffer_inc(uint8_t pointer, uint8_t size);
uint8_t buffer_isempty(bufferType *bf);
uint8_t buffer_len(bufferType *bf);
uint8_t buffer_size(bufferType *bf);
uint8_t buffer_isfull(bufferType *bf);
void uart_send_done(bufferType *bf);
void uart_send_temperature(bufferType *bf);
void uart_send_overtemperature_detected(bufferType *bf);
void dec2str4(unsigned int number, unsigned char data[4]);
void dec2str3(unsigned int n, unsigned char data[3]);
#endif /* BUFFER_H_ */
