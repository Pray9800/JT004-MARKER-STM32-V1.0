/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_gpio.h
 Author: PAN      Version: V1.0       Date:2026/05/09
 Description:       GPIO配置头文件

 Function List:
 History:
*******************************************************/

    #ifndef __BSP_KEY_H_
    #define __BSP_KEY_H_

#include "main.h"
#include "gpio.h"
#include "tim.h"


#define KEY0_READ()          HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)
#define KEY1_READ()          HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2_READ()          HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3_READ()          HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)

#define KEY0_SIG             2  //PB3
#define KEY1_SIG             1  //PB4
#define KEY2_SIG             3  //PB5
#define KEY3_SIG             4  //PB6

void blink();
uint8_t  Key_Process_Scan(void) ;


#endif /* __BSP_GPIO_H_ */
