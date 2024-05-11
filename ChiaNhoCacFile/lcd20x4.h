  /******************************************************************************
  * @file		lcd20x4.h
  * @author	Duong Xuan Tuoi
  * @date		05/02/2023
  ******************************************************************************/

#ifndef _LCD20X4_H
#define _LCD20X4_H

#include <regx51.h>
#include <delay.h>

//=========================khai bao cac chan vao ra============================//
//	DINH NGHIA CAC CHAN/PORT
sbit LCD_RS=P0^0;//khai bao chan RS cua LCD o P2.0
sbit LCD_RW=P0^1;//khai bao chan EN cua LCD o P2.1
sbit LCD_EN=P0^2;//khai bao chan EN cua LCD o P2.2

sbit LCD_D4=P0^4;//khai bao chan D4 cua LCD o P0.4
sbit LCD_D5=P0^5;//khai bao chan D4 cua LCD o P0.5
sbit LCD_D6=P0^6;//khai bao chan D4 cua LCD o P0.6
sbit LCD_D7=P0^7;//khai bao chan D4 cua LCD o P0.7

/*
	KHAI BÁO CÁC HÀM CHO LCD
*/
void LCD20X4_Init ( void );      								//ham khoi tao LCD20X4
void LCD20X4_Enable(void);       								//ham tao xung EN
void LCD20X4_Clear(void);            						//xoa noi dung hien thi tren LCD20X4
void LCD20X4_Send4Bit(unsigned char Data );						//ham goi du lieu 4 bit den LCD20X4
void LCD20X4_SendCommand (unsigned char  Command,unsigned char reg );//Goi 1 byte du lieu den LCD
void LCD20X4_Gotoxy(unsigned char  x, unsigned char y);			//ham thiet lap vi tri con tro
void LCD20X4_PutChar ( unsigned char  Data );					//ham viet mot ky tu len LCD20X4
void LCD20X4_PutString (unsigned char *String); 						// hien thi chuoi ky tu ra LCD     
void LCD20X4_SendInteger(int iNumber);
void LCD20X4_SendFloat(float fNumber);
void LCD20X4_CreateChar(unsigned char location, unsigned char charmap[]);
#endif