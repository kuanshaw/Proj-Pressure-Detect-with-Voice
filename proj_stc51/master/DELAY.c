#include"reg52.h"
#include"Allhead.h"

/***************************��ʱ����*********************/
void LCDDelay(unchar t)
{
  unchar x,y;
  for(x=t;x>0;x--)
    for(y=110;y>0;y--);
}


/**********************��ʱ����**************************/
void NRFDelay(unint t)
{
   unint x,y;
   for(x=t;x>0;x--)
    for(y=110;y>0;y--);
}