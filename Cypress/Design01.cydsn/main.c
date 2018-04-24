/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */

#include "project.h"
#include "i2c_lcd.h"

#define DIR_CW      1
#define DIR_CCW     2
#define MOTOR_ON    3
#define MOTOR_OFF   4
#define STEP_CW     5
#define TEMPLIMIT   6
#define RPS         7

//Funciones
void GeneralEventHandler(uint32 event, void *eventParam);
int command_compare(uint8 *array);
void command_clear(uint8 *array);
unsigned int parse_motor_vel(uint8 *array, uint8 *rpmstr);
void print_lcd_data(void);
unsigned int parse_temp_limit(uint8 *array);
void dec2str(unsigned int n, uint8 *data);
unsigned int parse_motor_angle(uint8 *data);

//Variables
uint16 motor_period = 0;
uint16 ms_count2 = 0;
uint16 ms_count3 = 0;
unsigned char secuencia[8] = { 0x8, 0xC, 0x4, 0x6, 0x2, 0x3, 0x1, 0x9 };
unsigned char seq = 0x00;
signed int x = 0;
signed int adc_val, temp_limit = 9999, current_temp = 50;
unsigned int command_ocurred = 0, cmd_code, motor_angle, current_angle = 0;
unsigned int motor_dir_flag = 0, motor_vel = 100, motor_free_run_flag = 1,
		motor_manual_angle_flag = 0;
CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
uint8 number_read = 0;
uint8 comand_str[20] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0 };
uint8 rpm_str[3] = { '1', '0', '0' };
uint8 temp_str[4] = { '1', '0', '0', '0' };

void Stack_Handler(uint32 eventCode, void *eventParam) {
	switch (eventCode) {
	case CYBLE_EVT_STACK_ON:
	case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
		CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;
	case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
		wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
		int i = 0;
		for (i = 0; i < wrReqParam->handleValPair.value.len; i++) {
			comand_str[i] = wrReqParam->handleValPair.value.val[i];
		}
		command_ocurred = 1;
		break;
	}
}

CY_ISR( ADC_Int_Handler) {
	adc_val = ADC_GetResult16(0u);
	current_temp = (adc_val * 9999) / 127;
}

CY_ISR( MY_ISR) {
	motor_period++;
	if (motor_period == motor_vel) { // 1 second
		if (motor_free_run_flag == 1 && (current_temp <= temp_limit)) {
			if (motor_dir_flag == 1) {
				Motor_Write(secuencia[x++]);
                seq = ~secuencia[x];
				if (x >= 8)
					x = 0;
			} else {
				Motor_Write(secuencia[x--]);
                seq = secuencia[x];
				if (x < 0)
					x = 7;
			}
		} else if (motor_manual_angle_flag == 1
				&& (current_temp <= temp_limit)) {
			if (current_angle == motor_angle) {

			} else {
				if (motor_dir_flag == 1) {
					Motor_Write(secuencia[x++]);
					current_angle--;
                    if (current_angle > 96) {
						current_angle = 96;
					}
					if (x >= 8)
						x = 0;
				} else {
					Motor_Write(secuencia[x--]);	
                    current_angle++;
					if (current_angle > 96) {
						current_angle = 0;
					}
					if (x < 0)
						x = 7;
				}
			}
		}
		motor_period = 0;
	}
}

CY_ISR( MY_ISR_T2) {
	ms_count2++;
	ms_count3++;

	if (ms_count2 == 1000) { // 1 second
		LED_Write(!LED_Read());
		ms_count2 = 0;
	}
	if (ms_count3 == 3000) {
		print_lcd_data();
		dec2str(current_temp, temp_str);
		ms_count3 = 0;
	}
}

void GeneralEventHandler(uint32 event, void *eventParam) {
	switch (event) {
	case CYBLE_EVT_STACK_ON:
		CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;

	case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
		// Start BLE advertisement for 30 seconds and update link
		CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;

	case CYBLE_EVT_GAP_DEVICE_CONNECTED: /* BLE link is established */
		CyBle_GappStopAdvertisement();
		break;

	case CYBLE_EVT_GATTS_WRITE_REQ:
		wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
		break;

	case CYBLE_EVT_GATTS_WRITE_CMD_REQ:

		break;

	case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF:
		break;

	default:
		break;
	}
}

int main(void) {

	Timer_1_Start(); // Configure and enable timer
	Timer_2_Start(); // Configure and enable timer
	isr_1_StartEx(MY_ISR); // Point to MY_ISR to carry out the interrupt sub-routine
	isr_2_StartEx(ADC_Int_Handler);
	isr_3_StartEx(MY_ISR_T2);
	CyGlobalIntEnable; // Enable global interrupts

	ADC_Start();
	ADC_IRQ_Enable();
	ADC_StartConvert();

	I2CP_Start();
	I2C_LCD_Init(1);
    
	CyBle_Start(Stack_Handler);

	for (;;) {
		CyBle_ProcessEvents();
		if (command_ocurred == 1) {
			command_ocurred = 0;
			cmd_code = command_compare(comand_str);

			switch (cmd_code) {
			case MOTOR_ON:
				motor_free_run_flag = 1;
				motor_manual_angle_flag = 0;
				break;
			case MOTOR_OFF:
				motor_free_run_flag = 0;
				motor_manual_angle_flag = 1;
				break;
			case DIR_CW:
				motor_dir_flag = 0;
				break;
			case DIR_CCW:
				motor_dir_flag = 1;
				break;
			case RPS:
				motor_vel = parse_motor_vel(comand_str, rpm_str);
				motor_period = 0;
				break;
			case STEP_CW:
				motor_angle = parse_motor_angle(comand_str);
				break;
			case TEMPLIMIT:
				temp_limit = parse_temp_limit(comand_str);
				break;
			default:
				break;
			}
			command_clear(comand_str);

		}

	}
}

void command_clear(uint8 *array) {
	for (int x = 0; x < 20; x++) {
		array[x] = 0;
	}
}

int command_compare(uint8 *array) {
	if (array[0] == 'D' && array[1] == 'I' && array[2] == 'R' && array[3] == ':'
			&& array[4] == 'C') {
		if (array[5] == 'W' && array[6] == 0) {
			return DIR_CW;
		} else if (array[5] == 'C' && array[6] == 'W' && array[7] == 0) {
			return DIR_CCW;
		}
		return 0;
	} else if (array[0] == 'R' && array[1] == 'P' && array[2] == 'S'
			&& array[3] == ':' && array[6] == '.' && array[8] == 0) {
		return RPS;
	} else if (array[0] == 'M' && array[1] == 'O' && array[2] == 'T'
			&& array[3] == 'O' && array[4] == 'R' && array[5] == ':'
			&& array[6] == 'O') {
		if (array[7] == 'N' && array[8] == 0) {
			return MOTOR_ON;
		} else if (array[7] == 'F' && array[8] == 'F' && array[9] == 0) {
			return MOTOR_OFF;
		}
		return 0;
	} else if (array[0] == 'S' && array[1] == 'T' && array[2] == 'E'
			&& array[3] == 'P' && array[4] == 'C' && array[5] == 'W'
			&& array[6] == ':' && array[11] == 0) {
		return STEP_CW;
	} else if (array[0] == 'T' && array[1] == 'E' && array[2] == 'M'
			&& array[3] == 'P' && array[4] == 'L' && array[5] == 'I'
			&& array[6] == 'M' && array[7] == 'I' && array[8] == 'T'
			&& array[9] == ':' && array[13] == '.' && array[15] == 0) {
		return TEMPLIMIT;
	}

	return 0;
}

unsigned int parse_motor_vel(uint8 *array, uint8 *rpmstr) {
	unsigned long val = (signed long) ((100 * (array[4] - 0x30))
			+ (10 * (array[5] - 0x30)) + (array[7] - 0x30));
	rpmstr[0] = array[4];
	rpmstr[1] = array[5];
	rpmstr[2] = array[7];

	val = (6000 / val);
	return (unsigned int) val;
}

void print_lcd_data(void) {
	lcd_clear();
	lcd_line0(0);
	if (motor_dir_flag == 1) {
		lcd_write_char('C');
		lcd_write_char('C');
		lcd_write_char('W');
	} else {
		lcd_write_char('C');
		lcd_write_char('W');
	}
	lcd_line0(3); 
	lcd_write_char('-');
	lcd_line0(5); 
	lcd_write_char('T');
	lcd_write_char('E');
	lcd_write_char('M');
	lcd_write_char('P');
	lcd_write_char(':');
	lcd_write_char(temp_str[3]);
	lcd_write_char(temp_str[2]);
	lcd_write_char(temp_str[1]);
	lcd_write_char('.');
	lcd_write_char(temp_str[0]);
	lcd_write_char('C');

	lcd_line2_pos(1);
	lcd_write_char('R');
	lcd_write_char('P');
	lcd_write_char('M');
	lcd_write_char(':');
	lcd_write_char(rpm_str[0]);
	lcd_write_char(rpm_str[1]);
	lcd_write_char('.');
	lcd_write_char(rpm_str[2]);

}

unsigned int parse_temp_limit(uint8 *array) {
	unsigned int val = (signed long) (1000 * (array[10] - 0x30)
			+ 100 * (array[11] - 0x30) + 10 * (array[12] - 0x30)
			+ (array[14] - 0x30));
	return val;
}

void dec2str(unsigned int n, uint8 *data) {
	data[3] = n / 1000 + 0x30;
	n = n % 1000;
	data[2] = n / 100 + 0x30;
	n = n % 100;
	data[1] = n / 10 + 0x30;
	n = n % 10;
	data[0] = n + 0x30;
}

unsigned int parse_motor_angle(uint8 *data) {
	unsigned long val = (unsigned long) (100 * (data[7] - 0x30)
			+ 10 * (data[8] - 0x30) + (data[9] - 0x30));
	val = (val * 96) / 999;
    if (val == 0) return 0;
	return val;
}
