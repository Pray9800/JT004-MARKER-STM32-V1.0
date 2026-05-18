#include "bsp_usart.h"
#include "string.h"
 #include "usart.h"



uint8_t rx_temp; // 单字节接收缓存
static uint8_t UART1_Rxbuff[50];
uart1_data_t  UR_Send_Msg;
volatile uint8_t UR_Send_packet_ready=0;
uint8_t uart_tt[7] = {0xa5,0x5a,0x0a,0x01,0x00,0xb6,0x6b}; //测试




void MCU_UR_Receive_IT_Start(uint8_t *pData, uint16_t Size)
{
 
    HAL_UART_Receive_IT(&huart1, pData, Size);
}


void MCU_UR_Send(uint8_t *buf,uint8_t len)
{
  
     HAL_GPIO_WritePin(CRL_GPIO_Port, CRL_Pin, GPIO_PIN_SET); 
     for(volatile uint16_t i = 0; i < 500; i++) { __NOP(); } //保留一定时间间隔 信号差的时候必须开启
     HAL_UART_Transmit_IT(&huart1, buf, len);
    // 拉低切回接收
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) 
    {
        // 中断传输完成
        //关闭发送开启接收
        for(volatile uint16_t i = 0; i < 500; i++) { __NOP(); }; //等价115200波特率下的一个字节时间 
        HAL_GPIO_WritePin(CRL_GPIO_Port, CRL_Pin, GPIO_PIN_RESET);

    }
}







/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/11
 Function:          HAL_UARTEx_RxEventCallback
 Description:       解析指令
                     A5 5A   cmd   len   data   B6 6B
                     帧头     命令   长度   数据   帧尾
 Input:             无
 Output:            无
 Return:            无
 Others:            无
// *******************************************************/
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart->Instance == USART1)
//     {
//         // 判断包头 A5，以及包尾 B6 6B
//         // A5 5A   cmd   len   data   B6 6B
//         // 帧头     命令   长度   数据   帧尾
//         if (Size >= 5 && UART1_Rxbuff[0] == 0xA5) 
//         {
//             if (UART1_Rxbuff[Size - 2] == 0xB6 && UART1_Rxbuff[Size - 1] == 0x6B)
//             {              
//                 //memcpy(UART1_RxData_deal, UART1_Rxbuff + 2, Size - 4);
//                 UR_Send_Msg.cmd  = UART1_Rxbuff[2];
//                 UR_Send_Msg.len  = UART1_Rxbuff[3];
//                 // 关键数据提取 现在用于灯带颜色
//                 UR_Send_Msg.data = UART1_Rxbuff[4]; 
//                 UR_Send_packet_ready = 1; // 通知 main 函数 
//                  blink_flag=0;
                 
//                  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
//             }           
//     }   
//          HAL_UARTEx_ReceiveToIdle_IT(&huart1, UART1_Rxbuff, sizeof(UART1_Rxbuff)); 
//     }
 
// }





/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/11
 Function:          HAL_UART_RxCpltCallback
 Description:       解析指令 单字节版本
                     A5 5A   cmd   len   data   B6 6B
                     帧头     命令   长度   数据   帧尾
 Input:             无
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t rx_cnt = 0; //  计数器，记录当前拼接到第几个字节了 
    
    if (huart->Instance == USART1)
    {
        
        
        if (rx_cnt == 0) //还没有收到数据
        {
            if (rx_temp == 0xA5) // 收到开头
            {
                UART1_Rxbuff[0] = rx_temp;
                rx_cnt = 1;
            }
            // 如果不是 A5 (比如是干扰乱码)，什么都不做，丢弃！
        }
        else if (rx_cnt == 1) // A5开头已经符合
        {
            if (rx_temp == 0x5A)  
            {
                UART1_Rxbuff[1] = rx_temp;
                rx_cnt = 2;
            }
            else //  A5 后面跟的不是 5A (说明前面的 A5 碰巧是乱码)，直接复位重找
            {
                rx_cnt = 0; 
            }
        }
        else // 包头 A5 5A 确认无误，开始接收后续数据
        {
            UART1_Rxbuff[rx_cnt] = rx_temp;  
            rx_cnt++;
            
           
            if (rx_cnt >= 7)  //要求长度是7之后才去校验后卫是不是B6 6B
            {
                // 检查最后两个
                if (UART1_Rxbuff[rx_cnt - 2] == 0xB6 && UART1_Rxbuff[rx_cnt - 1] == 0x6B)
                {
                    // 赋值 长度 命令  数据
                    UR_Send_Msg.cmd = UART1_Rxbuff[2];
                    UR_Send_Msg.len = UART1_Rxbuff[3];
                    UR_Send_Msg.data = UART1_Rxbuff[4];  //后续出现双字节数据 需要优化
                    UR_Send_packet_ready = 1; // 灯带控制标志位
                    //blink_flag = 0;
                 //   HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                    rx_cnt = 0; // 成功解析一条，计数器清零，准备收下一条
                }
                else if (rx_cnt > 10) // 超出10个字节的边界 代表收到现在还是没有收到B6 6B
                {
                    rx_cnt = 0; // 防止数组越界，强行清零
                }
            }
        }  
        // HAL_UART_Receive_IT(&huart1, &rx_temp, 1); 
        MCU_UR_Receive_IT_Start( &rx_temp, 1);//再次中断回调开启
    }
}

