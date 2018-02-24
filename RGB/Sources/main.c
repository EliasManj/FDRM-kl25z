/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

void LPTM_init(void);
void RGB_init(void);
void shiftLEDs(void);

unsigned long LEDs[3];
unsigned long tmp0;

int main(void) {
	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	RGB_init();
	LPTM_init();
	return 0;
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); //Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; //Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
	NVIC_ICPR = (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER = (1 << 28);		//Activate the LPTM interrupt
}

void RGB_init(void) {
	//First activate SIM_SCGC5 (system control gating register 5) for port B and D
	SIM_SCGC5 = (1 << 10);	//Port B
	SIM_SCGC5 |= (1 << 12);	//Port D
	PORTB_PCR18 = (1<<8);	//Set PTB18 as GPIO
	PORTB_PCR19 = (1<<8);	//set PTB19 as GPIO
	PORTD_PCR1 = (1<<8);	//Set PTD1 as GPIO
	GPIOB_PDDR = (1 << 18);	//Set PTB18 as output
	GPIOB_PDDR |= (1 << 19);	//set PTB19 as output
	GPIOD_PDDR = (1 << 1);	//Set PTD1 as output
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag
	GPIOB_PDOR = (LEDs[0] << 18);
	GPIOB_PDOR |= (LEDs[1] << 19);
	GPIOD_PDOR = (LEDs[2] << 1);
	shiftLEDs();
}

void shiftLEDs(void) {
	tmp0 = LEDs[0];
	LEDs[0] = LEDs[1];
	LEDs[1] = LEDs[2];
	LEDs[2] = tmp0;
}
