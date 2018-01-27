#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include "./etcs.h"
#include "./lcd.h"

SoftwareSerial mySerial(3, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

volatile unsigned char FPMXX_RECEIVE_BUFFER[64]; 

unsigned char FPMXX_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
unsigned char FPMXX_Get_Img[6] = {0x01,0x00,0x03,0x01,0x0,0x05};    //获得指纹图像
unsigned char FPMXX_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
unsigned char FPMXX_Search[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x03,0xE7,0x0,0xF8}; //搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
unsigned char FPMXX_Get_Device[6] = {0x01,0x00,0x03,0x16,0x00,0x1A};
unsigned char FPMXX_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
unsigned char FPMXX_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08}; //将图像放入到BUFFER1
unsigned char FPMXX_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09}; //将图像放入到BUFFER2
unsigned char FPMXX_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09}; //将BUFFER1跟BUFFER2合成特征模版
unsigned char FPMXX_Delete_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版


volatile unsigned char Menu_Max_Item = 3;
volatile unsigned char Menu_Select = 1;


void FPMXX_Cmd_Save_Finger( unsigned int storeID )
{
    unsigned long temp = 0;

    FPMXX_Save_Finger[5] =(storeID&0xFF00)>>8;
    FPMXX_Save_Finger[6] = (storeID&0x00FF);
           
	 for(char i=0;i<7;i++)   //计算校验和
	    temp = temp + FPMXX_Save_Finger[i];
			    
	 FPMXX_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
	 FPMXX_Save_Finger[8]= temp & 0x0000FF;
		   
   FPMXX_Send_Cmd(9,FPMXX_Save_Finger,12);
	
}


void LCD_Main_Menu()
{
    LCD_Clear_Screen();
    //选中箭头
    LCD_Write_Font16x16(2, 0, HZ_select_arrow);

    //添加指纹
    LCD_Write_Font16x16(4, 16, HZ_tian);
    LCD_Write_Font16x16(4, 32, HZ_jia);
    LCD_Write_Font16x16(4, 48, HZ_zhi);
    LCD_Write_Font16x16(4, 64, HZ_wen);

    //搜索指纹
    LCD_Write_Font16x16(2, 16, HZ_sou);
    LCD_Write_Font16x16(2, 32, HZ_suo);
    LCD_Write_Font16x16(2, 48, HZ_zhi);
    LCD_Write_Font16x16(2, 64, HZ_wen);

    //清空指纹库
    LCD_Write_Font16x16(6, 16, HZ_qing);
    LCD_Write_Font16x16(6, 32, HZ_kong);
    LCD_Write_Font16x16(6, 48, HZ_zhi);
    LCD_Write_Font16x16(6, 64, HZ_wen);
    LCD_Write_Font16x16(6, 80, HZ_ku);

}

void LCD_Main_Menu_Select(unsigned char select)
{

    LCD_Fill(2, 0, 16, 0x0);
    LCD_Fill(3, 0, 16, 0x0);
    LCD_Fill(4, 0, 16, 0x0);
    LCD_Fill(5, 0, 16, 0x0);
    LCD_Fill(6, 0, 16, 0x0);
    LCD_Fill(7, 0, 16, 0x0);

    switch (select)
    {
        case 1:
            //选中箭头
            LCD_Write_Font16x16(2, 0, HZ_select_arrow);
            break;

        case 2:
            //选中箭头
            LCD_Write_Font16x16(4, 0, HZ_select_arrow);
            break;

        case 3:
            //选中箭头
            LCD_Write_Font16x16(6, 0, HZ_select_arrow);
            break;
            
        default:
            break;
    }
}
void LCD_Select_SZ(unsigned char row, unsigned start, unsigned char sz, unsigned char black)
{
    switch (sz)
    {
        case 0:
            LCD_Write_Font16x8(row, start, SZ_0, black);
            break;

        case 1:
            LCD_Write_Font16x8(row, start, SZ_1, black);
            break;

        case 2:
            LCD_Write_Font16x8(row, start, SZ_2, black);
            break;

        case 3:
            LCD_Write_Font16x8(row, start, SZ_3, black);
            break;

        case 4:
            LCD_Write_Font16x8(row, start, SZ_4, black);
            break;

        case 5:
            LCD_Write_Font16x8(row, start, SZ_5, black);
            break;

        case 6:
            LCD_Write_Font16x8(row, start, SZ_6, black);
            break;

        case 7:
            LCD_Write_Font16x8(row, start, SZ_7, black);
            break;

        case 8:
            LCD_Write_Font16x8(row, start, SZ_8, black);
            break;

        case 9:
            LCD_Write_Font16x8(row, start, SZ_9, black);
            break;

    }
}


void LCD_Wirte_SZ(unsigned char row, unsigned char start, unsigned int sz)
{
    // switch(sz/100)
    LCD_Select_SZ(row, start, (sz / 100), 1);
    LCD_Select_SZ(row, start + 8, ((sz % 100) / 10), 1);
    LCD_Select_SZ(row, start + 16, (sz % 10), 1);

}

void LED_Init()
{
    pinMode(TEST, INPUT);
    pinMode(DIRECTION, OUTPUT);
}

void testDIRECTION()
{
    digitalWrite(DIRECTION, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);
    digitalWrite(DIRECTION, LOW);   // turn the LED on (HIGH is the voltage level)
}

void RELAY_Init()
{
    pinMode(RELAY, OUTPUT);
    //digitalWrite(RELAY, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(RELAY, LOW);
}

void KEY_Init()
{
    digitalWrite(KEY_UP, HIGH);
    pinMode(KEY_UP, OUTPUT);

    digitalWrite(KEY_DOWN, HIGH);
    pinMode(KEY_DOWN, OUTPUT);

    digitalWrite(KEY_CANCEL, HIGH);
    pinMode(KEY_CANCEL, OUTPUT);

    digitalWrite(KEY_OK, HIGH);
    pinMode(KEY_OK, OUTPUT);
}


//指纹发送指令
void FPMXX_Send_Cmd(unsigned char length,unsigned char *address,unsigned char returnLength) 
{ 
    unsigned char i = 0;
    mySerial.flush();
   
    for(i=0;i<6;i++) //包头{
        mySerial.write(FPMXX_Pack_Head[i]);
    }
	
    for(i=0;i<length;i++){	        
	    mySerial.write(*address);        
  	address++;
    }		
	
    FPMXX_Cmd_Receive_Data(returnLength);
 
   /*
   for(i=0;i<returnLength;i++)
   {
        Serial.write(FPMXX_RECEIVE_BUFFER[i]);
   }
   */
   	
}

//fingerprint modules cmd

void FPMXX_Delete_All_Fingerprint()
{

    LCD_Clear_Screen(); //清空显示屏

    //清空指纹库
    //  LCD_Write_Font16x16(1,16,HZ_qing); 
    //  LCD_Write_Font16x16(1,32,HZ_kong);
    //  LCD_Write_Font16x16(1,48,HZ_zhi);
    //  LCD_Write_Font16x16(1,64,HZ_wen);
    //  LCD_Write_Font16x16(1,80,HZ_ku);
    //  LCD_Write_Font16x16(1,96,HZ_Dot);
    //  LCD_Write_Font16x16(1,104,HZ_Dot);

    //添加清空指纹指令

    //
     FPMXX_Send_Cmd(6,FPMXX_Delete_All_Model,12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
    //FPMXX_Receive_Data(12); 



  //  finger.emptyDatabase();



    tone(BUZZER,440,80);;
    delay(500);
    tone(BUZZER,440,80);;

    LCD_Write_Font16x16(3, 16, HZ_qing);
    LCD_Write_Font16x16(3, 32, HZ_kong);
    LCD_Write_Font16x16(3, 48, HZ_Cheng);
    LCD_Write_Font16x16(3, 64, HZ_Gong);

    delay(2000);
    LCD_Main_Menu();
    Menu_Select = 1;

}


void lcd_input_finger(unsigned char index)
{
    switch (index)
    {
        case 1:
            LCD_Write_Font16x16(0, 48 - 16, HZ_zhi);
            LCD_Write_Font16x16(0, 64 - 16, HZ_wen);
            LCD_Write_Font16x8(0, 80 - 16, ZM_I, 0);
            LCD_Write_Font16x8(0, 88 - 16, ZM_D, 0);
            break;

        case 2:
            LCD_Write_Font16x16(0, 16 - 16, HZ_Qing3);
            LCD_Write_Font16x16(0, 32 - 16, HZ_An);
            LCD_Write_Font16x16(0, 48 - 16, HZ_Shou);
            LCD_Write_Font16x16(0, 64 - 16, HZ_zhi);
            LCD_Write_Font16x8(0, 64, SZ_1, 0);
            break;
        case 3:
            LCD_Write_Font16x16(0, 16 - 16, HZ_Qing3);
            LCD_Write_Font16x16(0, 32 - 16, HZ_An);
            LCD_Write_Font16x16(0, 48 - 16, HZ_Shou);
            LCD_Write_Font16x16(0, 64 - 16, HZ_zhi);
            LCD_Write_Font16x8(0, 64, SZ_2, 0);
            break;
        default:
            break;
    }
}





void FPMXX_Cmd_Receive_Data(unsigned int r_size)
{
    while(1){
         if(mySerial.available()==r_size){
             for(int i=0;i<r_size;i++){
                 FPMXX_RECEIVE_BUFFER[i] = mySerial.read();
             }
             break;
         } 
    }
}
  
void FPMXX_Cmd_StoreTemplate(unsigned int ID)
{
   unsigned int temp = 0;
  
           FPMXX_Save_Finger[5] =(ID&0xFF00)>>8;
           FPMXX_Save_Finger[6] = (ID&0x00FF);
           
       for(int i=0;i<7;i++)   //计算校验和
           temp = temp + FPMXX_Save_Finger[i];
          
       FPMXX_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
       FPMXX_Save_Finger[8]= temp & 0x0000FF;
  
  
//mySerial.print(0xEF, HEX);
  mySerial.write(FPMXX_Pack_Head,6);  
  mySerial.write(FPMXX_Save_Finger,9);  
  
 FPMXX_Cmd_Receive_Data(12);
}





void FPMXX_Add_Fingerprint()
{
    unsigned int  writeID = 0;

    LCD_Clear_Screen(); //清空显示屏
    lcd_input_finger(1);

    LCD_Write_Font16x8(3, 48, SZ_0, 1);
    LCD_Write_Font16x8(3, 48 + 8, SZ_0, 1);
    LCD_Write_Font16x8(3, 48 + 8 + 8, SZ_0, 1);

    delay(100);

    int key_status = 100;

    while (1){
        //BUTTON_CANCEL
        key_status = digitalRead(KEY_CANCEL);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_CANCEL);
                if (key_status != LOW){
                    break;
                }
            }
RETURN_MENU:
            delay(100);
            LCD_Main_Menu();
            Menu_Select = 1;
            break;
        }
        //KEY_OK
        key_status = digitalRead(KEY_OK);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_OK);
                if (key_status != LOW){
                    break;
                }
            }
            //sources
            LCD_Clear_Screen(); //清空显示屏                     
            lcd_input_finger(2);
            key_status = 1000;
            while(1){
            FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
		        //判断接收到的确认码,等于0指纹获取成功    FPMXX_Send_Cmd    FPMXX_Send_Cmd    FPMXX_Send_Cmd    FPMXX_Send_Cmd
		        if(FPMXX_RECEIVE_BUFFER[9]==0){
			          tone(BUZZER,440,80);
			          delay(100);
			          FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer1,12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
                        
                LCD_Clear_Screen(); //清空显示屏                     
                lcd_input_finger(3);
                     
                while(1){
                    FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
	                  //判断接收到的确认码,等于0指纹获取成功    FPMXX_Send_Cmd    FPMXX_Send_Cmd    FPMXX_Send_Cmd    FPMXX_Send_Cmd
		                if(FPMXX_RECEIVE_BUFFER[9]==0){
                        tone(BUZZER,440,80);
                        delay(500);
                        FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer2,12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
                        FPMXX_Cmd_StoreTemplate(writeID);
                        LCD_Clear_Screen(); //清空显示屏
                      
                        LCD_Write_Font16x16(0,16-16,HZ_tian); 
                        LCD_Write_Font16x16(0,32-16,HZ_jia);
                        LCD_Write_Font16x16(0,48-16,HZ_Cheng);
                        LCD_Write_Font16x16(0,64-16,HZ_Gong);
                        delay(50);
                        goto RETURN_MENU;
                 }
             key_status = digitalRead(KEY_CANCEL);
             if(key_status == 0){
             while (1){
                key_status = digitalRead(KEY_CANCEL);
                if (key_status != LOW){
                    break;
                }
           }
           goto RETURN_MENU;
        }
   }
   }
    //back main menu
                
         //BUTTON_CANCEL
        key_status = digitalRead(KEY_CANCEL);

    if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_CANCEL);
                if (key_status != LOW){
                    break;
                }
            }

            //退回主要界面

          goto RETURN_MENU;
        }
 
        }
         }
        //BUTTON_UP
        key_status = digitalRead(KEY_UP);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_UP);
                if (key_status != LOW){
                    break;
                }
            }
            writeID = (writeID + 1)%1000;
            LCD_Wirte_SZ(3, 48, writeID);
        }
        //BUTTON_DOWN
        key_status = digitalRead(KEY_DOWN);
        if (key_status == LOW){
            while (1){
                key_status = digitalRead(KEY_DOWN);
                if (key_status != LOW) {
                    break;
                }
            }
            writeID = (writeID + 999)%1000;
            LCD_Wirte_SZ(3, 48, writeID);
        }
    }
}
/*自己写的开门程序*******************************************************************************************/
void Iopen()
{
    digitalWrite(RELAY, HIGH);
    delay(100);
    while(1){
        if(digitalRead(TEST)==HIGH){
            delay(200);
            digitalWrite(RELAY, LOW);
            break;
        }
    }
    digitalWrite(DIRECTION,HIGH);
    
    delay(2000);
    digitalWrite(RELAY, HIGH);
    delay(50);
    while(1){
        if(digitalRead(TEST)==HIGH){
            
            digitalWrite(DIRECTION,LOW);
            delay(40);
            digitalWrite(RELAY,LOW);
            break;
        }
    }
}
/**********************************************************************************************/
void FPMXX_Find_Fingerprint()
{
    unsigned int find_finger_id = 0;
    unsigned char address = 0;
    unsigned long time;
    time=millis();
    LCD_Clear_Screen(); //清空显示屏

    //搜索指纹
    LCD_Write_Font16x16(0, 16 - 16, HZ_sou);
    LCD_Write_Font16x16(0, 32 - 16, HZ_suo);
    LCD_Write_Font16x16(0, 48 - 16, HZ_zhi);
    LCD_Write_Font16x16(0, 64 - 16, HZ_wen);

    unsigned char key_status = 1000;
    
    while (1){
    	  FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
	      //判断接收到的确认码,等于0指纹获取成功
        if(FPMXX_RECEIVE_BUFFER[9]==0){
            delay(10);
            FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer1,12);
	          FPMXX_Send_Cmd(11,FPMXX_Search,16);
	          if(FPMXX_RECEIVE_BUFFER[9] == 0){ //搜索到
                LCD_Fill(2, 0, 128, 0x00);//清空改好
                LCD_Fill(3, 0, 128, 0x00);//清空改好
                find_finger_id = FPMXX_RECEIVE_BUFFER[10]*256 + FPMXX_RECEIVE_BUFFER[11];
                LCD_Wirte_SZ(2, 64,find_finger_id);
                Iopen();
                LCD_Clear_Screen(); //清空显示屏
                //搜索指纹
                LCD_Write_Font16x16(0, 16 - 16, HZ_sou);
                LCD_Write_Font16x16(0, 32 - 16, HZ_suo);
                LCD_Write_Font16x16(0, 48 - 16, HZ_zhi);
                LCD_Write_Font16x16(0, 64 - 16, HZ_wen);
            }
            else{
                LCD_Fill(2, 0, 128, 0x00);//清空改好
                LCD_Fill(3, 0, 128, 0x00);//清空改好
                LCD_Write_Font16x16(2, 0, HZ_Wei);
                LCD_Write_Font16x16(2, 16, HZ_Zhao);
                LCD_Write_Font16x16(2, 32, HZ_Dao);
                delay(10);//本来是2000
                LCD_Clear_Screen(); //清空显示屏
                //搜索指纹
                LCD_Write_Font16x16(0, 16 - 16, HZ_sou);
                LCD_Write_Font16x16(0, 32 - 16, HZ_suo);
                LCD_Write_Font16x16(0, 48 - 16, HZ_zhi);
                LCD_Write_Font16x16(0, 64 - 16, HZ_wen);
            }
        }
        key_status = digitalRead(KEY_CANCEL);
        if (key_status == LOW){
            while (1){
                key_status = digitalRead(KEY_DOWN);
                if (key_status != LOW){
                    break;
                }
            }
            LCD_Main_Menu();
            Menu_Select = 1;
            break;
        }
    }
    
}

// the setup routine runs once when you press reset:
void setup()
{
    tone(BUZZER,440,80);
    LED_Init();
    RELAY_Init();
    KEY_Init();

    testDIRECTION();

    LCD_Pins_Init();
    LCD_Init();
    LCD_Clear_Screen();
    LCD_Main_Menu();

    mySerial.begin(57600);
    Serial.begin(9600);
    
    
    delay(1000);
    
}
// the loop routine runs over and over again forever:
void loop()
{
    //  Serial.Write()
    /*
    while(1){
      FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
      delay(300);
    }*/
    int key_status = 100;
    while (1){
        //BUTTON_DOWN
        key_status = digitalRead(KEY_DOWN);
        if (key_status == LOW){
            while (1){
                key_status = digitalRead(KEY_DOWN);
                if (key_status != LOW){
                    break;
                }
            }
            if (Menu_Select != 1){
                Menu_Select--;
            }
            LCD_Main_Menu_Select(Menu_Select);
        }
        //BUTTON_UP
        key_status = digitalRead(KEY_UP);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_UP);
                if (key_status != LOW){
                    break;
                }
            }
            if (Menu_Select != Menu_Max_Item){
                Menu_Select++;
            }
            LCD_Main_Menu_Select(Menu_Select);
        }
        //BUTTON_CANCEL
        key_status = digitalRead(KEY_CANCEL);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_CANCEL);
                if (key_status != LOW){
                    break;
                }
            }
            finger.getImage();    //不知道在干什么？
        }
        //BUTTON_OK
        key_status = digitalRead(KEY_OK);
        if (key_status == 0){
            while (1){
                key_status = digitalRead(KEY_OK);
                if (key_status != LOW){
                    break;
                }
            }
            //add sources
            switch (Menu_Select){
                //查找指纹
                case 1:
                    FPMXX_Find_Fingerprint();
                    //LCD_Main_Menu();
                    //Menu_Select = 1;          
                    break;
                //添加指纹
                case 2:
                    FPMXX_Add_Fingerprint();
                    break;
                //清空指纹
                case 3:
                    FPMXX_Delete_All_Fingerprint();
                    break;
            }
            //sources end
        }
        delay(100);
    }
}
