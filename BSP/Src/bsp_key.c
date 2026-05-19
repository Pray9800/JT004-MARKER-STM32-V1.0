

#include "bsp_key.h"

#include "bsp_tim.h"
#include "stm32f0xx_hal.h"



/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/11
 Function:          Key_Scan_Once
 Description:       闪烁10hz 检验程序是否正常运行
 Input:             
 Output:            按键代表的数值
 Return:            无
 Others:            无
*******************************************************/
void blink()
{
    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
    //Sys_Delay(50);

}

 
/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/11
 Function:          Key_Scan_Once
 Description:       按键读取和映射
 Input:             
 Output:            按键代表的数值
 Return:            无
 Others:            无
*******************************************************/
static uint8_t Key_Scan_Once(void)
{
    // 依次检测 KEY2, KEY1, KEY3, KEY0 (对应老代码   顺序)
    if (KEY1_READ() == GPIO_PIN_RESET) return KEY1_SIG   ;   //物理按键1
    if (KEY0_READ() == GPIO_PIN_RESET) return KEY0_SIG   ;   //物理按键2
    if (KEY2_READ() == GPIO_PIN_RESET) return KEY2_SIG   ;   //物理按键3
    if (KEY3_READ() == GPIO_PIN_RESET) return KEY3_SIG   ;   //物理按键4
    return 0;  //没有按键按下就是发送0
}





 
/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/11
 Function:          Key_Process_Scan
 Description:       读取按键 读取两次  优先级K1 K0 K2 K3 实现“采样 -> 等待  -> 再次采样确认
 Input:             需要刷新的灯珠数量
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
    uint8_t  Key_Process_Scan(void) 
{
     uint8_t keys_value_first,keys_value_second;
     keys_value_first= Key_Scan_Once();
     HAL_Delay(10);
     keys_value_second=Key_Scan_Once();
     if(keys_value_second==keys_value_first&&keys_value_second!=0)
     return keys_value_second;
    else 
    return 0;

}
// &&keys_value_second!=0






