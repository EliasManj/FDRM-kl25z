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
#define RPS			7

//Low power Timer and TPM
void LPTM_init(void);
void RGB_init(void);
void shift_rgb_leds(void);
void Ports_init(void);
void Timer_init(void);
void Set_timer_signal_GPIO(void);

//Step motor
void Toggle_signal(void);
void shift_step_motor(void);
void shift_step_motor_manual(signed int motor_angle);

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
signed int parse_motor_angle(CommandString *commandString);

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

void buffer_push(bufferType *bf, char data);
char buffer_pop(bufferType *bf);
uint8_t buffer_inc(uint8_t pointer, uint8_t size);
uint8_t buffer_isempty(bufferType *bf);
uint8_t buffer_len(bufferType *bf);
uint8_t buffer_isfull(bufferType *bf);
void uart_send_done(bufferType *bf);
void uart_send_temperature(bufferType *bf);

//Define variables
int rx_status;
char command[BUFLEN];
char uart_recive_value;
int cmd_code;
unsigned long LEDs[3];
unsigned long led_temp;
signed int motor_angle;
unsigned long motor_sequence[8] = { 0x00000008, 0x0000000C, 0x00000004,
		0x00000006, 0x00000002, 0x00000003, 0x00000001, 0x00000009 };

//Indicators
unsigned int motor_free_running_flag;
unsigned int motor_dir_flag;
unsigned int motor_manual_angle_flag;
unsigned char timerStateReached;
signed char motorSequenceIndex;
unsigned int temperature;

//TMP Timer
unsigned int tmp_counter_50ms;
unsigned int tmp_counter_1sec;
unsigned int tmp_counter_5sec;
void tmp_counter_50ms_tick(void);
void tmp_counter_1sec_tick(void);
void tmp_counter_5sec_tick(void);


//Define global initializers
void global_variables_initializer(void);
void global_variables_initializer(void) {
	LEDs[0] = 0;
	LEDs[1] = 1;
	LEDs[2] = 1;
	timerStateReached = 0;
	motorSequenceIndex = 0;
	motor_free_running_flag = 0;
	motor_dir_flag = 1;
	rx_bf = &Buffer_rx;
	commandString_p = &commandString;
	uart_recive_value = 0;
	motor_manual_angle_flag = 0;
	tmp_counter_50ms = 0;
	tmp_counter_1sec = 0;
	tmp_counter_5sec = 0;
	temperature = 0;
}
void global_modules_initializer(void);
void global_modules_initializer(void) {
	RGB_init();
	Ports_init();
	LPTM_init();
	ADC_init();
	uart_init();
	Timer_init();
	Set_timer_signal_GPIO();
}

int main(void) {

	global_variables_initializer();
	global_modules_initializer();

	while (1) {
		if (rx_status == 1) {
			rx_status = 0;
			if (commandString_p->is_full == 1) {
				command_clear(commandString_p);
			} else {
				cmd_code = command_compare_cmd_ugly(commandString_p);
				if (cmd_code != 0) {
					uart_send_done(rx_bf);
				}
				switch (cmd_code) {
				case DIR_CW:
					motor_dir_flag = 1;
					break;
				case DIR_CCW:
					motor_dir_flag = 0;
					break;
				case MOTOR_ON:
					motor_free_running_flag = 1;
					motor_manual_angle_flag = 0;
					break;
				case MOTOR_OFF:
					motor_free_running_flag = 0;
					motor_manual_angle_flag = 0;
					break;
				case STEP_CW:
					motor_free_running_flag = 0;
					motor_manual_angle_flag = 1;
					motor_angle = parse_motor_angle(commandString_p);
					break;
				case TEMPLIMIT:
					break;
				default:
					break;
				}
			}
			command_clear(commandString_p);
		}
	}

	return 0;
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
		uart_recive_value = UART0_D;
		buffer_push(rx_bf, uart_recive_value);
		if (uart_recive_value != CARR_RETURN) {
			command_add_item(commandString_p, uart_recive_value);
		} else {
			buffer_push(rx_bf, NEW_LINE);
			rx_status = 1;
		}
		UART0_C2 |= 0x80;	//Turn on TX interrupt
	}
}

void FTM0_IRQHandler() {
	TPM0_SC |= (1 << 7); 		//TOF clear interrupt flag
	TPM0_C2SC |= (1<<7);
	Toggle_signal();
	tmp_counter_50ms_tick();
}

void TPM0_IRQHandler(void) {
	TPM0_SC |= (1 << 7);
}

void ADC0_IRQHandler() {
	if ((ADC0_SC1A &(1<<7))==(1<<7)) {
		unsigned int temp = ((ADC0_RA*1000)/225)+225;
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
	if (motor_free_running_flag == 1)
		shift_step_motor();
	else if (motor_manual_angle_flag == 1)
		shift_step_motor_manual(motor_angle);
	shift_rgb_leds();
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
	TPM0_C2V =0x0000C350;		//50,000 -> 50000 clock cycles of 1MHz -> 50ms
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

void shift_step_motor(void) {
	if (motor_dir_flag == 1) {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
	} else {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex <= 0)
			motorSequenceIndex = 7;
	}
}

void shift_step_motor_manual(signed int motor_angle) {
	if (motor_angle == -1)
		return;
	if (motorSequenceIndex == motor_angle)
		return;
	if (motorSequenceIndex > motor_angle) {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex--]) & 0x0000000F);
		if (motorSequenceIndex < 0)
			motorSequenceIndex = 7;
	} else if (motorSequenceIndex < motor_angle) {
		GPIOB_PDOR = ((~motor_sequence[motorSequenceIndex++]) & 0x0000000F);
		if (motorSequenceIndex >= 8)
			motorSequenceIndex = 0;
	}
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

void Set_timer_signal_GPIO(void) {
	//Set PTC3 as a Test Point
	SIM_SCGC5 |= (1 << 11);		//Activate clock for port C
	PORTC_PCR3 = (1<<8);		//PTC3 set GPIO
	GPIOC_PDDR |= (1 << 3);		//PTC3 set output
	GPIOC_PDOR |= (1 << 3);		//Set PTC3 in LOW initially (negated logic)
}

void Toggle_signal(void) {
	GPIOC_PTOR = (1 << 3);
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

uint8_t string_compare(volatile char *array1, volatile char *array2) {
	int i;
	int response = 0;
	i = 0;
	while (array1[i] == array2[i] && response == 0) {
		if (array1[i] == '\0' || array2[i] == '\0') {
			response = 1;
		}
		i++;
	}
	return response;
}

void strcopy(volatile char *dest, volatile char *source) {
	char data;
	do {
		data = *dest++ = *source++;
	} while (data);
}

void command_add_item(CommandString *commandString, char item) {
	if (commandString->is_full == 0) {
		commandString->data[commandString->n_items++] = item;
	}
	if (commandString->n_items >= (commandString->size - 1)) {
		commandString->is_full = 1;
	}
}

void command_clear(CommandString *commandString) {
	uint8_t i = 0;
	while (i < commandString->size) {
		commandString->data[i++] = 0;
	}
	commandString->n_items = 0;
	commandString->is_full = 0;
}

uint8_t command_compare_cmd_ugly(CommandString *commandString) {
	//DIR:
	if (commandString->data[0] == 'D' && commandString->data[1] == 'I'
			&& commandString->data[2] == 'R' && commandString->data[3] == ':'
			&& commandString->data[4] == 'C') {
		if (commandString->data[5] == 'W' && commandString->data[6] == 0) {
			return DIR_CW;
		} else if (commandString->data[5] == 'C'
				&& commandString->data[6] == 'W') {
			return DIR_CCW;
		} else
			return 0;
	}
	//MOTOR
	else if (commandString->data[0] == 'M' && commandString->data[1] == 'O'
			&& commandString->data[2] == 'T' && commandString->data[3] == 'O'
			&& commandString->data[4] == 'R' && commandString->data[5] == ':'
			&& commandString->data[6] == 'O') {
		if (commandString->data[7] == 'N' && commandString->data[8] == 0) {
			return MOTOR_ON;
		} else if (commandString->data[7] == 'F'
				&& commandString->data[8] == 'F'
				&& commandString->data[9] == 0) {
			return MOTOR_OFF;
		} else
			return 0;
	}
	//STEPCW
	else if (commandString->data[0] == 'S' && commandString->data[1] == 'T'
			&& commandString->data[2] == 'E' && commandString->data[3] == 'P'
			&& commandString->data[4] == 'C' && commandString->data[5] == 'W'
			&& commandString->data[6] == ':' && commandString->data[10] == 0 && commandString->n_items<11) {
		return STEP_CW;
	}
	//TEMPLIMIT
	else if (commandString->data[0] == 'T' && commandString->data[1] == 'E'
			&& commandString->data[2] == 'M' && commandString->data[3] == 'P'
			&& commandString->data[4] == 'L' && commandString->data[5] == 'I'
			&& commandString->data[6] == 'M' && commandString->data[7] == 'I'
			&& commandString->data[8] == 'T' && commandString->data[9] == ':'
			&& commandString->data[15] == 0) {
		return TEMPLIMIT;
	}
	//RPS
	else if (commandString->data[0] == 'R' && commandString->data[1] == 'P'
			&& commandString->data[2] == 'S' && commandString->data[3] == ':'
			&& commandString->data[6] == '.' && commandString->data[8] == 0 && commandString->n_items<8)
		return RPS;
	return 0;
}

signed int parse_motor_angle(CommandString *commandString) {
	unsigned long val = (unsigned long) (100 * (commandString->data[7] - 0x30)
			+ 10 * (commandString->data[8] - 0x30)
			+ (commandString->data[9] - 0x30));
	if (0 <= val && val <= 124)
		return 0;
	else if (125 <= val && val <= 249)
		return 1;
	else if (250 <= val && val <= 374)
		return 2;
	else if (375 <= val && val <= 499)
		return 3;
	else if (500 <= val && val <= 624)
		return 4;
	else if (625 <= val && val <= 749)
		return 5;
	else if (750 <= val && val <= 874)
		return 6;
	else if (875 <= val && val <= 999)
		return 7;
	else
		return -1;
}

void tmp_counter_50ms_tick(){
	tmp_counter_50ms++;
	if(tmp_counter_50ms>=20){
		tmp_counter_50ms = 0;
		tmp_counter_1sec_tick();
	}
}

void tmp_counter_1sec_tick(){
	tmp_counter_1sec++;
	if(tmp_counter_1sec>=5){
		tmp_counter_1sec = 0;
		tmp_counter_5sec_tick();
	}
}

void tmp_counter_5sec_tick(){
	uart_send_temperature(rx_bf);
}

void uart_send_temperature(bufferType *bf) {
	buffer_push(bf, 'T');
	buffer_push(bf, 'E');
	buffer_push(bf, 'M');
	buffer_push(bf, 'P');
	buffer_push(bf, ':');
	buffer_push(bf, 0x0D);
	buffer_push(bf, 0x0A);
	UART0_C2 |= 0x80;	//Turn on TX interrupt
}
