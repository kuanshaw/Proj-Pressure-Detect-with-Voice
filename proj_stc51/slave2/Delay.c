#include"reg52.h"
#include"Allhead.h"
/***************************延时函数****************/
void Delay(unint t)
{
  unint x,y;
  for(x=t;x>0;x--)
   for(y=110;y>0;y--);
}
/**********************************************
   DS18B20专用延时子函数
**********************************************/
void DDelay(unint t) 
{ 
	unint n;
	n=0; 
	while(n<t) 
	{
	  n++;
	} 
}
