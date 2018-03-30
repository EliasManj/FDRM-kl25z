/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

#define BUFLEN 20
int rx_status;
char command[BUFLEN];
char val;

struct Buffer {
	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t size;
	volatile char data[BUFLEN];
};

typedef struct Buffer bufferType;
bufferType Buffer_rx = { 0, 0, 10, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
bufferType *rx_bf;

void buffer_push(bufferType *bf, char data);
char buffer_pop(bufferType *bf);
uint8_t buffer_inc(uint8_t pointer, uint8_t size);
uint8_t buffer_isempty(bufferType *bf);
uint8_t buffer_len(bufferType *bf);
uint8_t buffer_isfull(bufferType *bf);

int main(void) {
	rx_bf = &Buffer_rx;
	while (1) {
	}
	return 0;
}

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
