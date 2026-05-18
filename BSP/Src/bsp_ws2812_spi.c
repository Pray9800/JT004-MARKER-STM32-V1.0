#include "bsp_ws2812_spi.h"
#include "spi.h" 
#include "string.h"

// DMA 发送缓冲区
uint8_t ws_spi_data[WS_SPI_BUF_SIZE] = {0};

/*******************************************************
 Author:        PAN
 Version:       V1.0
 Date:          2026/05/18
 Function:      ws2812_spi_init
 Description:   初始化 WS2812 SPI 数据缓冲区
 Input:         无
 Output:        初始化全局 SPI 发送缓冲区 ws_spi_data
 Return:        无
 Others:        末尾约 40 字节保持 0x00 以满足 WS2812 复位低电平要求
*******************************************************/
void ws2812_spi_init(void)
{
    // 全局清零，其中末尾的 40 字节发送 0x00 相当于长时间低电平，触发 WS2812 锁存
    memset(ws_spi_data, 0x00, WS_SPI_BUF_SIZE);
}

/*******************************************************
 Author:        PAN
 Version:       V1.0
 Date:          2026/05/18
 Function:      WS_Set_Color_Spi
 Description:   将单个 WS2812 灯珠的 RGB 颜色转换为 SPI 发送编码并写入缓存
 Input:         index: 灯珠序号 (0 开始)
                r: 红色分量 (0~255)
                g: 绿色分量 (0~255)
                b: 蓝色分量 (0~255)
 Output:        按照 WS2812 SPI 编码规则填充 ws_spi_data 对应位置
 Return:        无
 Others:        WS2812 数据顺序为 G-R-B，编码 0/1 分别使用 WS_CODE_0 / WS_CODE_1
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
 Author:        PAN          Version:       V1.0
 Date:          2026/05/18   Function:      ws2812_refresh_spi
 Description:   通过 DMA 启动 SPI 发送 ws_spi_data 缓冲区数据
 Input:         无
 Output:        触发 WS2812 数据发送
 Return:        无
 Others:        仅在 SPI 空闲时调用，避免覆盖正在进行的 DMA 传输
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
 Author:        PAN             Version:       V1.0
 Date:          2026/05/18      Function:      ws2812_set_num_spi
 Description:   将前 num 颗 WS2812 灯珠设置为指定颜色并立即刷新输出
 Input:         num: 要设置的灯珠数量
                r: 红色分量 (0~255)
                g: 绿色分量 (0~255)
                b: 蓝色分量 (0~255)
 Output:        更新 ws_spi_data，触发 DMA SPI 发送
 Return:        无
 Others:        num 超出范围时自动截断为 WS_ARRAY_SIZE；等待 SPI 空闲后才发送
*******************************************************/
void ws2812_set_num_spi(uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
    if (num > WS_ARRAY_SIZE) num = WS_ARRAY_SIZE;
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) 
    {
        // 阻塞等待
    }
    // 清空前面的颜色缓存 
    memset(ws_spi_data, WS_CODE_0, WS_ARRAY_SIZE * 24);
    
    for (uint16_t i = 0; i < num; i++)
    {
        WS_Set_Color_Spi(i, r, g, b);
    }
    
    ws2812_refresh_spi();
}

/*******************************************************
 Author:        PAN             Version:       V1.0
 Date:          2026/05/18      Function:      ws2812_rgb_all_spi
 Description:   将前 ws_count 颗 WS2812 灯珠缓存为指定颜色，但不立即发送
 Input:         ws_count: 要设置的灯珠数量
                r: 红色分量 (0~255)
                g: 绿色分量 (0~255)
                b: 蓝色分量 (0~255)
 Output:        更新 ws_spi_data，对应灯珠颜色已缓存
 Return:        无
 Others:        设置完成后需调用 ws2812_refresh_spi 才会输出到灯带
*******************************************************/
void ws2812_rgb_all_spi(uint8_t ws_count, uint8_t r, uint8_t g, uint8_t b)	
{
    if (ws_count > WS_ARRAY_SIZE) ws_count = WS_ARRAY_SIZE;
    
    for(uint16_t i = 0; i < ws_count; i++)
    {
        WS_Set_Color_Spi(i, r, g, b);
    }
}