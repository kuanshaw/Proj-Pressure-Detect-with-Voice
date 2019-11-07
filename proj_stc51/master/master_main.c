#include "reg52.h"
#include "Allhead.h"
#include "intrins.h"

sbit K1 = P3^2;			 			//�˵�
sbit K2 = P3^3;  					//��
sbit K3 = P3^4;  					//��
sbit AUDIO_BUSY = P3^5;		//��ȡ����ģ���BUSY���ţ�����Ƿ����ڲ���

unchar yemian=0;
unchar num=0;
unchar flag=0,a,b,c;
unchar SJqiya1,SJqiya2;					//ʵ�����������ѹ
unchar gaoya = PRESSURE_H, diya = PRESSURE_L;  	//gaoya������ѹ���ޣ�diya������ѹ���ޡ�

// ����LCD��ʾ
unchar lcdgaoya[5],lcddiya[5];

// ��������ָ���б�
unchar audio_left_low[]   				  = {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4, 0xFF};
unchar audio_left_high[]  				  = {0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5, 0xFF};
unchar audio_right_low[]  					= {0xAA, 0x07, 0x02, 0x00, 0x03, 0xB6, 0xFF};
unchar audio_right_high[] 					= {0xAA, 0x07, 0x02, 0x00, 0x04, 0xB7, 0xFF};
unchar audio_left_low_right_low[]   = {0xAA, 0x07, 0x02, 0x00, 0x05, 0xB8, 0xFF};
unchar audio_left_high_right_high[] = {0xAA, 0x07, 0x02, 0x00, 0x06, 0xB9, 0xFF};
unchar audio_left_high_right_low[]  = {0xAA, 0x07, 0x02, 0x00, 0x07, 0xBA, 0xFF};
unchar audio_left_low_right_high[]  = {0xAA, 0x07, 0x02, 0x00, 0x08, 0xBB, 0xFF};

void display_gaoya() ;
void display_diya();
void key_table_scan();
void key_set_range() ;
void baojin();


void display_gaoya()   //��ѹ��ʾ	 ����
{
	lcdgaoya[0]=gaoya/100+'0';
	lcdgaoya[1]=gaoya%100/10+'0';
	lcdgaoya[2]=gaoya%100%10+'0';	
}


void display_diya()   //��ѹ��ʾ	����
{
	lcddiya[0]=diya/100+'0';
	lcddiya[1]=diya%100/10+'0';
	lcddiya[2]=diya%100%10+'0';	
}

void key_table_scan() //�������� ѡ��˵�ģʽ
{
	if(!K1)
	{  
		NRFDelay(50);
		if(!K1)
		{   
			while(!K1)
				;	
			yemian++;
			if(yemian>=3) { yemian=0; } 
		}
	}
}


void key_set_range()  //���������ֵ�����޸�
{
	if(!K2)
	{	  
		NRFDelay(80);
		if(!K2)
		{ 
			if(yemian==1)
			{						
				gaoya+=1;
				if(gaoya>250) 
				{ 
					gaoya=250; 
				} 
				display_gaoya(); 
				LcdShow(1,4,lcdgaoya);
			}

			if(yemian==2)
			{
				diya+=1;
				if(diya>250) 
				{ 
					diya=250; 
				}
				display_diya(); //��ʾ
				LcdShow(1,4,lcddiya);
			}
		}
	}

	if(!K3)
	{  
		NRFDelay(80);
		if(!K3)
		{  
			if(yemian==1)
			{
				gaoya-=1;
				if(gaoya<=1) { gaoya=1; } 	
				display_gaoya(); //
				LcdShow(1,4,lcdgaoya);
			}

			if(yemian==2)
			{
				diya-=1;
				if(diya<=1) { diya=1; } 
				display_diya(); //��ʾ
				LcdShow(1,4,lcddiya);
			}
		}	
	}
}


unchar audio_state = 0x00, audio_flag = 0;
unchar delay_counter = 0;
void baojin() 
{
	// ����ģ�����ڲ�����
	if(AUDIO_BUSY){
		NRFDelay(50);
		if(AUDIO_BUSY){
			NRFDelay(50);
			if(AUDIO_BUSY){
				return;
			}
		}
	}
	
	// ���������Ѿ������
	if((RevTempDate0[0] == '-') || (RevTempDate1[0] == '-')) {
		return;
	}
	
	// ��ʼ����
	a=RevTempDate0[0]-48;
	b=RevTempDate0[1]-48;
	c=RevTempDate0[2]-48;
	SJqiya1=a*100+b*10+c;   //�����ʵ����ѹ1

	a=RevTempDate1[0]-48;
	b=RevTempDate1[1]-48;
	c=RevTempDate1[2]-48;
	SJqiya2=a*100+b*10+c;   //�����ʵ����ѹ2
	
	// ��ȡ����̥ѹ״̬
	audio_state = 0x00;
	if( SJqiya1 >= gaoya ){
		audio_state |= 0x01;
	}
	else if( SJqiya1 < diya ){
		audio_state |= 0x02;
	}
	if( SJqiya2 >= gaoya ){
		audio_state |= 0x04;
	}
	else if( SJqiya2 < diya ){
		audio_state |= 0x08;
	}
	
	// ������ӳ٣��ӳ�ʱ���ں궨��������
	if(audio_state != 0x00){
		delay_counter++;
		if(delay_counter >= AUDIO_DELAY_TIME){
			delay_counter = 0;
			audio_flag = 1;
		}
	}
	
	// Max232SendChar(audio_state);  /* ������ */
	
	// ������������ָ��
	if(audio_flag){
		audio_flag = 0;
		
		switch(audio_state){
			case 0x01:
				Max232SendAChar(audio_left_high);
				break;
			case 0x02:
				Max232SendAChar(audio_left_low);
				break;
			case 0x04:
				Max232SendAChar(audio_right_high);
				break;
			case 0x08:
				Max232SendAChar(audio_right_low);
				break;
			case 0x05:
				Max232SendAChar(audio_left_high_right_high);
				break;
			case 0x0A:
				Max232SendAChar(audio_left_low_right_low);
				break;
			case 0x09:	
				Max232SendAChar(audio_left_high_right_low);
				break;
			case 0x06:	
				Max232SendAChar(audio_left_low_right_high);
				break;
			default:
				break;
		}
	}
}


/************************������********************/
void main()
{
	LCDInt();

	Max232Int();		//���ڳ�ʼ��
	
	LcdShow(0,2,"    Welcome    ");
	NRFDelay(1000);//������ʱ
	LcdShow(1,2,"   (Kpa)    ");
	NRFDelay(1000);
	
	NRF24L01Int();

	while(1)
	{
		key_table_scan();
		
		if(yemian==0)  
		{
			if(flag==0)
			 {
					flag=1;
					LCDInt();
					LcdShow(0,0," Left    Right  ");
			 }
			NRFSetRXMode();//����Ϊ����ģʽ
			GetDate();
			baojin();
		1}
		else if(yemian==1)  //��������ģʽ
		{
			if(flag==1)
			{
				flag=2;
				LCDInt();
				LcdShow(0,3,"   Pressure_H  ");
				LcdShow(2,2,"     .00Kpa");
				display_gaoya(); //��ѹ��ʾ
				LcdShow(1,4,lcdgaoya);
			}
			key_set_range();
		}
		else if(yemian==2)  //��������ģʽ
		{ 
			if(flag==2)
			{
				flag=0;
				LCDInt();
				LcdShow(0,3,"   Pressure_L  ");
				LcdShow(2,2,"     .00Kpa");
				display_diya(); //��ѹ��ʾ
				LcdShow(1,4,lcddiya);
			}
			key_set_range();
		}
	}
}

