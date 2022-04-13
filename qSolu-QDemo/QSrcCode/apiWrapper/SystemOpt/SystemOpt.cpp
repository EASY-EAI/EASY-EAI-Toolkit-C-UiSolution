// ============================ Linux C ============================
#include <sys/wait.h>

#include <unistd.h>
#include <errno.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t calculateWeekDay(int y,int m, int d)
{
    uint8_t iWeek;
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

void mSleep(int ms)
{
    usleep(ms * 1000);
}

uint32_t SYSTEM(const char * cmdstring)
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
