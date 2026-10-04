#include"../include/memoryInfo.h"
#include"../include/cpu.h"
#include"../include/process.h"
//#include<include/ui.h>

#include<stdio.h>
#include<stdlib.h>

int main(void)
{
    GetCPUTicks();
    MemoryInfo* memInfo = (MemoryInfo*)malloc(sizeof(MemoryInfo));

    if(ReadMemoryInfo(memInfo) != 0)
    {
        fprintf(stderr, "Impossible de lire la memoire\n");
        return 1;
    }

    unsigned long used = memInfo->totalKb - memInfo->availableKb;
    printf("RAM : %lu / %lu Mo (%.1f %%)\n",
           used / 1024, memInfo->totalKb / 1024,
           100.0 * used / memInfo->totalKb);

    printf("CPU: %f%%\n", ReturnCPUPourc());
    free(memInfo);

    GetProcessList();
    return 0;
}