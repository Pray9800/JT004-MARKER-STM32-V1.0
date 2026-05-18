#ifndef __BSP_WS2812_SPI_H
#define __BSP_WS2812_SPI_H

#include "main.h"

// 你实际使用的灯珠数量
#define WS_ARRAY_SIZE 18  

// 编码定义 (基于 6.0MHz SPI 波特率)
#define WS_CODE_0  0xC0  // 11000000
#define WS_CODE_1  0xF8  // 11111000

// 缓冲数组大小: 每个灯珠 24 bits (对应 24 Bytes SPI 数据) + 40 Bytes 的复位信号(>50us低电平)
#define WS_SPI_BUF_SIZE  (WS_ARRAY_SIZE * 24 + 40)

extern uint8_t ws_spi_data[WS_SPI_BUF_SIZE];

void ws2812_spi_init(void);
void ws2812_rgb_all(uint8_t ws_count, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_num(uint16_t num, uint8_t r, uint8_t g, uint8_t b);
void ws2812_refresh(void);

#endif