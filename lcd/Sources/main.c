#include "derivative.h" /* include peripheral declarations */


void delay_ms(unsigned long);
void LCD_init(void);
void LCD_interface_init(void);
void LCD_send(unsigned char, unsigned char, unsigned char);

int main(void)
{	
	LCD_interface_init();
	LCD_init();
	
	LCD_send('E',1,8);
	for(;;) {
		
	}
	return 0;
}

void LCD_interface_init(void){
	SIM_SCGC5|=(1<<13)+(1<<10)+1; //CLK para PORT E, PORT B y LPTMR
	
	PORTE_PCR2|=1<<8;		//Config como GPIO
	PORTE_PCR3|=1<<8;
	PORTB_PCR8|=1<<8;
	PORTB_PCR9|=1<<8;
	PORTB_PCR10|=1<<8;
	PORTB_PCR11|=1<<8;
	
	GPIOE_PDDR|=(1<<2)+(1<<3);	//PTE 2 - 3 Output
	GPIOB_PDDR=15<<8;		//PTB 8 - 11 Output
	
}

void delay_ms(unsigned long tiempo_ms){
	LPTMR0_PSR=0b00000101;	//Prescaler bypass y LPO de CLK
	LPTMR0_CMR=tiempo_ms;	//Asignar tiempo_ms a registro con valor a comparar
	LPTMR0_CSR=0b00000001;	//Timer Enable 
	
	while(!(LPTMR0_CSR&0b10000000));				//Esperar la bandera TCF
	LPTMR0_CSR=1<<7;								//Sobreescribir CSR para deshabilitar LPTMR y w1c de TCF
}

void LCD_init(void){
	delay_ms(15);
	LCD_send(0x30,0,4);
	delay_ms(5);
	LCD_send(0x30,0,4);
	delay_ms(1);
	LCD_send(0x30,0,4);
	LCD_send(0x20,0,4);
	LCD_send(0x2C,0,8);		//4 bits operation, 2 lines and resolution 5x10 dots
	LCD_send(0x08,0,8);		//Display OFF, Cursor OFF, Blinking OFF
	LCD_send(0x01,0,8);		
	LCD_send(0x06,0,8);		//Increment and no shift
	LCD_send(0x0E,0,8);	//Display ON, Cursor ON, Blinking OFF
}

void LCD_send(unsigned char dato, unsigned char RS, unsigned char size){
	if(RS) GPIOE_PSOR|=1<<3;
	else GPIOE_PCOR|=1<<3;
	
	GPIOE_PCOR|=1<<2;	//E en cero como inicial
	delay_ms(1);
	GPIOE_PSOR|=1<<2;	//E en uno para enviar comando/dato
	delay_ms(1);
		if(dato&0x80) GPIOB_PSOR|=1<<8;
		else GPIOB_PCOR|=1<<8;
		if(dato&0x40) GPIOB_PSOR|=1<<9;
		else GPIOB_PCOR|=1<<9;
		if(dato&0x20) GPIOB_PSOR|=1<<10;
		else GPIOB_PCOR|=1<<10;
		if(dato&0x10) GPIOB_PSOR|=1<<11;
		else GPIOB_PCOR|=1<<11;
	delay_ms(1);
	GPIOE_PCOR|=1<<2;	//E en cero para terminar de enviar comando/dato
	
	if(size==8){
		dato<<=4;			//Shift left dato cuatro a la izquierda
		GPIOE_PSOR|=1<<2;	//E en uno para enviar comando/dato
			delay_ms(1);
				if(dato&0x80) GPIOB_PSOR|=1<<8;
				else GPIOB_PCOR|=1<<8;
				if(dato&0x40) GPIOB_PSOR|=1<<9;
				else GPIOB_PCOR|=1<<9;
				if(dato&0x20) GPIOB_PSOR|=1<<10;
				else GPIOB_PCOR|=1<<10;
				if(dato&0x10) GPIOB_PSOR|=1<<11;
				else GPIOB_PCOR|=1<<11;
			delay_ms(1);
			GPIOE_PCOR|=1<<2;	//E en cero para terminar de enviar comando/dato
	}
	
}
