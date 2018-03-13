#include "derivative.h" 

void LPTM_init(void);
void RGB_init(void);
void shiftLEDs(void);
void ADC_init(void);
void Ports_init(void);
void shift(void);
void timer_Init(void);
void UART_init(void);

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
	Ports_init();
	timer_Init();
	//LPTM_init();
	ADC_init();
	return 0;
}

void FTM0_IRQHandler() {
	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM0_C2SC |= (1<<7);
	timerStateReached = 1;
	shift();
	shiftLEDs();
}

void timer_Init(void) {
	//Setiing MCGIRCLK
	SIM_SCGC6 |= (1 << 24);		//CLK TMP0
	SIM_SCGC5 |= (1 << 13);		//PTE29
	PORTE_PCR29 |= (3<<8);		//TMP0_CH2
	MCG_C1 |= (1 << 1);			//IRCLKEN
	MCG_C2 |= 1;				//IRCS = 1 (Fast clock) 4MHz
	MCG_SC = (2 << 1);			//FCRDIV = 4, divide by 4 = 1MHz
	SIM_SOPT2 = (3 << 24);		//TPM clock source select TMPSRC
	TPM0_SC |= (1 << 6);		//TOIE enable interrupt
	TPM0_SC |= (1 << 8);		//DMA enable overflow
	TPM0_C2SC = (5<<2);			//Output compare -> toggle mode FOR TMP0 CH2
	TPM0_C2V =0x0000FFFF;		//Maximun value for 16 bits 65535
	NVIC_ICPR |= (1 << 17);
	NVIC_ISER |= (1 << 17);
	TPM0_SC |= (1 << 3);		//CMOD select clock mode mux
}

void Ports_init(void) {
	SIM_SCGC5 |= (1 << 10);
	PORTB_PCR0 =(1<<8); 		//PTB0 GPIO
	PORTB_PCR1 =(1<<8); 		//PTB1 GPIO
	PORTB_PCR2 =(1<<8); 		//PTB2 GPIO
	PORTB_PCR3 =(1<<8); 		//PTB3 GPIO
	GPIOB_PDDR |= 0x0000000F;	//Set GPIOB as output
}

void ADC_init(void) {
	SIM_SCGC6 |= (1 << 27);
	SIM_SCGC5 |= (1 << 13);
	NVIC_ICPR |= (1 << 15);
	NVIC_ISER |= (1 << 15);
	//ADC0
	ADC0_CFG1 = 0x00000000;
	ADC0_CFG2 = (0 << 4);		//Seleccionar el canal A del ADC
	ADC0_SC1A =0b1000100;//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE21
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); //Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; //Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
	LPTMR0_CMR = 500;			//compare of 500 clock cycles = .5 secs
	NVIC_ICPR |= (1 << 28);		//Clean flag of LPTM in the interrupt vector
	NVIC_ISER |= (1 << 28);		//Activate the LPTM interrupt
	LPTMR0_CSR = 0b01000001;	//Activate the timer and enable interrupts	
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
		temp = ((ADC0_RA*1000)/225)+225;
		if(timerStateReached==1) {
			//LPTMR0_CMR = temp;
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
	GPIOB_PDOR = ((secuencia[i++]) & 0x0000000F);
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

void UART_init(void) {//Se configura un canal de recepción y transmisión E_20 Tx y E_21 Rx
	SIM_SCGC5 |= (1 << 13);		//RELOJ PORTE
	SIM_SCGC4 |= (1 << 10); 	//RELOJ UART
	PORTE_PCR20 =(1<<10);	//UART0 pin Tx (alt4)
	PORTE_PCR21 =(1<<10);	//UART0 pin RX (alt4)
	UART0_BDL = 137;		//Clk=640*32768, baud rate 9600bps porqué BDL=137??
	UART0_C2 = 12;			//Habilitando TX y RX (TE y RE en el registro)

}
