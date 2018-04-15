/*
 * buffer.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Elias g
 */
#include "buffer.h"

void buffer_push(bufferType *bf, char data) {
	bf->data[bf->tail] = data;
	bf->tail = buffer_inc(bf->tail, bf->size);
}

char buffer_pop(bufferType *bf) {
	uint8_t item = bf->data[bf->head];
	bf->head = buffer_inc(bf->head, bf->size);
	return item;
}

uint8_t buffer_inc(uint8_t pointer, uint8_t size) {
	uint8_t i = pointer;
	if (++i >= size) {
		i = 0;
	}
	return i;
}

uint8_t buffer_isempty(bufferType *bf) {
	return bf->head == bf->tail;
}

uint8_t buffer_isfull(bufferType *bf) {
	return buffer_len(bf) == (bf->size - 1);
}

uint8_t buffer_len(bufferType *bf) {
	uint8_t len = bf->tail - bf->head;
	if (len < 0) {
		len += bf->size;
	}
	return len;
}

void uart_send_done(bufferType *bf) {
	buffer_push(bf, 'D');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, 'E');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void dec2str(unsigned int n, unsigned char data[4]) {
	data[3] = n / 1000 + 0x30;
	n = n % 1000;
	data[2] = n / 100 + 0x30;
	n = n % 100;
	data[1] = n / 10 + 0x30;
	n = n % 10;
	data[0] = n + 0x30;
}
