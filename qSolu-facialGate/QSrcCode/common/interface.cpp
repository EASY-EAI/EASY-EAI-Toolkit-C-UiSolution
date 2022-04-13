#include "interface.h"

using namespace std;

int hexToi(const char *str)
{
   int tmp = 0;
   if((*str == '0') && (*(str+1) == 'x'))//跳过"0x"
       str+=2;
   while(isxdigit(*str)) {//'2'-'0'->2  ;'a'-'a'+10->10;'A'-'A'+10->10
       if(isdigit(*str)) {         //数字字符
           tmp = tmp*16 + (*str-'0');
       } else if(islower(*str)) {  //小写字母
           tmp = tmp*16+ (*str-'a'+10);
       } else {                    //大写字母
           tmp = tmp*16 + (*str-'A'+10);
       }
       str++;
   }

   return tmp;
}


unsigned char ascii2HexVal_single(unsigned char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    else
        return 0;
}
void ascii2HexVal(int *val, unsigned char *hexData, int lenth)
{
    int carry_bit = 0;
    for(int i = lenth - 1; i>=0; i--){
        //printf("carry_bit = %d, i = %d , hexData = %c\n",carry_bit,i, hexData[i]);
        *val += ascii2HexVal_single(hexData[i])*pow(16,carry_bit);
        carry_bit++;
    }
}

unsigned char val2char_single(unsigned char c)
{
    if (0<=c && c<=9)
        return c + '0';
    else if (10 <= c && c <= 15)
        return c-10 + 'A';
    else
        return ' ';
}

void val2char(void *ret, int val, int bit_num)
{
    char *pTemp;
    unsigned char *pOpt;

    if(NULL == ret)
        return ;

    pTemp = malloc(bit_num);
    if(NULL == pTemp)
        return ;

    memset(pTemp, '0', bit_num);
    pOpt = pTemp + bit_num;

    for(int i = 0; i < bit_num; i++){
        if(0 == val)
            break;

        pOpt--;
        *pOpt = val2char_single(val&0x0000000f);

        val = (val>>4);
    }

    memcpy(ret, pTemp, bit_num);

    free(pTemp);
}

/*********************************************************************
Function:
Description:
    判断一个字符是否是一个10进制的字符
Calls:
    Called By:
parameter:
    Return:
author:
********************************************************************/
int IsDecCharacter(char hex)
{
    if((hex >= '0') && (hex <= '9'))
    {
        return (hex - '0');
    }

    return -1;
}

/*********************************************************************
Function:
Description:
    将一个10进制的字符串，转换成其对应的数值
Calls:
    Called By:
parameter:
    Return:
author:
********************************************************************/
int ConvertDecStringToValue(const char *DecStr, int *value)
{
    if(DecStr && value)
    {
        unsigned int index = 0;
        int temp;

        *value = 0;
        while((temp = IsDecCharacter(DecStr[index])) >= 0)/*是10进制的字符*/
        {
            *value = *value * 10 + temp;
            index ++;
        }

        if(strlen(DecStr) == index)/*整个字符串分析完成*/
        {
            return 0;
        }
    }

    return -1;
}

/*********************************************************************
Function:
Description:
    将一个10进制的字符串，转换成其对应的数值
Calls:
    Called By:
parameter:
    Return:
author:
********************************************************************/
int ConvertDecStringToValue(const char *DecStr, unsigned long *value)
{
    if(DecStr && value)
    {
        unsigned int index = 0;
        int temp;

        *value = 0;
        while((temp = IsDecCharacter(DecStr[index])) >= 0)//是10进制的字符
        {
            *value = *value * 10 + temp;
            index ++;
        }

        if(strlen(DecStr) == index)//整个字符串分析完成
        {
            return 0;
        }
    }

    return -1;
}

/*********************************************************************
Function:
Description:
    从一个字符串里提取参数
Example:
    value = readValueFromString((char *)buff, "Date:");
Calls:
    Called By:
parameter:
    Return:
author:
********************************************************************/
unsigned long readValueFromString(char *buff, const char *strTitle)
{
    char *p = NULL;
    int count_p = 0, count_cValue = 0, buffLen = 0, titleLen = 0;

    unsigned long value = 0;
    char strValue[12] = {0};

    p = buff;
    buffLen = strlen(buff); //算上'\n'的
    titleLen = strlen(strTitle);

    while(count_p <= buffLen){
        if(0 == strncmp(p, strTitle, titleLen)){
            p += titleLen;

            while(('.' != *p) && (',' != *p) && (' ' != *p) &&
                   !(('A' <= *p) && (*p <= 'z')) &&
                   ((count_p + titleLen + count_cValue) < buffLen)){
                p++;
                count_cValue++;
            }
            p -= count_cValue;

            memcpy(strValue, p, count_cValue);
            break;
        }
        p++;
        count_p++;
    }
    ConvertDecStringToValue(strValue, &value);

    return value;
}

// 系统当前时间戳
int SystemTime()
{
    time_t t;
    t = time(NULL);

    return time(&t);
}

void SysDateTime(unsigned long *curDate, unsigned long *curTime)
{
    time_t timer;//time_t就是long int 类型
    struct tm *tblock;

    timer = time(NULL);
    tblock = localtime(&timer);

    *curDate = 10000 * (tblock->tm_year+1900) + 100 * (tblock->tm_mon + 1) + (tblock->tm_mday);
    *curTime = 10000 * (tblock->tm_hour) + 100 * (tblock->tm_min) + (tblock->tm_sec);
}

void SetSysTime(int year, int mon, int day, int hour, int min, int second)
{
    struct tm t;
    struct timeval tv;
    __time_t time;

    year -= 1900;
    t.tm_year = year;
    t.tm_mon = mon-1;
    t.tm_mday = day;

    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = second;

    time = mktime(&t);
    if(-1 == time){
        perror("mktime");
        return ;
    }

    tv.tv_sec = time;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
}

int CalculateDayNum(int year, int month)
{
    int DayNum;

    if (month == 4 || month == 6 || month == 9 || month == 11){
        DayNum = 30;
    } else if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            DayNum = 29;
        else
            DayNum = 28;
    } else {
        DayNum = 31;
    }

    return DayNum;
}

unsigned char CalculateWeekDay(int y,int m, int d)
{
    int iWeek;
    if(m==1||m==2) {
        m+=12;
        y--;
    }
    iWeek=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
#ifdef __TEST_SOFT__
    switch(iWeek)
    {
        case 0: printf("星期一\n"); break;
        case 1: printf("星期二\n"); break;
        case 2: printf("星期三\n"); break;
        case 3: printf("星期四\n"); break;
        case 4: printf("星期五\n"); break;
        case 5: printf("星期六\n"); break;
        case 6: printf("星期日\n"); break;
    }
#endif
    return iWeek;
}

int getLocalIp(char *interface, char *ip, int ip_len)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    memset(ip, 0, ip_len);
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0) {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    sprintf(ip, "%s", inet_ntoa(sin.sin_addr));

    close(sd);
    return 0;
}

void mSleep(int ms)
{
    usleep(ms * 1000);
}

void uiCreatThread(void *args, ThreadEntry threadfunc)
{
    pthread_t id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&id, &attr, threadfunc, args);
    pthread_attr_destroy(&attr);
}

int createThread(ThreadEntry entry, void* para, pthread_t *pid, int priority)
{
    pthread_attr_t attr;
    pthread_attr_t *pAttr;
    struct sched_param schedParam;
    pthread_attr_init(&attr);

    if(priority > 0)
    {
        schedParam.sched_priority = priority;
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        pthread_attr_setschedparam(&attr, &schedParam);
        pAttr = &attr;
    }
    else
    {
        pAttr = NULL;
    }

    if(0 == pthread_create(pid, pAttr, entry, para))//创建线程
    {
        pthread_attr_destroy(&attr);
        return 0;
    }

    pthread_attr_destroy(&attr);
    return -1;
}

int createNormalThread(ThreadEntry entry, void *para, pthread_t *pid)
{
    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(0 == pthread_create(&ThreadId, &attr, entry, para))
    {
        pthread_attr_destroy(&attr);
//        TRY_EVALUATE_POINTER(pid, ThreadId);
        if(pid){
            *pid = ThreadId;
        }

        return 0;
    }

    pthread_attr_destroy(&attr);
    return -1;
}
int SYSTEM(const char * cmdstring)
{
    pid_t pid;
    int status;

    if(cmdstring == NULL)
    {
        return (1);
    }

    if((pid = vfork())<0)
    {
        status = -1;
    }
    else if(pid == 0)	//子进中运行脚本命令
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        exit(127); //子进程正常执行则不会执行此语句
    }
    else		//父进程中等待子进程返回
    {
        while(waitpid(pid, &status, 0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }
    return status;
}
