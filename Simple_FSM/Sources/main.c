/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

void RGB_init(void);
void LPTM_init(void);
void setInput(void);

struct State {
	unsigned char Output[3];
	unsigned long Time;
	unsigned char Next[2];
};

#define RED 	0
#define GREEN	1
#define BLUE 	2
#define PURPLE	3
#define YELLOW	4
#define CYAN	5

typedef const struct State StateEncoding;

StateEncoding stateEncoding[6] = { 
		{ { 0, 1, 1 }, 500, { GREEN, CYAN } }, 			//RED state
		{ { 1, 0, 1 }, 500, { BLUE, RED } },			//GREEN state
		{ { 1, 1, 0 }, 500, { PURPLE, GREEN } },		//BLUE state
		{ { 0, 1, 0 }, 500, { YELLOW, BLUE } },			//PURPLE state
		{ { 0, 0, 1 }, 500, { CYAN, PURPLE } },			//YELLOW state
		{ { 1, 0, 0 }, 500, { RED, YELLOW } },			//CYAN state
};

unsigned char currentState;
unsigned long Input;

int main(void) {
	currentState = RED;
	RGB_init();
	setInput();
	LPTM_init();
	return 0;
}

void setInput(void){
	PORTB_PCR8 = (1<<8); 	//Set PTB8 as GPIO
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag
	GPIOB_PDOR = ((stateEncoding[currentState].Output[0])<<18);
	GPIOB_PDOR |= ((stateEncoding[currentState].Output[1])<<19);
	GPIOD_PDOR = ((stateEncoding[currentState].Output[2])<<1);
	LPTMR0_CMR = stateEncoding[currentState].Time;
	Input = ~((GPIOB_PDIR&0x00000100)>>8);
	currentState = stateEncoding[currentState].Next[Input];
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

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); //Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; //Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt
}
