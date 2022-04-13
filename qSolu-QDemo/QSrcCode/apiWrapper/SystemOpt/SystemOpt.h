#ifndef __SYSTEMOPT_H__
#define __SYSTEMOPT_H__

#include "system_opt.h"

extern void mSleep(int ms);
extern uint8_t calculateWeekDay(int y,int m, int d);
extern uint32_t SYSTEM(const char * cmdstring);

#endif //__SYSTEMOPT_H__
