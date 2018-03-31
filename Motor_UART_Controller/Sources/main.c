#include "derivative.h" 

//ASCII codes
#define BUFLEN 		16
#define NEW_LINE 	0x0A
#define CARR_RETURN 0x0D
#define BACKSPACE 	0x08

//Commands encoding
#define DIR_CW 		1
#define DIR_CCW		2
#define MOTOR_ON 	3
#define MOTOR_OFF 	4
#define STEP_CW		5
#define TEMPLIMIT 	6

//Low power Timer and TPM
void LPTM_init(void);
void RGB_init(void);
void shiftLEDs(void);
void Ports_init(void);
void shift(void);
void Timer_init(void);
void UART_init(void);
void Set_timer_signal_GPIO(void);
void Toggle_signal(void);

//ADC
void ADC_init(void);

//UART
void uart_init(void);

//Define Command struct
struct Command {
	volatile uint8_t size;
	volatile uint8_t n_items;
	volatile uint8_t is_full;
	volatile char data[BUFLEN];
};

typedef struct Command CommandString;
CommandString commandString = { BUFLEN, 0, 0, { } };
CommandString *commandString_p;

uint8_t string_compare(volatile char *array1, volatile char *array2);
void strcopy(volatile char *dest, volatile char *source);
void command_add_item(CommandString *commandString, char item);
void command_clear(CommandString *commandString);
uint8_t command_compare_cmd_ugly(CommandString *commandString);

//Define Buffer
struct Buffer {
	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t size;
	volatile char data[BUFLEN];
};

typedef struct Buffer bufferType;
bufferType Buffer_rx = { 0, 0, BUFLEN, { } };
bufferType *rx_bf;

void uart_init(void);
void buffer_push(bufferType *bf, char data);
char buffer_pop(bufferType *bf);
uint8_t buffer_inc(uint8_t pointer, uint8_t size);
uint8_t buffer_isempty(bufferType *bf);
uint8_t buffer_len(bufferType *bf);
uint8_t buffer_isfull(bufferType *bf);
void uart_send_done(bufferType *bf);

//Define variables
int rx_status;
char command[BUFLEN];
char val;
int cmd_code;
unsigned int temp;
unsigned long LEDs[3];
unsigned long tmp0;
unsigned char timerStateReached;
unsigned char motorSequenceIndex;
unsigned long secuencia[8] = { 0x00000008, 0x0000000C, 0x00000004, 0x00000006,
		0x00000002, 0x00000003, 0x00000001, 0x00000009 };

int main(void) {

	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	timerStateReached = 0;
	motorSequenceIndex = 0;
	RGB_init();
	Ports_init();
	Timer_init();
	LPTM_init();
	ADC_init();
	Set_timer_signal_GPIO();
	//uart_init();
	return 0;
}

void UART0_IRQHandler(void) {
	
}

void FTM0_IRQHandler() {
	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM0_C2SC |= (1<<7);
	Toggle_signal();
}

void TPM0_IRQHandler(void) {
	TPM0_SC |= (1 << 7);
}

void ADC0_IRQHandler() {
	if ((ADC0_SC1A &(1<<7))==(1<<7)) {
		temp = ((ADC0_RA*1000)/225)+225;
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

void Timer_init(void) {
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
	TPM0_C2V =0x0000FFFF;		//Maximun value for 16 bits 65535 -> 65535 clock cycles of 1MHz
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
	ADC0_SC1A =0b1000100;		//Habilitar interrupciones del ADC y el canal AD4-> esta en el canal PTE21
}

void LPTM_init(void) {
	SIM_SCGC5 |= (1 << 0); 		//Activate the LPTMR in the system control gating register
	LPTMR0_PSR = 0b00000101; 	//Bypass the preescaler and select the LPO(low power oscilator of 1Khz as the source of the timer)
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

void shift(void) {
	GPIOB_PDOR = ~((secuencia[motorSequenceIndex++]) & 0x0000000F);
	if (motorSequenceIndex == 8) {
		motorSequenceIndex = 0;
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

void Set_timer_signal_GPIO(void){
	//Set PTC3 as a Test Point
	SIM_SCGC5 |= (1<<11);		//Activate clock for port C
	PORTC_PCR3 = (1<<8);		//PTC3 set GPIO
	GPIOC_PDDR |= (1<<3);		//PTC3 set output
	GPIOC_PDOR |= (1<<3);		//Set PTC3 in LOW initially (negated logic)
}

void Toggle_signal(void){
	GPIOC_PTOR = (1<<3);
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

void uart_send_done(bufferType *bf) {
	buffer_push(bf, 'D');
	buffer_push(bf, 'O');
	buffer_push(bf, 'N');
	buffer_push(bf, 'E');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}

