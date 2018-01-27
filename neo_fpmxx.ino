#include <SoftwareSerial.h>
#include "./neo_fpmxx.h"

SoftwareSerial mySerial(3, 2);

volatile unsigned char FPMXX_RECEIVE_BUFFER[64]; 
unsigned char FPMXX_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
unsigned char FPMXX_Get_Img[6] = {0x01,0x00,0x03,0x01,0x0,0x05};    //获得指纹图像
unsigned char FPMXX_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
unsigned char FPMXX_Search[11]={0x01,0x0,0x08,0x04,0x01,0x0,0x0,0x03,0xE7,0x0,0xF8}; //搜索指纹搜索范围0 - 256,使用BUFFER1中的特征码搜索
unsigned char FPMXX_Get_Device[6] = {0x01,0x00,0x03,0x16,0x00,0x1A};
unsigned char FPMXX_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
unsigned char FPMXX_Img_To_Buffer1[7]={0x01,0x0,0x04,0x02,0x01,0x0,0x08}; //将图像放入到BUFFER1
unsigned char FPMXX_Img_To_Buffer2[7]={0x01,0x0,0x04,0x02,0x02,0x0,0x09}; //将图像放入到BUFFER2
unsigned char FPMXX_Reg_Model[6]={0x01,0x0,0x03,0x05,0x0,0x09}; //将BUFFER1跟BUFFER2合成特征模版
unsigned char FPMXX_Delete_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版

void FPMXX_Cmd_Receive_Data(unsigned int r_size)
{
    Serial.println("FPMXX_Cmd_Receive_Data:start");
    while(1){
         Serial.println("FPMXX_Cmd_Receive_Data:into while");
         Serial.print("FPMXX_Cmd_Receive_Data:mySerial.available()=");
         Serial.print(mySerial.available());
         Serial.print("\n");
         if(mySerial.available()==r_size){
             for(int i=0;i<r_size;i++){
                 FPMXX_RECEIVE_BUFFER[i] = mySerial.read();
                 Serial.println("FPMXX_Cmd_Receive_Data:read a data");
             }
             break;
         }
         delay(50);
    }
}
void FPMXX_Send_Cmd(unsigned char length,unsigned char *address,unsigned char returnLength) 
{ 
    unsigned char i = 0;
    mySerial.flush();
    for(i=0;i<6;i++){ 
        mySerial.write(FPMXX_Pack_Head[i]); //包头
    }
    Serial.println("FPMXX_Send_Cmd:sent package head");
    for(i=0;i<length;i++){	        
	    mySerial.write(*address);    //命令    
  	    address++;
    }
    Serial.println("FPMXX_Send_Cmd:sent package");
    FPMXX_Cmd_Receive_Data(returnLength);
}
void FPMXX_Cmd_StoreTemplate(unsigned int ID)//存储指纹
{
    unsigned int temp = 0;
    FPMXX_Save_Finger[5] =(ID&0xFF00)>>8;
    FPMXX_Save_Finger[6] = (ID&0x00FF);
           
    for(int i=0;i<7;i++)   //计算校验和
        temp = temp + FPMXX_Save_Finger[i];
          
    FPMXX_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
    FPMXX_Save_Finger[8]= temp & 0x0000FF;
    mySerial.write(FPMXX_Pack_Head,6);  
    mySerial.write(FPMXX_Save_Finger,9);  
    FPMXX_Cmd_Receive_Data(12);
}
void FPMXX_Add_Fingerprint(unsigned int writeID)//添加指纹
{
    Serial.println("FPMXX_Add_Fingerprint:start");
    while(1){
        Serial.println("FPMXX_Add_Fingerprint:try to get finger1");
        FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
        Serial.println("FPMXX_Add_Fingerprint:try to get finger1,step2");
		//判断接收到的确认码,等于0指纹获取成功
		if(FPMXX_RECEIVE_BUFFER[9]==0){
			delay(100);
			FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer1,12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
			Serial.println("FPMXX_Add_Fingerprint:got finger1");
			break;
	    }
	}     
    while(1){
        FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
	    //判断接收到的确认码,等于0指纹获取成功 
		if(FPMXX_RECEIVE_BUFFER[9]==0){
            delay(100);
            FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer2,12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer2
            Serial.println("FPMXX_Add_Fingerprint:got finger2");
            FPMXX_Cmd_StoreTemplate(writeID);
            Serial.println("FPMXX_Add_Fingerprint:success");
            break;
        }
    }
    return 0;
}
void FPMXX_Find_Fingerprint()//搜索指纹
{
    unsigned int find_finger_id = 0;
    while (1){
    	FPMXX_Send_Cmd(6,FPMXX_Get_Img,12);
	    //判断接收到的确认码,等于0指纹获取成功
        if(FPMXX_RECEIVE_BUFFER[9]==0){
            delay(10);
            FPMXX_Send_Cmd(7,FPMXX_Img_To_Buffer1,12);
	        FPMXX_Send_Cmd(11,FPMXX_Search,16);
	        if(FPMXX_RECEIVE_BUFFER[9] == 0){ //搜索到
                find_finger_id = FPMXX_RECEIVE_BUFFER[10]*256 + FPMXX_RECEIVE_BUFFER[11];
                Serial.println("get");
                //Iopen();//自己写的开锁函数
            }
        }
    }
    
}
void FPMXX_Delete_All_Fingerprint()
{
    FPMXX_Send_Cmd(6,FPMXX_Delete_All_Model,12); 
}
void setup()
{
    mySerial.begin(57600);
    Serial.begin(9600);
    Serial.println("ready?");
}
void loop()
{
    for(int i=0;i<3;i++){
        delay(1000);
        Serial.println(i);
    }
    Serial.println("go!");
    
    FPMXX_Add_Fingerprint(10);
    delay(10000);
}
