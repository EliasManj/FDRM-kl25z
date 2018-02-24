#include "derivative.h" 

unsigned char tmp_ADC=0;

void delay(unsigned long tiempo_ms) {
	LPTMR0_PSR = 0b00000101;
	LPTMR0_CMR = tiempo_ms;
	LPTMR0_CSR = 0b01000001;  //1; //0x01 // 0x00000001
	while (!(LPTMR0_CSR&0b10000000));
	LPTMR0_CSR = 0;
}

int main(void) {
	unsigned long secuencia[8] = { 0x00000008, 0x0000000C, 0x00000004, 0x00000006, 0x00000002, 0x00000003, 0x00000001, 0x00000009 };
	unsigned long secuenciaR[8] = { 0x00000009, 0x00000001, 0x00000003, 0x00000002, 0x00000006, 0x00000004, 0x0000000C, 0x00000008 };
	unsigned char i = 0, j = 0;

	SIM_SCGC5 = 0x00001C01;
	SIM_SCGC6 = 0x08000001;

	ADC0_CFG2 = 1 << 4;
	ADC0_SC1A =(1<<6);		//Enable ADC interrupt and channel select

	PORTB_PCR0 =0x00000100; //PTB0 GPIO
	PORTB_PCR1 =0x00000100; //PTB1 GPIO
	PORTB_PCR2 =0x00000100; //PTB2 GPIO
	PORTB_PCR3 =0x00000100; //PTB3 GPIO
	PORTD_PCR4 =0x00000100; //PTB5 GPIO
	PORTC_PCR2 =0x00000000; //PTC2 GPIO

	NVIC_ICPR = (1 << 28) + (1 << 15);		//Disable flags
	NVIC_ISER = (1 << 28) + (1 << 15);		//Enable LPTMR0 & ADC interruptions

//	LPTMR0_PSR=(1<<2)+1; 	//Bypass prescaler, 1KHz base de tiempo
//  LPTMR0_CMR=10;			//Start to convert every 10ms
//  LPTMR0_CSR=(1<<6)+1; 	//Enable Interrupt, enable

	GPIOB_PDDR = 0x0000000F;
	GPIOD_PDDR = 0x00000000;

	while(1){

		if((GPIOD_PDIR&0x00000010)==0x00000010){
			GPIOB_PDOR = secuencia[i++];
			delay(tmp_ADC);
			if (i == 8) i = 0;
		} 
		else{
			GPIOB_PDOR = secuenciaR[j++];
			delay(tmp_ADC);
			if (j == 8)  j = 0;
		}
		ADC0_SC1A=0x10000;
		while (ADC0_SC2&0x80);
	}
	return 0;
}

void ADC0_IRQHandler() {
	tmp_ADC = ADC0_RA ;
	
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7); 	//Clear flag
	ADC0_SC1A |=4;
}
