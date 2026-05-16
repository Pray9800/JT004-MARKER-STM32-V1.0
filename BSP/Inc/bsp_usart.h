/*******************************************************
 Copyright (C), HangZhou Jianjia Co.,Ltd.
 File name:         bsp_uasrt.h
 Author: PAN        Version: V1.0       Date:2026/05/09
 Description:       和UR机械臂的通讯

 Function List:
 History:
*******************************************************/




#include "main.h"
#include "gpio.h"
#include "usart.h"




// 解析后的数据结构体
typedef struct{
    uint8_t    cmd;    // 命令
    uint8_t    len;//寄存器地址
    uint8_t    data;  // 数据主体指针
}uart1_data_t;
extern volatile uint8_t blink_flag;
extern uart1_data_t  UR_Send_Msg;
extern volatile uint8_t UR_Send_packet_ready;
extern uint8_t UART1_Rxbuff[50];

extern uint8_t rx_temp; // 声明刚刚定义的单字节缓存
void MCU_UR_Receive_IT_Start( uint8_t *pData, uint16_t Size);
void MCU_UR_Send(uint8_t *buf,uint8_t len);