/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "buffer.h"

void LPTM_init(void);
void timer0_init(void);
void timer1_init(void);
void RGB_init(void);
void shift_rgb_leds(void);
void gpio_lcd_ports_init(void);
void tmp_counter_50ms_tick(void);
void lcd_initialize(bufferType *bf);
void write_to_lcd(bufferType *bf);

unsigned long LEDs[3];
unsigned long led_temp;

//LCD display

#define IDLE			0
#define SET_RS			1
#define DATA_ENABLE_SET	2
#define ENABLE_CLEAR	3
#define DATA_CLEAR		4

unsigned char DB7_4;

struct lcdState {
	unsigned char RS[2];
	unsigned int EN;
	unsigned int nextState[2];
};

typedef const struct lcdState LcdEncoding;
unsigned int current_lcd_state = IDLE;
unsigned char lcd_en = 0;
unsigned char lcd_rs = 0;
unsigned char lcd_db7 = 0;
unsigned char lcd_db6 = 0;
unsigned char lcd_db5 = 0;
unsigned char lcd_db4 = 0;
unsigned char lcd_bf_empty = 1;
char lcd_data;

LcdEncoding lcdEncoding[5] = { { { 0, 1 }, 0, { SET_RS, IDLE } },		//IDLE
		{ { 0, 1 }, 0, { DATA_ENABLE_SET, DATA_ENABLE_SET } },	//SET_RS
		{ { 0, 1 }, 1, { ENABLE_CLEAR, ENABLE_CLEAR } },	//DATA_ENABLE_SET
		{ { 0, 1 }, 0, { DATA_CLEAR, DATA_CLEAR } },			//ENABLE_CLEAR 
		{ { 0, 1 }, 0, { IDLE, IDLE } },						//DATA_CLEAR
		};

bufferType Buffer_lcd = { 0, 0, BUFLEN, { } };
bufferType *lcd_bf;

int main(void) {
	lcd_bf = &Buffer_lcd;
	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	RGB_init();
	gpio_lcd_ports_init();
	timer0_init();
	timer1_init();
	LPTM_init();
	lcd_initialize(lcd_bf);
	return 0;
}

void timer0_init(void) {
	//Setiing MCGIRCLK
	SIM_SCGC6 |= (1 << 24);		//CLK TMP0
	SIM_SCGC5 |= (1 << 13);		//PTE29
	PORTE_PCR29 |= (3<<8);		//TMP0_CH2 for PTE29
	MCG_C1 |= (1 << 1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz
	SIM_SOPT2 |= (3 << 24);		//TPM clock source select TMPSRC
	TPM0_SC |= (1 << 6);		//TOIE enable interrupt
	TPM0_SC |= (1 << 8);		//DMA enable overflow
	TPM0_C2SC = (5<<2);			//Output compare -> toggle mode FOR TMP0 CH2
	TPM0_C2V =0x0000C350;		//50,000 -> 50000 clock cycles of 1MHz -> 50ms
	NVIC_ICPR |= (1 << 17);
	NVIC_ISER |= (1 << 17);
	TPM0_SC |= (1 << 3);		//CMOD select clock mode mux
}

void timer1_init(void) {
	//Setiing MCGIRCLK
	SIM_SCGC6 |= (1 << 25);		//CLK TMP1
	SIM_SCGC5 |= (1 << 9);		//PTA12
	PORTA_PCR12 = (3<<8);		//TMP1_CH2 for PTA12
	MCG_C1 |= (1 << 1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz
	SIM_SOPT2 |= (3 << 24);		//TPM clock source select TMPSRC
	TPM1_SC |= (1 << 6);		//TOIE enable interrupt
	TPM1_SC |= (1 << 8);		//DMA enable overflow
	TPM1_C0SC = (5<<2);			//Output compare -> toggle mode FOR TMP1 CH0
	TPM1_C0SC |= (1<<6);			//Output compare -> toggle mode FOR TMP1 CH0
	TPM1_C0V =0x000003E8;		//1,000 -> 1000 clock cycles of 1MHz -> 1ms
	NVIC_ICPR |= (1 << 17);
	NVIC_ISER |= (1 << 17);
	TPM1_SC |= (1 << 3);		//CMOD select clock mode mux
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); //Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; //Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag
}

void FTM0_IRQHandler() {
	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM0_C2SC |= (1<<7);
	shift_rgb_leds();
	tmp_counter_50ms_tick();
}

void FTM1_IRQHandler() {
	TPM1_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM1_C0SC |= (1<<7);
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

void tmp_counter_50ms_tick(void) {
	lcd_en = lcdEncoding[current_lcd_state].EN;
	GPIOA_PDOR ^= (-(lcd_en) ^ GPIOA_PDOR ) & (1UL << 17); //SET EN
	if (!buffer_isempty(lcd_bf) && current_lcd_state != 0) {
		if (current_lcd_state == SET_RS) {
			lcd_data = buffer_pop(lcd_bf);
			lcd_rs = lcdEncoding[current_lcd_state].RS[(lcd_data & 0x10) >> 4];
			GPIOA_PDOR ^= (-(lcd_rs) ^ GPIOA_PDOR ) & (1UL << 16); //SET RS
		}
		if (current_lcd_state == DATA_ENABLE_SET) {
			write_to_lcd(lcd_bf);
		}
	}
	current_lcd_state = lcdEncoding[current_lcd_state].nextState[buffer_isempty(
			lcd_bf)];
}

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
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x0E);
	buffer_push(bf, 0x00);
	buffer_push(bf, 0x06);
	buffer_push(bf, 0x14);
	buffer_push(bf, 0x18);
}
