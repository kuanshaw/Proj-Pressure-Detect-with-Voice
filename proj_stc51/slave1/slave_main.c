#include "reg52.h"
#include "Allhead.h"
#include  <INTRINS.H> //Keil library  
#define   uchar unsigned char
#define   uint unsigned int	


#define	BMP085_SlaveAddress   0xee	  //¶¨ÒåÆ÷¼þÔÚIIC×ÜÏßÖÐµÄ´ÓµØÖ·                               

#define OSS 0	// Oversampling Setting (note: code is not set up to use other OSS values)
							   
typedef unsigned char  BYTE;
typedef unsigned short WORD;


sbit	  SCL=P3^3;      //IICÊ±ÖÓÒý½Å¶¨Òå
sbit 	  SDA=P3^2;      //IICÊý¾ÝÒý½Å¶¨Òå

   	
uchar ge,shi,bai,qian,wan,shiwan;           //ÏÔÊ¾±äÁ¿
int  dis_data;                              //±äÁ¿
uchar TxDate[5];

short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;
typedef bit BOOL  ; 
uchar data_byte;


void conversion(long temp_data);
uchar Single_Read(uchar REG_Address);                                      //µ¥¸ö¶ÁÈ¡ÄÚ²¿¼Ä´æÆ÷Êý¾Ý
bit  BMP085_RecvACK();
BYTE BMP085_RecvByte();




//*********************************************************
void conversion(long temp_data)  
{  
    
    shiwan=temp_data/100000+0x30 ;
    temp_data=temp_data%100000;   //È¡ÓàÔËËã 
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //È¡ÓàÔËËã
	qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //È¡ÓàÔËËã
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //È¡ÓàÔËËã
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //È¡ÓàÔËËã
    ge=temp_data+0x30; 	
}


/**************************************
ÑÓÊ±5Î¢Ãë(STC90C52RC@12M)
²»Í¬µÄ¹¤×÷»·¾³,ÐèÒªµ÷Õû´Ëº¯Êý£¬×¢ÒâÊ±ÖÓ¹ý¿ìÊ±ÐèÒªÐÞ¸Ä
µ±¸ÄÓÃ1TµÄMCUÊ±,Çëµ÷Õû´ËÑÓÊ±º¯Êý
**************************************/
void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}

/**************************************
ÑÓÊ±5ºÁÃë(STC90C52RC@12M)
²»Í¬µÄ¹¤×÷»·¾³,ÐèÒªµ÷Õû´Ëº¯Êý
µ±¸ÄÓÃ1TµÄMCUÊ±,Çëµ÷Õû´ËÑÓÊ±º¯Êý
**************************************/
void Delay5ms()
{
    WORD n = 560;

    while (n--);
}

/**************************************
ÆðÊ¼ÐÅºÅ
**************************************/
void BMP085_Start()
{
    SDA = 1;                    //À­¸ßÊý¾ÝÏß
    SCL = 1;                    //À­¸ßÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±
    SDA = 0;                    //²úÉúÏÂ½µÑØ
    Delay5us();                 //ÑÓÊ±
    SCL = 0;                    //À­µÍÊ±ÖÓÏß
}

/**************************************
Í£Ö¹ÐÅºÅ
**************************************/
void BMP085_Stop()
{
    SDA = 0;                    //À­µÍÊý¾ÝÏß
    SCL = 1;                    //À­¸ßÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±
    Delay5us();                 //ÑÓÊ±
}

/**************************************
·¢ËÍÓ¦´ðÐÅºÅ
Èë¿Ú²ÎÊý:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(bit ack)
{
    SDA = ack;                  //Ð´Ó¦´ðÐÅºÅ
    SCL = 1;                    //À­¸ßÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±
    SCL = 0;                    //À­µÍÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±
}

/**************************************
½ÓÊÕÓ¦´ðÐÅºÅ
**************************************/
bit BMP085_RecvACK()
{
    SCL = 1;                    //À­¸ßÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±
    CY = SDA;                   //¶ÁÓ¦´ðÐÅºÅ
    SCL = 0;                    //À­µÍÊ±ÖÓÏß
    Delay5us();                 //ÑÓÊ±

    return CY;
}

/**************************************
ÏòIIC×ÜÏß·¢ËÍÒ»¸ö×Ö½ÚÊý¾Ý
**************************************/
void BMP085_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8Î»¼ÆÊýÆ÷
    {
        dat <<= 1;              //ÒÆ³öÊý¾ÝµÄ×î¸ßÎ»
        SDA = CY;               //ËÍÊý¾Ý¿Ú
        SCL = 1;                //À­¸ßÊ±ÖÓÏß
        Delay5us();             //ÑÓÊ±
        SCL = 0;                //À­µÍÊ±ÖÓÏß
        Delay5us();             //ÑÓÊ±
    }
    BMP085_RecvACK();
}

/**************************************
´ÓIIC×ÜÏß½ÓÊÕÒ»¸ö×Ö½ÚÊý¾Ý
**************************************/
BYTE BMP085_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //Ê¹ÄÜÄÚ²¿ÉÏÀ­,×¼±¸¶ÁÈ¡Êý¾Ý,
    for (i=0; i<8; i++)         //8Î»¼ÆÊýÆ÷
    {
        dat <<= 1;
        SCL = 1;                //À­¸ßÊ±ÖÓÏß
        Delay5us();             //ÑÓÊ±
        dat |= SDA;             //¶ÁÊý¾Ý               
        SCL = 0;                //À­µÍÊ±ÖÓÏß
        Delay5us();             //ÑÓÊ±
    }
    return dat;
}

//*********************************************************
//¶Á³öBMP085ÄÚ²¿Êý¾Ý,Á¬ÐøÁ½¸ö
//*********************************************************
short Multiple_read(uchar ST_Address)
{   
	uchar msb, lsb;
	short _data;
    msb = BMP085_RecvByte();                 //BUF[0]´æ´¢
    BMP085_SendACK(0);                       //»ØÓ¦ACK
    lsb = BMP085_RecvByte();     
	BMP085_SendACK(1);                       //×îºóÒ»¸öÊý¾ÝÐèÒª»ØNOACK

    BMP085_Stop();                           //Í£Ö¹ÐÅºÅ
    Delay5ms();
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{

    BMP085_Start();                  //ÆðÊ¼ÐÅºÅ
    BMP085_SendByte(BMP085_SlaveAddress);   //·¢ËÍÉè±¸µØÖ·+Ð´ÐÅºÅ
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //·¢ËÍÍ£Ö¹ÐÅºÅ
	Delay(10);	// max time is 4.5ms
	
	return (long) Multiple_read(0xF6);
}
//*************************************************************
long bmp085ReadPressure(void)
{
	long pressure = 0;

    BMP085_Start();                   //ÆðÊ¼ÐÅºÅ
    BMP085_SendByte(BMP085_SlaveAddress);   //·¢ËÍÉè±¸µØÖ·+Ð´ÐÅºÅ
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34);       	  // write register data for pressure
    BMP085_Stop();                    //·¢ËÍÍ£Ö¹ÐÅºÅ
	Delay(10);    	                  // max time is 4.5ms
	
	pressure = Multiple_read(0xF6);
	pressure &= 0x0000FFFF;
	
	return pressure;	
	//return (long) bmp085ReadShort(0xF6);
}

//**************************************************************

//³õÊ¼»¯BMP085£¬¸ù¾ÝÐèÒªÇë²Î¿¼pdf½øÐÐÐÞ¸Ä**************
void Init_BMP085()
{
	ac1 = Multiple_read(0xAA);
	ac2 = Multiple_read(0xAC);
	ac3 = Multiple_read(0xAE);
	ac4 = Multiple_read(0xB0);
	ac5 = Multiple_read(0xB2);
	ac6 = Multiple_read(0xB4);
	b1 =  Multiple_read(0xB6);
	b2 =  Multiple_read(0xB8);
	mb =  Multiple_read(0xBA);
	mc =  Multiple_read(0xBC);
	md =  Multiple_read(0xBE);
}

//***********************************************************************
void bmp085Convert()
{
	long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
	long  temperature;
	long  pressure;

	ut = bmp085ReadTemp();
	ut = bmp085ReadTemp();	   // ¶ÁÈ¡ÎÂ¶È
	up = bmp085ReadPressure();
	up = bmp085ReadPressure();  // ¶ÁÈ¡Ñ¹Ç¿

	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = (b5 + 8) >> 4;

	//*************

	conversion(temperature);

//	TxDate[0]=bai;       
//	TxDate[1]=shi; 
//	TxDate[2]=ge; 
	// display(0x09,0XDF);     //ÎÂ¶Èµ¥Î»

	//*************

	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	p = (b7 * 2) / b4 ;
	else  
	p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	pressure = p + ((x1 + x2 + 3791) >> 4);

	conversion(pressure);

	TxDate[0]=shiwan+'0'; 
	TxDate[1]=wan+'0';    
	TxDate[2]=qian+'0'; 
	TxDate[3]=bai+'0'; 
	TxDate[4]=shi+'0'; 
}



/*********************Ö÷º¯Êý*************************/

void main()
{
	NRF24L01Int();
	Init_BMP085();                //³õÊ¼»¯BMP085 

	while(1)
	{
		bmp085Convert();

//		TxDate[0]='0'; 
//		TxDate[1]='9';    
//		TxDate[2]='9'; 
//		TxDate[3]='1'; 
//		TxDate[4]='1'; 

		NRFSetTxMode(TxDate);
		while(CheckACK());	//¼ì²âÊÇ·ñ·¢ËÍÍê±Ï
	}
}


