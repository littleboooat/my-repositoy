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

// �� W25Q64 �ж�ȡָ�����ֵ� HZK16 �������ݣ�font_data ���ⲿ��������32�ֽڣ�
void Read_HZK16_From_W25Q64(uint8_t area, uint8_t index, uint8_t *font_data);

// �� HZK16 ��������ת��Ϊ SSD1306 OLED ���õĸ�ʽ��������¸�16�ֽڣ�
static void HZK16_To_SSD1306(
    const uint8_t *raw32,
    uint8_t outTop[16],
    uint8_t outBottom[16]);

// �� OLED ����ʾһ�� 16��16 ���֣�x Ϊ����λ�ã���λΪ���أ�
void OLED_ShowChinese(uint8_t Line, uint8_t Column, const uint8_t *gb2312);

// ��ʾ������֣����� GB2312 ������ַ���ָ�룬�� "�������"��
void OLED_ShowChineseString(uint8_t Line, uint8_t Column, const char *gbStr);

// �����ʾ��Ӣ���ַ�����Ӣ���ַ��Զ�ʶ�𣬺���Ϊ GB2312 ���룩
void OLED_ShowMixedString(uint8_t Line, uint8_t Column, const char *gbStr);
#endif
