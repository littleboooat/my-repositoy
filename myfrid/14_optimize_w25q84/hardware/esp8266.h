#ifndef __ESP8266_H
#define __ESP8266_H
void  esp8266_clear(void);
void esp8266_init(void);
uint8_t ESP8266_SendCmd(char* cmd,char* res);
void ESP8266_SendData(unsigned char *data, unsigned short len);
#define USART_DEBUG		USART1
unsigned char *ESP8266_GetIPD(unsigned short timeOut);
#endif
