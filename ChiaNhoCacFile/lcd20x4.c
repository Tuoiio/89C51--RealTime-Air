
/**
  ******************************************************************************
  * @file		LCD20X4.c
  * @author	Duong Xuan Tuoi
  * @date		25/06/2019
  ******************************************************************************
  */
#include "LCD20X4.h"
/*******************************************************************************
Noi Dung      :   Gui tin hieu Enable den LCD20X4.
Tham Bien     :   Khong.
Tra Ve        :   Khong.
*******************************************************************************/
 void LCD20X4_Enable(void)
{
    LCD_EN = 1;//cho chan EN len muc cao
    delay_us(3);
		LCD_EN = 0;//cho chân EN xuong muc thap
    delay_us(50);
}

/*******************************************************************************
Noi Dung      :   Ham goi 1 byte ra LCD20X4.
Tham Bien     :   data: du liwu can goi, reg: chon thanh ghi du lieu hay thanh ghi lenh
Tra Ve        :   Khong.
*******************************************************************************/
 void LCD20X4_Send4Bit (unsigned char Data )
{
		LCD_D4 =  Data &    0x01; //=>0x01=0b00000001=> goi bit dau tien
		LCD_D5 = (Data >> 1) & 1;//=>dich sang phai 1 bit roi xuat ra bit thu 2
		LCD_D6 = (Data >> 2) & 1;//=>dich sang phai 1 bit roi xuat ra bit thu 3
		LCD_D7 = (Data >> 3) & 1;//=>dich sang phai 1 bit roi xuat ra bit thu 4
}
 /*******************************************************************************
Noi Dung    :   Gui 1 byte du lieu den LCD.
Tham Bien   :   command: byte du lieu can goi; reg chon thanh ghi lenh hay du lieu
Tra Ve      :   Khong.
********************************************************************************/

void LCD20X4_SendCommand (unsigned char  Command,unsigned char reg ){
	 if(reg == 0)	LCD_RS = 0;	// chon thanh ghi lenh
   else					LCD_RS = 1;		// chon thanh ghi du lieu
  
		LCD20X4_Send4Bit(Command >> 4);   /* Gui 4 bit cao */
		LCD20X4_Enable();
		LCD20X4_Send4Bit(Command);      /* Gui 4 bit thap*/
		LCD20X4_Enable();
}


/*******************************************************************************
Noi Dung     :   Xoa noi dung hien thi tren LCD20X4.
Tham Bien    :   Khong.
Tra Ve       :   Khong.
********************************************************************************/
void LCD20X4_Clear(){
    LCD20X4_SendCommand(0x01,0);// goi lenh dieu khien LCD
    delay_ms(5);
}
/*******************************************************************************
Noi Dung     :   Khoi tao LCD20X4.
Tham Bien    :   Khong.
Tra Ve       :   Khong.
*******************************************************************************/
 void LCD20X4_Init ( void )
{
    LCD_RS = 0;
	  LCD_RW = 0;
    LCD_EN = 0;
    LCD20X4_Send4Bit(0x03);// goi du lieu cho LCD20X4 kieu mat dinh, dua con tro ve dau dong dau tin
    LCD20X4_Enable();
    delay_us(5);
    LCD20X4_Send4Bit(0x02);// goi du lieu cho LCD20X4 kieu mat dinh, dua con tro ve dau dong dau tin
    LCD20X4_Enable();
    delay_us(5);
    LCD20X4_SendCommand(0x28,0);// ghi lenh: giao thuc 4 bit, hien thi 2 hang, ki tu 5x7
    LCD20X4_SendCommand(0x0C,0);// ghi lenh: cho phep hien thi man hinh
    LCD20X4_Clear();           // xoa toan bo khung hinh
}
/*******************************************************************************
Noi Dung     :   Thiet lap vi tri con tro LCD20X4.
Tham Bien    :   x: vi tri cot cua con tro. x = 0 -> 15.
                 y: vi tri hang cua con tro. y= 0,1.
Tra Ve       :   Khong.
********************************************************************************/
void LCD20X4_Gotoxy(unsigned char x, unsigned char y){
  unsigned char  Address;
	switch (y){
		case 0:{ Address = 0x00; break; } //Starting address of 0st line
		case 1:{ Address = 0x40; break; }//Starting address of 1st line
		case 2:{ Address = 0x14; break; }//Starting address of 2st line
		case 3:{ Address = 0x54; break; }//Starting address of 3st line
	}
	Address = Address + x;
  LCD20X4_SendCommand(0x80 | Address, 0);//ghi lenh dieu khien LCD
}


/*******************************************************************************
Noi Dung    :   Viet 1 ki tu len LCD20X4.
Tham Bien   :   Data  :   Gia tri ki tu can hien thi.
Tra Ve      :   Khong.
********************************************************************************/
 void LCD20X4_PutChar (unsigned char Data ){
    LCD20X4_SendCommand(Data,1);// ghi du lieu
}
/*******************************************************************************
Noi Dung    :   Viet 1 chuoi ki tu len LCD20X4.
Tham Bien   :   *s   :  chuoi du lieu can hien thi.
Tra Ve      :   Khong.
********************************************************************************/
void LCD20X4_PutString (unsigned char *s){
    while (*s){
      LCD20X4_PutChar(*s);
      s++;
    }
}

/*******************************************************************************
Noi Dung    :   Hin thi so len LCD20X4.
Tham Bien   :   number  : so can hiren thi, leng la chieu dai cau chuoi so
Tra Ve      :   Khong.
********************************************************************************/

void LCD20X4_SendInteger(int iNumber){
	char i, CacKyTu[10];
	if(iNumber != 0){
		if(iNumber < 0) LCD20X4_SendCommand(0x2D, 1);  // dau tru
		for(i = 0; iNumber != 0; i++){
			if(iNumber > 0)
				CacKyTu[i] = iNumber  % 10;
			else
				CacKyTu[i] = (iNumber * -1) % 10;
			iNumber = iNumber / 10;
		}
		while(i > 0){
			LCD20X4_SendCommand(CacKyTu[i - 1] | 0x30, 1);
			i--;
		}
	}
	else{
		LCD20X4_SendCommand(0 + 0x30, 1);
	}
}

void LCD20X4_SendFloat(float fNumber){
	int a, b;
	a = (int)fNumber;
	if (fNumber >= 0.0) b = (fNumber - a) * 10;
	else{
		b = (fNumber - a) * (-10);
		if(fNumber <= 0.0 && fNumber > -1.0)
			LCD20X4_SendCommand(0x2D,1);
	}
	LCD20X4_SendInteger(a);
	if(b >= 0){
		LCD20X4_SendCommand(0x2E,1);
		LCD20X4_SendInteger(b);
	}
}

void LCD20X4_CreateChar(unsigned char location, unsigned char charmap[]){
	char x;
	location &= 0x07;
	LCD20X4_SendCommand(0x40 | (location << 3), 0);
	for(x = 0; x < 8; x++){
		LCD20X4_SendCommand(charmap[x], 1);
	}
}
