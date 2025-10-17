#ifndef __RTC_H
#define __RTC_H
#include "time.h"
extern uint16_t mytime[6];
void myRTCsettime(void);
void myRTC_Init(void);
void myRTCreadtime(void);
extern time_t myUnix;
#endif
