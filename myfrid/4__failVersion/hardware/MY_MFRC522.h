#ifndef __MY_MFRC522_DRIVER_H
#define __MY_MFRC522_DRIVER_H
void MFRC522_ClearBitMask(uint8_t addr,uint8_t mask);
void MFRC522_SetBitMask(uint8_t addr,uint8_t mask);
uint8_t MFRC522_ToCard(uint8_t Command,uint8_t *senddata,uint8_t dataNum,uint8_t *back_datas,uint8_t *backlen);
uint8_t MFRC522_Request(uint8_t reg_mode,uint8_t *back_bits);
void MFRC522_DumpRegs(void);
#endif
