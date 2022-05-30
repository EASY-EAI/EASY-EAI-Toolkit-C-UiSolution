// ============================ Linux C ============================
#include <sys/wait.h>

#include <unistd.h>
#include <errno.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int32_t get_SN(char *pSN, size_t SN_len)
{
    char buf[20480] = {0};
    FILE *fp = fopen("/proc/cpuinfo", "r");
    char *p = NULL;
    int seek = 0;
    int ret = 0;

    //读uid
    if (fp == NULL) {
            fprintf(stderr, "fopen error.\n");
            goto end;
    }

    while (!feof(fp)) {
            ret = fread(buf+seek, 1, 1024, fp);
            seek += ret;
            //printf("ret:%d\n", ret);
    }
    fclose(fp);

    if ((p = strstr(buf, "Serial\t\t: "))) {
            //printf("found1!\n");
            p += (strlen("Serial\t\t: "));
    }else{
        goto end;
    }

    if((NULL != pSN) && (SN_len > 0)){
        SN_len = strlen(p) < SN_len ? strlen(p) : SN_len;
        memcpy(pSN, p, SN_len);
    }else{
        goto end;
    }
end:
    return 0;
}
