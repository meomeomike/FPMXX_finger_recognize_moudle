// Pin 13 has an LED connected on most Arduino boards.
//int led = 13;
int TEST = 4;           
int DIRECTION = 5;      //which connect with direction relay
int BUZZER = 6;
int RELAY = 7;          //which connect to power relay

//LCD_PINS
int LCD_CS = 8;         //LCD选中引脚,低电平有效
int LCD_RST = 9;        //LCD复位引脚,低电平有效
int LCD_RS = 12;        //LCD寄存器选择引脚, 为1时候选择的数据寄存器，为0时是控制寄存器
int LCD_SCK = 11;       //LCD串行时钟接口
int LCD_MOSI = 10;      //LCD串行数据写入接口

int KEY_OK = A3;
int KEY_DOWN = A1;
int KEY_UP = A2;
int KEY_CANCEL = A0;

void LCD_Write_Data(int data_temp);
void LCD_Reset();
void LCD_Fill(unsigned char Row, unsigned char Column, unsigned char length, unsigned char Fill_data);
void LCD_Write_Address(unsigned char Row, unsigned char Column);
void LCD_Pins_Init();
void LCD_Write_Font16x8(unsigned char Row, unsigned char Column, unsigned char* Font, unsigned char black);
void LCD_Write_Font16x16(unsigned char Row, unsigned char Column, unsigned char* Font);
void LCD_Write_Cmd(int data_tmp);
void LCD_Write_Data(int data_temp);
void LCD_Init();
void LCD_Clear_Screen();

