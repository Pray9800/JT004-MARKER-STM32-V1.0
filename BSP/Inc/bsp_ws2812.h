/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_ws2812.h
 Author: PAN        Version: V1.0       Date:2026/05/09
 Description:       d灯带的三个操作

 Function List:
 History:
*******************************************************/



#ifndef _WS2812_H_
#define _WS2812_H_


#include "main.h"
#include "gpio.h"
#include <stdint.h>

 

// 使用寄存器直接操作  寄存器方式
#define LED_C_H    (LED_C_GPIO_Port->BSRR = LED_C_Pin)  
#define LED_C_L    (LED_C_GPIO_Port->BRR  = LED_C_Pin)  

// 最大支持的灯珠数量  暂定18
#define WS_ARRAY_SIZE 18 


void ws2812_refresh(uint8_t ws_count);
void ws2812_rgb_all(uint8_t ws_count, uint8_t ws_r, uint8_t ws_g, uint8_t ws_b);
void ws2812_set_num(uint16_t num, uint8_t r, uint8_t g, uint8_t b);

#endif