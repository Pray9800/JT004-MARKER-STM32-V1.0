#include "bsp_ws2812_spi.h"
#include "spi.h" // 确保包含了 CubeMX 生成的 spi.h
#include "string.h"

// DMA 发送缓冲区
uint8_t ws_spi_data[WS_SPI_BUF_SIZE] = {0};

/*******************************************************
 Function:    ws2812_spi_init
 Description: 初始化缓冲区，将最后 40 字节清零作为复位信号
*******************************************************/
void ws2812_spi_init(void)
{
    // 全局清零，其中末尾的 40 字节发送 0x00 相当于长时间低电平，触发 WS2812 锁存
    memset(ws_spi_data, 0x00, WS_SPI_BUF_SIZE);
}

/*******************************************************
 Function:    WS_Set_Color_Spi
 Description: 将 RGB 颜色转换为 SPI 数据并存入数组
 Input:       index: 灯珠序号 (0 开始)
              r, g, b: 颜色值
*******************************************************/
static void WS_Set_Color_Spi(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if(index >= WS_ARRAY_SIZE) return; // 防越界
    
    // WS2812B 数据顺序是 G - R - B
    uint32_t color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    
    uint16_t start_idx = index * 24;
    
    // 将 24位 颜色值拆分成 24 个 SPI 字节
    for (int i = 0; i < 24; i++)
    {
        // 从最高位 (第23位) 开始判断
        if (color & (1 << (23 - i))) 
        {
            ws_spi_data[start_idx + i] = WS_CODE_1;
        }
        else 
        {
            ws_spi_data[start_idx + i] = WS_CODE_0;
        }
    }
}

/*******************************************************
 Function:    ws2812_refresh
 Description: 启动 DMA 发送 (非阻塞，极速完成，不干扰中断)
*******************************************************/
void ws2812_refresh_spi(void)
{
    // 发送前确保上一次 DMA 传输已经结束
    if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY) 
    {
        // SPI_DMA发送
        HAL_SPI_Transmit_DMA(&hspi1, ws_spi_data, WS_SPI_BUF_SIZE);
    }
}

/*******************************************************
 Function:    ws2812_set_num
 Description: 设置前 N 颗灯的颜色并立刻刷新
*******************************************************/
void ws2812_set_num_spi(uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
    if (num > WS_ARRAY_SIZE) num = WS_ARRAY_SIZE;
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) 
    {
        // 阻塞等待
    }
    // 清空前面的颜色缓存 (防止变暗/变色时残留旧数据)
    memset(ws_spi_data, WS_CODE_0, WS_ARRAY_SIZE * 24);
    
    for (uint16_t i = 0; i < num; i++)
    {
        WS_Set_Color_Spi(i, r, g, b);
    }
    
    ws2812_refresh_spi();
}

/*******************************************************
 Function:    ws2812_rgb_all
 Description: （不立刻刷新）
*******************************************************/
void ws2812_rgb_all_spi(uint8_t ws_count, uint8_t r, uint8_t g, uint8_t b)	
{
    if (ws_count > WS_ARRAY_SIZE) ws_count = WS_ARRAY_SIZE;
    
    for(uint16_t i = 0; i < ws_count; i++)
    {
        WS_Set_Color_Spi(i, r, g, b);
    }
}