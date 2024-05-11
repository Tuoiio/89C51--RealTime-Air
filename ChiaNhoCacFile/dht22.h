#ifndef _DHT22_H_
#define _DHT22_H_


#include <delay.h>
#include <regx51.h>

void delay_30us(void);
//-------------------------------------Request----------------------------------//(Gui yeu cau cho DHT22)

static void Request(void);

//----------------------------------Check Bit-----------------------------------// (Kiem tra bit)

 static void Response(void);


bit DHT22_Read(void);

float DHT22_Humidity(void);

float DHT22_Temperature(void);

#endif
