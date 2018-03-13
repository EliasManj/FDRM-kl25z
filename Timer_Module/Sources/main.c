/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

void timerInit(void);
void RGB_init(void);
unsigned int i = 0;


int main(void) {

	RGB_init();
	timerInit();
	GPIOE_PDOR = (1<<1);
	pointerPTE29 = &GPIOE_PDIR;
	
	//*((volatile unsigned long *)GPIOD_PDOR) = *pointerPTE29;
	GPIOB_PDOR = (1<<19);
	GPIOB_PDOR |= (1<<18);
	do {
		do {
		} while (!(TPM0_C2SC &(1<<7)));
		TPM0_C2SC|=(1<<7);		//Apagar bandera
		
		GPIOD_PDOR = (i<<1);
		i=~i&0x00000001;
	} while(1);

	return 0;
}

void timerInit(void) {
	//Setiing MCGIRCLK
	SIM_SCGC6 |= (1 << 24);		//CLK TMP0
	SIM_SCGC5 |= (1 << 13);		//PTE29
	PORTE_PCR29 = (3<<8);		//TMP0_CH2
	MCG_C1 |= (1 << 1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz
	SIM_SOPT2 = (3 << 24);		//TPM clock source select TMPSRC
	TPM0_SC = (1 << 3);			//CMOD select clock mode mux
	TPM0_C2SC = (5<<2);			//Output compare -> toggle mode FOR TMP0 CH2
	TPM0_C2V =0x0000FFFF;		//Maximun value for 16 bits 65535
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

void ADC_init(void) {
	SIM_SCGC6 |= (1 << 27);
	SIM_SCGC5 |= (1 << 13);
	NVIC_ICPR |= (1 << 15);
	NVIC_ISER |= (1 << 15);
	//ADC0
	ADC0_CFG1 = 0x00000000;
	ADC0_CFG2 = (1 << 4);		//Seleccionar el canal B del ADC
	ADC0_SC1A =0b1000100;		//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE30	
}




