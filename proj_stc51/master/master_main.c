#include "reg52.h"
#include "Allhead.h"
#include "intrins.h"

sbit K1 = P3^2;			 			//菜单
sbit K2 = P3^3;  					//加
sbit K3 = P3^4;  					//减
sbit AUDIO_BUSY = P3^5;		//读取语音模块的BUSY引脚，检测是否正在播放

unchar yemian=0;
unchar num=0;
unchar flag=0,a,b,c;
unchar SJqiya1,SJqiya2;					//实际中所测的气压
unchar gaoya = PRESSURE_H, diya = PRESSURE_L;  	//gaoya代表气压上限，diya代表气压下限。

// 用于LCD显示
unchar lcdgaoya[5],lcddiya[5];

// 语音播放指令列表
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


void display_gaoya()   //气压显示	 上限
{
	lcdgaoya[0]=gaoya/100+'0';
	lcdgaoya[1]=gaoya%100/10+'0';
	lcdgaoya[2]=gaoya%100%10+'0';	
}


void display_diya()   //气压显示	下限
{
	lcddiya[0]=diya/100+'0';
	lcddiya[1]=diya%100/10+'0';
	lcddiya[2]=diya%100%10+'0';	
}

void key_table_scan() //按键程序 选择菜单模式
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


void key_set_range()  //对里面的数值进行修改
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
				display_diya(); //显示
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
				display_diya(); //显示
				LcdShow(1,4,lcddiya);
			}
		}	
	}
}


unchar audio_state = 0x00, audio_flag = 0;
unchar delay_counter = 0;
void baojin() 
{
	// 语音模块正在播放中
	if(AUDIO_BUSY){
		NRFDelay(50);
		if(AUDIO_BUSY){
			NRFDelay(50);
			if(AUDIO_BUSY){
				return;
			}
		}
	}
	
	// 接受数组已经被清空
	if((RevTempDate0[0] == '-') || (RevTempDate1[0] == '-')) {
		return;
	}
	
	// 开始计算
	a=RevTempDate0[0]-48;
	b=RevTempDate0[1]-48;
	c=RevTempDate0[2]-48;
	SJqiya1=a*100+b*10+c;   //所测的实际气压1

	a=RevTempDate1[0]-48;
	b=RevTempDate1[1]-48;
	c=RevTempDate1[2]-48;
	SJqiya2=a*100+b*10+c;   //所测的实际气压2
	
	// 读取左右胎压状态
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
	
	// 命令发送延迟，延迟时间在宏定义中设置
	if(audio_state != 0x00){
		delay_counter++;
		if(delay_counter >= AUDIO_DELAY_TIME){
			delay_counter = 0;
			audio_flag = 1;
		}
	}
	
	// Max232SendChar(audio_state);  /* 调试用 */
	
	// 发送语音播放指令
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


/************************主函数********************/
void main()
{
	LCDInt();

	Max232Int();		//串口初始化
	
	LcdShow(0,2,"    Welcome    ");
	NRFDelay(1000);//短暂延时
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
			NRFSetRXMode();//设置为接收模式
			GetDate();
			baojin();
		1}
		else if(yemian==1)  //进入设置模式
		{
			if(flag==1)
			{
				flag=2;
				LCDInt();
				LcdShow(0,3,"   Pressure_H  ");
				LcdShow(2,2,"     .00Kpa");
				display_gaoya(); //气压显示
				LcdShow(1,4,lcdgaoya);
			}
			key_set_range();
		}
		else if(yemian==2)  //进入设置模式
		{ 
			if(flag==2)
			{
				flag=0;
				LCDInt();
				LcdShow(0,3,"   Pressure_L  ");
				LcdShow(2,2,"     .00Kpa");
				display_diya(); //气压显示
				LcdShow(1,4,lcddiya);
			}
			key_set_range();
		}
	}
}

