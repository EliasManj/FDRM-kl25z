/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

#define NEW_LINE 	0x0A
#define CARR_RETURN 0x0D
#define BACKSPACE 	0x08

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

void uart_init(void);
void buffer_push(bufferType *bf, char data);
char buffer_pop(bufferType *bf);
uint8_t buffer_inc(uint8_t pointer, uint8_t size);
uint8_t buffer_isempty(bufferType *bf);
uint8_t buffer_len(bufferType *bf);
uint8_t buffer_isfull(bufferType *bf);

int main(void) {
	rx_bf = &Buffer_rx;
	uart_init();
	while (1) {
	}
	return 0;
}

void uart_init(void) {
	SIM_SCGC4 |= (1 << 10);	//CLK UART0
	SIM_SCGC5 |= (1 << 9);	//CLOCK for PORTA
	SIM_SOPT2 |= (1 << 26);	//Enable UART0 clock with MCGFLLCLK clock or MCGPLLCLK/2 clock
	PORTA_PCR1 = (1<<9);	//Port control for UART_0
	PORTA_PCR2 = (1<<9);	//Port control for UART_0
	UART0_BDL = 137;		//clock=640*32768, baud rate 9600
	UART0_C2 |= (1 << 5);		//reciver interrupt enable for RDRF
	UART0_C2 |= (1 << 2);		//RE reciver enable
	UART0_C2 |= (1 << 3);		//TE Transmiter enable
	NVIC_ISER |= (1 << 12);
	NVIC_ICPR |= (1 << 12);
}

void UART0_IRQHandler(void) {
	//WRITE
	if (((UART0_S1 & 0x80) >> 7) && (!buffer_isempty(rx_bf))) {
		UART0_D = buffer_pop(rx_bf);
		if (buffer_isempty(rx_bf)) {
			UART0_C2 &= ~(0x80);
		}
	}
	//READ
	if ((UART0_S1 & 0x20) >> 5 && !(buffer_isfull(rx_bf))) {
		val = UART0_D;
		buffer_push(rx_bf, val);
		if (val != CARR_RETURN) {

		} else {
			buffer_push(rx_bf, NEW_LINE);
			rx_status = 1;
		}
		UART0_C2 |= 0x80;	//Turn on TX interrupt
	}
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
