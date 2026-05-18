#include "app_task.h"
#include "bsp_key.h"
#include "bsp_usart.h"
// #include "bsp_ws2812.h"
#include "bsp_ws2812_spi.h"
#include "bsp_iwdg.h"
#include "bsp_tim.h"
#include "stm32f0xx_hal.h"




uint8_t g_keys_value = 0, g_keys_value_last = 0;//按键之后返回值
uint8_t g_rgb_value = 0, g_rgb_sign = 0, g_rgb_sign_last = 0; //UR机械臂传来的sig 和对应颜色数值的value
// A5 5A 帧头 0A 指令 01长度 01数据 b6 6b真尾
//00 没有按键  01第一个按键  02第二个按键  03第三个按键  04第四个按键 
uint8_t uart_key[7] = {0xa5,0x5a,0x0a,0x01,0x00,0xb6,0x6b}; 
uint16_t rgb_cnt = 0;  // 刷新用于计数






void task_init()
{
     
        TIM17_IT_1KHz_Start();  //定时器开启
        MCU_UR_Receive_IT_Start( &rx_temp, 1);//中断回调开启
        ws2812_spi_init(); // 初始化缓冲区
        ws2812_set_num_spi(WS_ARRAY_SIZE, 100, 100, 100); //颜色初始化
        Sys_Delay(500);//延时
        ws2812_set_num_spi(WS_ARRAY_SIZE, 100, 100, 100); //颜色初始化
     
}


void task()
{
      
    task_init(); //初始化
  
    while (1)
    {         
             
         
         //闪烁 50ms延时
          if(blink_cnt>=blink_fre_100ms) //100ms 频率在tim17里面计时      
          {
            blink(); 
            blink_cnt=0; //闪烁计数清零
          }
        
        //按键读取
         g_keys_value=Key_Process_Scan();   //周期性扫描 10ms 消抖

        //发现按键传输数值有变 
        if(g_keys_value !=  g_keys_value_last)
        {    
            uart_key[4] = g_keys_value;  //按键信号赋值 
            MCU_UR_Send(uart_key, sizeof(uart_key)); //嵌入格式发送
            Sys_Delay(10);
            g_keys_value_last = g_keys_value;
        }

        //读取UR那边的小灯带颜色指令  
        if (UR_Send_packet_ready)  //来自串口中断回调用于赋值
        {     
            
            // 暂时没有CMD区分
            g_rgb_sign = UR_Send_Msg.data; //赋值
                if(g_rgb_sign == 4) //红
                {
                     g_rgb_value = 4;//0x0100红色刷新指令

                }
                    
                else if(g_rgb_sign == 3)//绿 
                {
                     g_rgb_value = 2; //0x0010绿色刷新指令

                }
                
                else if(g_rgb_sign == 2)//蓝
                {     
                      g_rgb_value = 1;      //0x0001蓝色刷新指令

                }
                    
                else if(g_rgb_sign == 1)//白
                
                {
                     g_rgb_value = 7;  //0x0111白色刷新指令

                }
                 
            UR_Send_packet_ready = 0; // UR传输的清除标志
        }

        //定时器100hz计时  刷新频率3次 刷新小灯带
        if( g_timer_cnt>=10)
        {
        g_timer_cnt=0;
        if( g_rgb_sign != g_rgb_sign_last)
            {
                rgb_cnt ++;//刷新次数
                if( rgb_cnt >=3)
                {
                    rgb_cnt = 0;
                    g_rgb_sign_last = g_rgb_sign; //不再刷新
                } 
                else
                        ws2812_set_num_spi(WS_ARRAY_SIZE , 
                                ((g_rgb_value>>2)&0x01)*BRIGHTNESS, 
                                ((g_rgb_value>>1)&0x01)*BRIGHTNESS, 
                                ((g_rgb_value & 0x01))*BRIGHTNESS); //一直刷新10次 100hz 频率                 
            }
        }          
          IWDG_Refresh(); //看门狗刷新       
    }      
  
}