#ifndef __BSP_WS2812_SPI_H
#define __BSP_WS2812_SPI_H

#include "main.h"

// 灯珠数量
#define WS_ARRAY_SIZE 18  

// 编码定义 (基于 6.0MHz SPI 波特率)
#define WS_CODE_0  0xC0  // 11000000
#define WS_CODE_1  0xF8  // 11111000

/**以下是6MHz SPI 波特率下的时序分析：
   WS2812B 数据手册要求：
   - 发送 0 码：高电平 ~300ns，低电平 ~800ns
   - 发送 1 码：高电平 ~800ns，低电平 ~300ns
   SPI 波特率为 6.0MHz 时，每个 SPI bit 的周期为 166.67ns。
   因此：
   - WS_CODE_0 (0xC0 = 11000000) 的前两位是 1，持续约 333ns，满足高电平时间；后六位是 0，持续约 1000ns，满足低电平时间。
   - WS_CODE_1 (0xF8 = 11111000) 的前五位是 1，持续约 833ns，满足高电平时间；后3位是 0，持续约 500ns，满足低电平时间。
 *  333ns        1000ns
   ┌──────┐────────────────────────┐
   │ 1  1 │ 0  0  0  0  0  0       │
───┘      └────────────────────────┘
          833ns            500ns
   ┌───────────────────────┐────────┐
   │ 1  1  1  1  1         │ 0  0  0│
───┘                       └────────┘
 */
//基于6MB/S的SPI波特率
// 缓冲数组大小: 每个灯珠 24 bits (对应 24 Bytes SPI 数据) + 250 Bytes 的复位信号(>280us低电平)
#define WS_SPI_BUF_SIZE  (WS_ARRAY_SIZE * 24 + 300)

extern uint8_t ws_spi_data[WS_SPI_BUF_SIZE];

void ws2812_spi_init(void); 
void ws2812_rgb_all_spi(uint8_t ws_count, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_num_spi(uint16_t num, uint8_t r, uint8_t g, uint8_t b);
void ws2812_refresh_spi(void);

#endif /* __BSP_WS2812_SPI_H */