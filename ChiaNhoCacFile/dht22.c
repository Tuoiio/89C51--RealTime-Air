#include <dht22.h>
#include <intrins.h>

sbit DHT_PIN = P3^7;

static  unsigned char  DHT_Data[5], CheckSum = 0;

void delay_30us(void){
	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	 
	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	 
	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	 
	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	 
//	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	_nop_();	 // thoi gian goi ham delay lau hon nen bo dong nay
}

 static void Request(void){
	DHT_PIN = 0;		 		
	delay_ms(20);
	DHT_PIN = 1;				
}

static  void Response(){
	while(DHT_PIN);
	while(!(DHT_PIN));
	while(DHT_PIN);
}

bit  DHT22_Read(void)	{
	char i, j;
	Request();
	Response();
	for(i = 0; i < 5; i++){
		DHT_Data[i] = 0;
		for (j = 0; j < 8; j++){
			while(!(DHT_PIN));		
			delay_30us();
			if(DHT_PIN)
				DHT_Data[i] = (DHT_Data[i] << 1) | 0x01;
			else
				DHT_Data[i] = DHT_Data[i] << 1;
			while(DHT_PIN);
		}
	}
	// Kiem tra lai ket qua xem co dung khong
	CheckSum = DHT_Data[0] + DHT_Data[1] + DHT_Data[2] + DHT_Data[3];
	
	// Neu sai tra ve 0, dung tra ve 1
	if(CheckSum != DHT_Data[4])	return 0;
	else										return 1;
}

float DHT22_Humidity(void){
	int Temp;
	Temp = (DHT_Data[0] << 8) | DHT_Data[1];
	
	return (float)(Temp / 10.0);
}

float DHT22_Temperature(void){
	int Temp;
	Temp = (DHT_Data[2] << 8) | DHT_Data[3];
	if((DHT_Data[2] & 0x80) == 0){
		return (float)(Temp / 10.0);
	} else {
		Temp = Temp & 0x7FFF;
		return (float)(Temp / -10.0);
	}
}