#ifndef _ALLHEAD_H_
#define _ALLHEAD_H_

#define NRF_OVERTIME  		 15			//NRF接受超时时间
#define AUDIO_DELAY_TIME   20			//语音模块播放指令延迟发送时间

#define PRESSURE_H   100		// 气压上限
#define PRESSURE_L   90			// 气压下限

#include "pubile.h"
#include "DELAY.h"
#include "LCD1602.h"
#include "NRF24L01.h" 
#include "MAX232.h" 		// 串口头文件

#endif
