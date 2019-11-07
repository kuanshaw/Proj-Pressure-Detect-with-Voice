#ifndef _LCD1602_H_
#define _LCD1602_H_
/*********************************************/

extern void LCDWriteCmd(unchar cmd);
void LCDWriteDate(unchar date);
void LcdShow(unchar x,unchar y,unchar *str);
void SetXY(unchar x,unchar y);
extern void LCDInt();

/********************************************/
#endif
