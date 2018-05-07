/*
 * buffer.c
 *
 *  Created on: Apr 8, 2018
 *      Author: Octavio Rojas
 */

#include "buffer.h"
#include "derivative.h"

//Se programan como tal las funciones

void buffer_push(bufferType *bf, char data) {

	bf->data[bf->tail] = data;

	bf->tail = buffer_inc(bf->tail, bf->size);

} //mete un dato en la cola del buffer

char buffer_pop(bufferType *bf) {

	uint8_t item = bf->data[bf->head];

	bf->head = buffer_inc(bf->head, bf->size);

	return item;

} //saca un dato de la cabeza del buffer

uint8_t buffer_inc(uint8_t pointer, uint8_t size) {

	uint8_t i = pointer;

	if (++i >= size) {

		i = 0;

	} //esta es la funcion que hace que sea un buffer circular.

	return i;

}

uint8_t buffer_isempty(bufferType *bf) {

	return bf->head == bf->tail; //si el apuntador de head y el de tail son iguales entonces significa que está vacío el buffer

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

//////////////////////
//Funciones de texto//
//////////////////////

void uart_send_done(bufferType *bf) {
	buffer_push(bf, 'D');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, 'E');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_ready(bufferType *bf) {
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_notready(bufferType *bf) {
	buffer_push(bf, 'N');
	buffer_push(bf, 'O');
	buffer_push(bf, 'T');
	buffer_push(bf, ' ');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_startq(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'T');
	buffer_push(bf, 'A');
	buffer_push(bf, 'R');
	buffer_push(bf, 'T');
	buffer_push(bf, '?');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_sysverified(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'Y');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'V');
	buffer_push(bf, 'E');
	buffer_push(bf, 'R');
	buffer_push(bf, 'I');
	buffer_push(bf, 'F');
	buffer_push(bf, 'I');
	buffer_push(bf, 'E');
	buffer_push(bf, 'D');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_sysalreadyon(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'Y');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'A');
	buffer_push(bf, 'L');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt

}

void uart_send_sysalreadyoff(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'Y');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'A');
	buffer_push(bf, 'L');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'F');
	buffer_push(bf, 'F');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_lightalreadyon(bufferType *bf) {
	buffer_push(bf, 'L');
	buffer_push(bf, 'I');
	buffer_push(bf, 'G');
	buffer_push(bf, 'H');
	buffer_push(bf, 'T');
	buffer_push(bf, ' ');
	buffer_push(bf, 'A');
	buffer_push(bf, 'L');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_lightalreadyoff(bufferType *bf) {
	buffer_push(bf, 'L');
	buffer_push(bf, 'I');
	buffer_push(bf, 'G');
	buffer_push(bf, 'H');
	buffer_push(bf, 'T');
	buffer_push(bf, ' ');
	buffer_push(bf, 'A');
	buffer_push(bf, 'L');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'F');
	buffer_push(bf, 'F');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_sysisoff(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'Y');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'I');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'F');
	buffer_push(bf, 'F');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_doorajar(bufferType *bf) {
	buffer_push(bf, 'D');
	buffer_push(bf, 'O');
	buffer_push(bf, 'O');
	buffer_push(bf, 'R');
	buffer_push(bf, ' ');
	buffer_push(bf, 'A');
	buffer_push(bf, 'J');
	buffer_push(bf, 'A');
	buffer_push(bf, 'R');
	buffer_push(bf, '!');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void uart_send_lightison(bufferType *bf) {
	buffer_push(bf, 'L');
	buffer_push(bf, 'I');
	buffer_push(bf, 'G');
	buffer_push(bf, 'H');
	buffer_push(bf, 'T');
	buffer_push(bf, ' ');
	buffer_push(bf, 'I');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void lcd_send_returning(bufferType *bf) {

	buffer_push(bf, 0x00);
	buffer_push(bf, 0x01);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x02);

	buffer_push(bf, 0x15); //R
	buffer_push(bf, 0x12); //R

	buffer_push(bf, 0x16); //e
	buffer_push(bf, 0x15); //e

	buffer_push(bf, 0x17); //t
	buffer_push(bf, 0x14); //t

	buffer_push(bf, 0x17); //u
	buffer_push(bf, 0x15); //u

	buffer_push(bf, 0x17); //r
	buffer_push(bf, 0x12); //r

	buffer_push(bf, 0x16); //n
	buffer_push(bf, 0x1E); //n

	buffer_push(bf, 0x16); //i
	buffer_push(bf, 0x19); //i

	buffer_push(bf, 0x16); //n
	buffer_push(bf, 0x1E); //n

	buffer_push(bf, 0x16); //g
	buffer_push(bf, 0x17); //g

}

void uart_send_stopscan(bufferType *bf) {
	buffer_push(bf, 'S');
	buffer_push(bf, 'T');
	buffer_push(bf, 'O');
	buffer_push(bf, 'P');
	buffer_push(bf, 'E');
	buffer_push(bf, 'D');
	buffer_push(bf, ' ');
	buffer_push(bf, 'S');
	buffer_push(bf, 'C');
	buffer_push(bf, 'A');
	buffer_push(bf, 'N');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void send_Matlabisready(bufferType *bf) {

	buffer_push(bf, 'M');
	buffer_push(bf, 'A');
	buffer_push(bf, 'T');
	buffer_push(bf, 'L');
	buffer_push(bf, 'A');
	buffer_push(bf, 'B');
	buffer_push(bf, ' ');
	buffer_push(bf, 'I');
	buffer_push(bf, 'S');
	buffer_push(bf, ' ');
	buffer_push(bf, 'R');
	buffer_push(bf, 'E');
	buffer_push(bf, 'A');
	buffer_push(bf, 'D');
	buffer_push(bf, 'Y');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

void dec2str(unsigned int n, unsigned char *data) {

	data[2] = n / 100 + 0x30;

	n = n % 100;

	data[1] = n / 10 + 0x30;

	n = n % 10;

	data[0] = n + 0x30;

}

