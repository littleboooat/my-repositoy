#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"
#include "w25qxx.h"

/*
	���ܣ�	ͨ��USART1������λ�����͵����ݣ�������д�뵽W25Q64��
	ƽ̨��	STM32ϵ��
	���������https://blog.csdn.net/qq_41906031/article/details/108266629
	
*/

//Ҫд�뵽W25Q64���ַ�������		�����ݽ��յı�־
extern uint32_t data_stat;	//���ڽ������ݸ���
extern uint8_t USART_RX_BUF[RECEIVE_DATA_LEN];
extern uint8_t start_flag;
extern uint8_t data_flag;


int main(void)
{
	u32 i = 0;
	uint32_t addr_start = 0;
	uint32_t file_len = 0;
	uint32_t receive_flag = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 					//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();
	uart_init(115200);													//���ô��ڲ�����Ϊ115200
	W25QXX_Init();														//W25QXX��ʼ��
	delay_ms(200);
	
	while(W25QXX_ReadID()!=W25Q64)										//��ⲻ��W25Q64
	{
		printf("W25Q65 Check Failed!\r\n");
	}

	printf("W25Qxx Successful initialization\r\n");						//��ʾW25Qxx�Ѿ���ʼ���ɹ�

	//��֤W25Q64�����������������Ƿ���ȷ
//	W25QXX_Read(USART_RX_BUF, 46*4096 , 4096); 
//	for(i=0; i<4096; i++)
//	{
//		if(USART_RX_BUF[i] > 0xf)
//			printf("%x ", USART_RX_BUF[i]);
//		else
//			printf("0%x ", USART_RX_BUF[i]);
//	}
//	while(1);

	
	USART_RX_BUF[0] = 0;												//�ϵ����ָ����ջ���������ֹ�ڲ����ݲ�ȷ������ж�ʧ��
	USART_RX_BUF[1] = 0;
	USART_RX_BUF[10] = 0;		
	while(1) 
	{	
		if(start_flag == 0)
		{
			if(USART_RX_BUF[0] == 0xFE && USART_RX_BUF[1] == 0xFF && USART_RX_BUF[10] == 0xFE)		//�ȴ���ָ�д���ַ���ļ�����
			{
				
				start_flag = 1;				//��մ��ڽ��ճ��ȣ�׼����������
				data_stat = 0;				//������ݽ���
				receive_flag = data_stat;	//

				addr_start = 0;	
				addr_start |= USART_RX_BUF[2];
				addr_start |= USART_RX_BUF[3] << 8;
				addr_start |= USART_RX_BUF[4] << 16;
				addr_start |= USART_RX_BUF[5] << 24;		//��ȡ������ݵ���ʼλ��

				file_len = 0;
				file_len |= USART_RX_BUF[6];
				file_len |= USART_RX_BUF[7] << 8;
				file_len |= USART_RX_BUF[8] << 16;
				file_len |= USART_RX_BUF[9] << 24;			//��ȡ����ļ�����
				delay_ms(5);
				
				printf("start---"); 													//������ʾ�Ѿ��յ����ݽ������� ׼����������
				printf("addr_start=%d file_size=%d", addr_start, file_len); 			//�Ӵ���Ҫд���ļ�����ʼ��ַ��Ҫд���ļ��Ĵ�С
				
				printf("Start erasing sectors\r\n"); 		//�Ӵ�����ʾҪ������Ҫд�������
				for(i=addr_start/RECEIVE_DATA_LEN; i<(file_len+addr_start)/RECEIVE_DATA_LEN + (((file_len+addr_start)%RECEIVE_DATA_LEN)>0?1:0); i++)		//������ҪЩ���ļ�������
				{
					W25QXX_Erase_Sector(i);												//��������
					printf("Erasing sector %d\r\n", i);
					delay_ms(150);
				}
				printf("Sector erasing is complete\r\n");
				printf("\r\nStart writing data\r\n");
				printf("%s","NA");  													//��Ӧ��λ���Ѿ����յ�������������Ѿ�׼���õȴ�������λ�����͵�����
			}

		}
		else if(start_flag == 1)														//��ʼ��W25Qxx��д������
		{
			if(data_stat % RECEIVE_DATA_LEN == 0 && receive_flag != data_stat)			//�ӻ������ж�ȡ���յ��ļ������ݿ飬д��W25Qxx��		ע�ͣ������һ���ļ���Ϊ4096Byte
			{
				W25QXX_Write_NoCheck(USART_RX_BUF, addr_start + data_stat-RECEIVE_DATA_LEN, RECEIVE_DATA_LEN);
				receive_flag = data_stat;
				delay_ms(50);
				printf(" %d%s", data_stat, "A");										//��Ӧ��λ���Ѿ����������е����ݿ�д��W25Qxx�еȴ�������һ�����ݿ�
			}
			else if(data_stat >= file_len)
			{
				W25QXX_Write_NoCheck(USART_RX_BUF, addr_start + data_stat-(file_len%RECEIVE_DATA_LEN), file_len%RECEIVE_DATA_LEN);			//д���ļ����һ���� �����ݿ鲻�����������ݿ�С��4096Byte
				printf("\r\n%s", "Write to file successfully-----");
				printf("File Size %d Byte", data_stat);		
				while(1);																//�����������
			}
		}
	}
}

