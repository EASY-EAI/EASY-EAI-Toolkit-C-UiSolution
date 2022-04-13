#ifndef INTERFACE_H
#define INTERFACE_H

#include "system.h"

#define TAKE_MIN(x, y)  (x>=y? y : x)
typedef void*(*ThreadEntry)(void *p);

extern int hexToi(const char *str);
extern void ascii2HexVal(int *val, unsigned char *hexData, int lenth);
extern void val2char(void *ret, int val, int bit_num);
extern int IsDecCharacter(char hex);
extern int ConvertDecStringToValue(const char *DecStr, int *value);
extern int ConvertDecStringToValue(const char *DecStr, unsigned long *value);
extern unsigned long readValueFromString(char *buff, const char *strTitle);
extern int SystemTime();
extern void SysDateTime(unsigned long *curDate, unsigned long *curTime);
extern void SetSysTime(int year, int mon, int day, int hour, int min, int second);
extern int CalculateDayNum(int year, int month);
extern unsigned char CalculateWeekDay(int y,int m, int d);
extern int getLocalIp(char *interface, char *ip, int ip_len);
extern void mSleep(int ms);
extern void uiCreatThread(void *args, ThreadEntry threadfunc);
extern int createThread(ThreadEntry entry, void* para, pthread_t *pid, int priority);
extern int createNormalThread(ThreadEntry entry, void *para, pthread_t *pid);
extern int SYSTEM(const char * cmdstring);


#endif // INTERFACE_H

