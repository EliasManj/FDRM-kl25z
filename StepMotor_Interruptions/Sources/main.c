#include "derivative.h" 

void LPTM_init(void);
void RGB_init(void);
void shiftLEDs(void);
void adcInit(void);
void portsInit(void);
void shift(void);

unsigned int temp;
unsigned long LEDs[3];
unsigned long tmp0;
unsigned char timerStateReached;
unsigned char i;
unsigned long secuencia[8] = { 0x00000008, 0x0000000C, 0x00000004, 0x00000006,
		0x00000002, 0x00000003, 0x00000001, 0x00000009 };

int main(void) {
	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	timerStateReached = 0;
	i = 0;
	RGB_init();
	portsInit();
	LPTM_init();
	adcInit();
	return 0;
}

void portsInit(void) {
	SIM_SCGC5 |= (1 << 10);
	PORTB_PCR0 =(1<<8); 		//PTB0 GPIO
	PORTB_PCR1 =(1<<8); 		//PTB1 GPIO
	PORTB_PCR2 =(1<<8); 		//PTB2 GPIO
	PORTB_PCR3 =(1<<8); 		//PTB3 GPIO
	GPIOB_PDDR |= 0x0000000F;	//Set GPIOB as output
}

void adcInit(void) {
	SIM_SCGC6 |= (1 << 27);
	SIM_SCGC5 |= (1 << 13);
	NVIC_ICPR |= (1 << 15);
	NVIC_ISER |= (1 << 15);
	//ADC0
	ADC0_CFG1 = 0x00000000;
	ADC0_CFG2 = (1 << 4);		//Seleccionar el canal B del ADC
	ADC0_SC1A =0b1000100;		//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE30	
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); 		//Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; 	//Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt
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

void ADC0_IRQHandler() {
	if ((ADC0_SC1A &(1<<7))==(1<<7)) {
		temp = ((ADC0_RA/255)*1000)+225;
		if(timerStateReached==1) {
			LPTMR0_CMR = temp;
			timerStateReached=0;
		}
		ADC0_SC1A |=4;
	}
}

void LPTimer_IRQHandler() {
	LPTMR0_CSR |= (1 << 7);	//Clear timer compare flag
	timerStateReached = 1;
	shift();
	shiftLEDs();
}

void shift(void) {
	GPIOB_PDOR = ((~secuencia[i++]) & 0x0000000F);
	if (i == 8) {
		i = 0;
	}
}

void shiftLEDs(void) {
	GPIOB_PDOR |= (LEDs[0] << 18);
	GPIOB_PDOR |= (LEDs[1] << 19);
	GPIOD_PDOR = (LEDs[2] << 1);
	tmp0 = LEDs[0];
	LEDs[0] = LEDs[1];
	LEDs[1] = LEDs[2];
	LEDs[2] = tmp0;
}

