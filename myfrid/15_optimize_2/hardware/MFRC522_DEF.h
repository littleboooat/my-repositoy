 /*RC522 常用宏定义汇总 - rc522_macros.h */

#ifndef __RC522_MACROS_H 
#define __RC522_MACROS_H

/* 操作结果状态码 */
#define MI_OK           0       // 操作成功
#define MI_NOTAGERR     1       // 没有卡片响应 
#define MI_ERR          2       // 操作失败

/* RC522 命令码 */
#define PCD_IDLE            0x00  // 无动作，取消当前命令
#define PCD_AUTHENT         0x0E  // 认证密钥
#define PCD_RECEIVE         0x08  // 接收数据 
#define PCD_TRANSMIT        0x04  // 发送数据
#define PCD_TRANSCEIVE      0x0C  // 发送并接收数据
#define PCD_RESETPHASE      0x0F  // 复位 RC522 
#define PCD_CALCCRC         0x03  // 计算 CRC
#define PCD_SOFTRESET  0x0F

/* RC522 寄存器地址（常用） */
#define CommandReg          0x01 
#define CommIEnReg          0x02
#define CommIrqReg          0x04 
#define ErrorReg            0x06 
#define Status1Reg          0x07 
#define Status2Reg          0x08 
#define FIFODataReg         0x09 
#define FIFOLevelReg        0x0A 
#define ControlReg          0x0C
#define BitFramingReg       0x0D
#define CollReg             0x0E 
#define ModeReg             0x11 
#define TxControlReg        0x14
#define CRCResultRegM       0x21 
#define CRCResultRegL       0x22
#define DivIRqReg          0x05
/* 发给卡片的操作命令 */
#define PICC_REQIDL         0x26  // 寻卡（未休眠） 
#define PICC_REQALL         0x52  // 寻所有卡
#define PICC_ANTICOLL       0x93  // 防冲突 
#define PICC_SELECTTAG      0x93  // 选卡命令
#define PICC_AUTHENT1A      0x60  // 验证 A 密钥 
#define PICC_AUTHENT1B      0x61  // 验证 B 密钥 
#define PICC_READ           0x30  // 读块（16字节） 
#define PICC_WRITE          0xA0  // 写块 
#define PICC_HALT           0x50  // 卡休眠
 
/* 密钥类型 */ #define KEY_A_TYPE          0 
#define KEY_B_TYPE          1
#define authModeA           0x60
#define authModeB           0x61
/* 卡片类型（ATQA 判断，可选） */ 
#define CARD_TYPE_MIFARE_S50    0x0400 
#define CARD_TYPE_MIFARE_UL     0x0200

/* 数据相关 */ 
#define MAX_LEN             16  // 最大读取长度（字节）
/* =========  MFRC522 寄存器地址  ========= */
/*  PAGE0：命令 / 状态区 */
#define CommandReg         0x01      /* 启动/停止命令       */  /*1*/
#define CommIEnReg         0x02      /* 中断使能            */  /*1*/
#define CommIrqReg         0x04      /* 中断标志            */  /*1*/
#define ErrorReg           0x06      /* 错误状态            */  /*1*/
#define Status2Reg         0x08      /* 收发器状态          */  /*1*/
#define FIFODataReg        0x09      /* 64 B FIFO 入口/出口 */  /*1*/
#define FIFOLevelReg       0x0A      /* FIFO 深度           */  /*1*/
#define ControlReg         0x0C      /* 控制寄存器          */  /*1*/
#define BitFramingReg      0x0D      /* 面向位帧调节        */  /*1*/

/*  PAGE1：通信参数区 */
#define ModeReg            0x11      /* 通用发送/接收模式   */  /*2*/
#define TxModeReg          0x12      /* 发送速率            */  /*2*/
#define RxModeReg          0x13      /* 接收速率            */  /*2*/
#define TxControlReg       0x14      /* 天线驱动控制        */  /*3*/
#define TxASKReg           0x15      /*  TxAutoReg寄存器：配置发射部分的自动调制参数*/ 
	
/*  PAGE2：定时器 / 模拟配置 */
#define RFCfgReg           0x26      /* 接收增益            */  /*5*/
#define TModeReg           0x2A      /* 定时器模式          */  /*6*/
#define TPrescalerReg      0x2B      /* 定时器预分频低 8 b  */  /*6*/
#define TReloadRegH        0x2C      /* 定时器重装值高 8 b  */  /*7*/
#define TReloadRegL        0x2D      /* 定时器重装值低 8 b  */  /*7*/
#define MFRC522_REG_RX_THRESHOLD			0x18
/* =========  MFRC522 命令字  ========= */
#define PCD_IDLE           0x00      /* 取消当前命令        */
#define PCD_CALCCRC        0x03
#define PCD_TRANSMIT       0x04
#define PCD_RECEIVE        0x08
#define PCD_TRANSCEIVE     0x0C
#define PCD_AUTHENT        0x0E      /* 密钥验证            */
#define PCD_RESETPHASE     0x0F      /* 软复位（SOFTRESET） */

/* =========  位掩码 / 常用宏  ========= */
#define BIT_FIFO_FLUSH     0x80      /* FIFOLevelReg bit7   */
#define BIT_START_SEND     0x80      /* BitFramingReg bit7  */
#define BIT_TX1_RF_EN      0x01      /* TxControlReg bit0   */
#define BIT_TX2_RF_EN      0x02      /* TxControlReg bit1   */

/* =========  自定义返回状态  ========= */
#define MI_OK              0
#define MI_NOTAGERR        1
#define MI_ERR             2
#endif  // __RC522_MACROS_H
