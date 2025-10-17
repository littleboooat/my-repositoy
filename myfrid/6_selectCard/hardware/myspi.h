#ifndef __MFR522_DRIVER_H
#define __MFR522_DRIVER_H
void SPI_int(void);
uint16_t SPI_TransimitReceive(uint16_t SDATA);
void MFR522_WriteReg(uint8_t addr,uint8_t val);
uint8_t MFR522_ReadReg(uint8_t addr);
#endif
