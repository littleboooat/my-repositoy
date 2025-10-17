#include "stm32f10x.h"
#include "OLED_Font.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "W25Q64.h"
//==========================================================
//oled是128*64（横向128）
//1. 英文字符（8×16 点阵）
//每个字符宽 8 像素，高 16 像素
//宽度：128 / 8 = 16 列
//高度：64 / 16 = 4 行
//一共可显示：16 列 × 4 行 = 64 个英文字符
//2. 中文字符（16×16 点阵）
//每个汉字宽 16 像素，高 16 像素
//宽度：128 / 16 = 8 列
//高度：64 / 16 = 4 行
//一共可显示：8 列 × 4 行 = 32 个汉字
//==========================================================
/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
extern SemaphoreHandle_t I2CMutex;
void OLED_I2C_SendByte(uint8_t Byte)
{
	 uint8_t i;
//    xSemaphoreTake(I2CMutex, portMAX_DELAY);
	 for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
//	xSemaphoreGive(I2CMutex);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
整个屏幕有 8 个“页”，每页 8 行像素。
→ 8 页 × 8 行 = 64 行。

每页有 128 列。
→ 128 列 × 8 页 = 128×64 全屏。
先用setcursor,设定光标位置。按8页，128列。然后OLED_WriteData（即点亮这一页相关像素点）。由于字符占两页（即16），所以要分开显示上下两页））
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	xSemaphoreTake(I2CMutex, portMAX_DELAY);
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容,OLED_F8x16为1行16列的数组
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
    xSemaphoreGive(I2CMutex);
}


/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}



#define HZK16_FONT_SIZE     32      // 每个字占 32 字节（16x16）
#define HZK16_BASE_ADDR     0x1000 // HZK16 字库在 W25Q64 的起始地址

// 传入汉字 GB2312 两个字节，比如 '你' => 0xC4E3
int Read_HZK16_From_W25Q64(const uint8_t *gb2312_code, uint8_t *font_data) 
{
    uint8_t area = gb2312_code[0] - 0xA1; // 区码
    uint8_t index = gb2312_code[1] - 0xA1; // 位码

    if (area >= 94 || index >= 94) return -1;

    uint32_t offset = (area * 94 + index) * HZK16_FONT_SIZE;
    uint32_t flash_addr = HZK16_BASE_ADDR + offset;

    // 调用你的 W25Q64 读函数
   W25Q64_ReadData(flash_addr, font_data, HZK16_FONT_SIZE);
    return 0;
}



/** 将 16×16 点阵原始数据转换为 SSD1306 页格式 */
static void HZK16_To_SSD1306(
    const uint8_t *raw32,
    uint8_t outTop[16],
    uint8_t outBottom[16])
{
    for (uint8_t col = 0; col < 16; col++) {
        uint8_t byte_top = 0, byte_bottom = 0;
        for (uint8_t row = 0; row < 8; row++) {
            uint8_t byte = raw32[row * 2 + col / 8];
            uint8_t bit = (byte >> (7 - (col % 8))) & 0x01;
            byte_top |= bit << row;
        }
        for (uint8_t row = 0; row < 8; row++) {
            uint8_t byte = raw32[(row + 8) * 2 + col / 8];
            uint8_t bit = (byte >> (7 - (col % 8))) & 0x01;
            byte_bottom |= bit << row;
        }
        outTop[col] = byte_top;
        outBottom[col] = byte_bottom;
    }
}

/** 显示一个中文字符（16x16）
即一页8像素，一次性显示1页*16行
*/
void OLED_ShowChinese(uint8_t Line, uint8_t Column, const uint8_t *gb2312)
{
    uint8_t raw32[32];
    if (Read_HZK16_From_W25Q64(gb2312, raw32)) return;

    uint8_t colTop[16], colBot[16];
    HZK16_To_SSD1306(raw32, colTop, colBot);

    xSemaphoreTake(I2CMutex, portMAX_DELAY);

    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (uint8_t i = 0; i < 16; i++) OLED_WriteData(colTop[i]);

    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (uint8_t i = 0; i < 16; i++) OLED_WriteData(colBot[i]);

    xSemaphoreGive(I2CMutex);
}



/** 连续显示多个中文字符（每个字符占 2 列） */
void OLED_ShowChineseString(uint8_t Line, uint8_t Column, const char *gbStr)
{
    while (*gbStr) {
        if ((uint8_t)*gbStr >= 0xA1) {
            OLED_ShowChinese(Line, Column, (const uint8_t *)gbStr);
            Column += 2;
            gbStr += 2;
        } else {
            gbStr++; // 忽略非中文
        }
        if (Column > 16) break;
    }
}

/** 中英文混合显示（自动判断宽度） */
void OLED_ShowMixedString(uint8_t Line, uint8_t Column, const char *gbStr)
{
    while (*gbStr && Column <= 16) {
        if ((uint8_t)*gbStr >= 0xA1) {
            OLED_ShowChinese(Line, Column, (const uint8_t *)gbStr);
            Column += 2;
            gbStr += 2;
        } else {
            OLED_ShowChar(Line, Column, *gbStr);
            Column += 1;
            gbStr += 1;
        }
    }
}
