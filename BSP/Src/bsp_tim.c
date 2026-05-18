


#include "bsp_tim.h"
#include "main.h"          // 请确保这里能找到 HAL_Delay


volatile uint8_t g_timer_cnt = 0; // 1ms 计时位 

volatile uint8_t blink_cnt;     //led闪烁计时
 
/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/05/11
 Function:          Key_Scan_Once
 Description:       毫秒延时（ms）
 Input:             ms           
 Output:            按键代表的数值
 Return:            无
 Others:            无
*******************************************************/
void Sys_Delay(uint16_t time_ms)

{
   HAL_Delay(time_ms); 
}



  //定时器开启
  void TIM17_IT_1KHz_Start()
  {
    HAL_TIM_Base_Start_IT(&htim17);
  }



/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/05/18
 Function:          HAL_TIM_PeriodElapsedCallback
 Description:       定时器回调
 Input:             tim7          
 Output:            改变计数值
 Return:            无
 Others:            无
*******************************************************/
  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM17)
    {
        // 每次中断翻转 flag
         g_timer_cnt++ ; // 1000Hz 产生一个标志
         blink_cnt++;
        
    }
}
