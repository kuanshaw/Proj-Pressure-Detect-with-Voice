#include"reg52.h"
#include"Allhead.h"
/***************************��ʱ����****************/
void Delay(unint t)
{
  unint x,y;
  for(x=t;x>0;x--)
   for(y=110;y>0;y--);
}
/**********************************************
   DS18B20ר����ʱ�Ӻ���
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
