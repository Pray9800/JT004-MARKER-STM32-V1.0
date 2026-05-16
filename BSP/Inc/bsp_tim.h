/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_delay.h
 Author: PAN        Version: V1.0       Date:2026/05/15
 Description:       系统延时封装（基于HAL_Delay）

 Function List:
     1. BSP_Delay_ms()     - 毫秒级延时
 History:
*******************************************************/

#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "stdint.h"
#include "tim.h"



extern  volatile uint8_t g_timer_cnt; 
extern  volatile uint8_t blink_cnt;     //led闪烁计时
/*===================== 函数声明 =====================*/
void Sys_Delay(uint16_t time_ms);
void TIM17_IT_1KHz_Start();
#endif /* __BSP_DELAY_H */