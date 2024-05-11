#include <i2c.h>
#include < regx51.h>

sbit SCL = P3^6;
sbit SDA = P3^5;

void Delay_I2C(unsigned int Time){
	while(Time--);
}
/********************Bat dau gui tin hieu*****************/
void I2C_Start(void){
	SDA = 1;
	SCL = 1;
	SDA = 0;
	Delay_I2C(2);
	SCL = 0;
}
/*******************Dung gui tin hieu********************/
void I2C_Stop(void){
	SDA = 0;
	SCL = 0;
	SCL = 1;
	Delay_I2C(2);
	SDA = 1;
}

/*********************Nhan ACK tu slave******************/
bit I2C_Get_ACK(void){
	bit Result;
	SDA = 1;
	Delay_I2C(2);
	SCL = 1;
	Delay_I2C(2);
	Result = SDA;
	SCL = 0;
	return Result;
}

bit I2C_Write(unsigned char Data){
	unsigned char i;
	for(i = 0; i < 8; i++){
		SDA = (Data << i) & 0x80;
		SCL = 1;
		Delay_I2C(2); 
		SCL = 0;
	}
	// ACK
	return(I2C_Get_ACK());
}


/********************Gui ACK tu Master******************/
void I2C_ACK(void){
	SDA = 0;
	Delay_I2C(2);
	SCL = 1;
	Delay_I2C(2);
	SCL = 0;
}
/*******************Gui NACK tu Master********************/
void I2C_NACK(void){
	SDA = 1;
	Delay_I2C(2);
	SCL = 1;
	Delay_I2C(2);
	SCL = 0;
}

/********************* Doc du lieu tu Slave*****************/
unsigned char I2C_Read(bit ACK){
	unsigned char i, Data = 0x00;
	for(i = 0; i < 8; i++){
		SDA = 1;
		Delay_I2C(2);
		SCL = 1;
		Delay_I2C(2);
		Data = (Data << 1) | SDA;
		SCL = 0;
	}
	// ACK
	if(ACK){ I2C_ACK();}
	else    {I2C_NACK();}
	return Data;
}