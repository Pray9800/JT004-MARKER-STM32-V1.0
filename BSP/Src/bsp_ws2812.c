#include "bsp_ws2812.h"

// 数据缓存数组，每个灯珠需要 3 个字节 (G, R, B)
uint8_t ws_data[WS_ARRAY_SIZE * 3] = {0};

/* ========================================================
 * 48MHz 主频下，1 NOP = 20.83 ns
 * NOP_14 约 291ns 符合数据手册 220-380 的需求
 * NOP_36 约 750ns 符合数据手册  580-1us的需求 
 * ======================================================== */
#define NOP_10()  __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()
#define NOP_14()  NOP_10();__NOP();__NOP();__NOP();__NOP()
#define NOP_36()  NOP_10();NOP_10();NOP_10();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()




/**
发送 0 码 (高电平 ~300ns，低电平 ~800ns)
 static inline说明 当 static 和 inline 连用时，会产生奇效：
编译器发现这个函数只能在当前文件用（static），
并且你要求了到处复制粘贴（inline）。那么编译器在复制粘贴完之后，
会直接把 WS_Send_0 这个函数本体从 Flash 内存里删因为它已经不需要
作为一个独立的实体存在了，这为你极其紧张的单片机 Flash 省下了
宝贵的存储空间。--参考了gemini AI
 */

static inline void WS_Send_0(void)  
{
    LED_C_H;     // 瞬间拉高
    NOP_14();    // 延时约 291ns
    LED_C_L;     // 瞬间拉低
    NOP_36();    // 延时约 750ns
}

/**
  *  发送 1 码 (高电平 ~800ns，低电平 ~300ns)
  */
static inline void WS_Send_1(void)
{
    LED_C_H;     // 瞬间拉高
    NOP_36();    // 延时约 750ns
    LED_C_L;     // 瞬间拉低
    NOP_14();    // 延时约 291ns
}

/**
  * @brief  复位信号 (低电平 > 280us)
  */
static inline void WS_Reset(void)
{
    LED_C_L;      // 确保拉低
    // 延时 300us+
    for(uint16_t i=0;i<3000;i++) NOP_10();


}
 
/*******************************************************
 Author: PAN       Version: V1.0       Date:2026/05/09
 Function:          ws2812_refresh(uint8_t ws_count)
 Description:       刷新数据到 WS2812 硬件
 Calls:             HAL_GPIO_WritePin
 Input:             需要刷新的灯珠数量
 Output:            无
 Return:            无
 Others:            无
*******************************************************/
void ws2812_refresh(uint8_t ws_count)
{
    uint8_t i, j, data;
    
    // HAL_NVIC_DisableIRQ(TIM16_IRQn); // 仅关闭 TIM16 中断
    // HAL_NVIC_EnableIRQ(TIM16_IRQn);  // 重新开启
    //选择AI推荐的__disable_irq(); 
    __disable_irq();  //暂停中断
    
    // (每颗灯 3 字节)
    for (i = 0; i < ws_count * 3; i++)
    {
        data = ws_data[i]; //GRB的颜色发送顺序
        
        // 从高位到低位依次发送 8 bit
        for (j = 0; j < 8; j++)
        {
            if (data & 0x80)
            {
                WS_Send_1();
            }
            else
            {
                WS_Send_0();
            }
            data <<= 1; // 移位，准备发送下一位
        }
    }
    
    // 发送完毕，恢复系统中断
    __enable_irq(); 
    
    // 发送复位码，让灯珠锁存刚才的数据并显示出来
    WS_Reset();
}


/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/05/09
 Function:          ws2812_rgb_all
 Description:       将所有灯珠设置为同一个颜色
 Input:             ws_count: 灯珠数量 ws_r, ws_g, ws_b: 颜色值 (0-255)
 Output:            无
 Return:            无
 Others:            无
*******************************************************/ 
void ws2812_rgb_all(uint8_t ws_count, uint8_t ws_r, uint8_t ws_g, uint8_t ws_b)	
{
    uint8_t i;
    
    // 安全防护：防止数组越界
    if (ws_count > WS_ARRAY_SIZE) ws_count = WS_ARRAY_SIZE;
    
    for(i = 0; i < ws_count; i++)
    {
        // WS2812B 的硬件数据接收顺序是 G - R - B G有8个 R有8个 B有8个
        ws_data[i * 3 + 0] = ws_g;
        ws_data[i * 3 + 1] = ws_r;
        ws_data[i * 3 + 2] = ws_b;
    }
    // 修改缓存区 点亮以内刷新
}



/*******************************************************
 Author: PAN        Version: V1.0       Date:2026/05/09
 Function:          ws2812_rgb_all
 Description:       点亮指定数量的前 N 颗灯，全部设置为此颜色，刷新
 Input:             num: 要点亮的灯珠数量
 Output:            无
 Return:            无
 Others:            无
*******************************************************/ 
void ws2812_set_num(uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t i;
    
    // 安全防护：防止数组越界
    if (num > WS_ARRAY_SIZE) num = WS_ARRAY_SIZE;
    
    for (i = 0; i < num; i++)
    {
        ws_data[i * 3 + 0] = g;
        ws_data[i * 3 + 1] = r;
        ws_data[i * 3 + 2] = b;
    }
    
    // 立即调用底层刷新硬件
    ws2812_refresh(num);
}