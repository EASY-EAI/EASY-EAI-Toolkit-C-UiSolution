#ifndef __SYSTEMOPT_H__
#define __SYSTEMOPT_H__

#include "system_opt.h"

//system time parameter config api
extern void mSleep(int ms);
extern uint8_t calculateWeekDay(int y,int m, int d);

//system shell operation
extern int32_t get_SN(char *pSN, size_t SN_len);
extern uint32_t SYSTEM(const char * cmdstring);

#endif //__SYSTEMOPT_H__
