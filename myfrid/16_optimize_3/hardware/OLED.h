#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
#include <stdint.h>

// 从 W25Q64 中读取指定汉字的 HZK16 点阵数据（font_data 需外部分配至少32字节）
void Read_HZK16_From_W25Q64(uint8_t area, uint8_t index, uint8_t *font_data);

// 将 HZK16 点阵数据转换为 SSD1306 OLED 可用的格式（输出上下各16字节）
static void HZK16_To_SSD1306(
    const uint8_t *raw32,
    uint8_t outTop[16],
    uint8_t outBottom[16]);

// 在 OLED 上显示一个 16×16 汉字（x 为像素位置，单位为像素）
void OLED_ShowChinese(uint8_t Line, uint8_t Column, const uint8_t *gb2312);

// 显示多个汉字（传入 GB2312 编码的字符串指针，如 "你好世界"）
void OLED_ShowChineseString(uint8_t Line, uint8_t Column, const char *gbStr);

// 混合显示中英文字符串（英文字符自动识别，汉字为 GB2312 编码）
void OLED_ShowMixedString(uint8_t Line, uint8_t Column, const char *gbStr);
#endif
