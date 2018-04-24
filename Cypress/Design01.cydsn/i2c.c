#include "i2c.h"


unsigned char flagRTII2C=0;

void nopDelay(void){
	
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");	
}

//void I2C_Init(I2C p)
//{
	//IICF=0x40;
	//IICC=0b10010000;
	//IICF = p.baudrate; // 100kbps
	//IICC = p.I2C_ON_MST_ON; //ModI2C=1, MasterMode=1
	
	//IICC = 0b11010000; //INT-ON
//}

void I2C_Write(unsigned char data)
{
	
	//unsigned int kp;
	//nopDelay();
	//if(flagRTII2C==1){
		//flagRTII2C=0;

	
	//IICC_MST = 1; 	
	//IICD = data; 
	//}
    I2CP_I2CMasterSendStart(0x27,0x00);
	I2CP_I2CMasterWriteByte(data);
    
	//for(kp=0;kp<20;kp++)
	//		asm ("nop");
//	if(flagRTII2C==1){
//		flagRTII2C=0;
//			for(kp=0;kp<20;kp++)
//				asm nop;
		//	}
	flagRTII2C=0;
//	do
//		asm nop;
//	while(IICS_IICIF == 0);
	//I2C_ClearFlag();	
}

void I2C_WriteFor(unsigned char data){
	
	//unsigned int kl;
	
	//nopDelay();
	//if(flagRTII2C==1){
		//IICC_MST = 1; 
		//IICD = data;
    I2CP_I2CMasterWriteByte(data);
	//}
		
//		if(flagRTII2C==1){
//			//flagRTII2C=0;
//			nopDelay();
//			flagRTII2C=0;
//		}
	//if(flagRTII2C==1){
		
	//	}
}

void I2C_Stop(void)
{
	I2CP_I2CMasterSendStop();
    //IICC_MST = 0; //MasterMode=0
}

void I2C_ClearFlag(void)
{
	//(void) IICS; 
	//IICS_IICIF =1;
    
	//IICS_IICIF = p.flagOFF; 
}

void IIC_RTI(void)
{
	I2C_ClearFlag(); //clear the flag
	
}

