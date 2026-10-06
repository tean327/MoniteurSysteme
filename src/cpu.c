#include"../include/cpu.h"
#include<stdio.h>
#include <unistd.h>


int ReadCPUInfo(CpuInfo* out)
{
    FILE *f = fopen("/proc/stat", "r");
    if (!f) {
        perror("fopen /proc/stat");
        return -1;
    }

    int found = 0;
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
    //On lit seulement la première ligne du fichier
    //Elle ressemble a qql chose comme ca: cpu  80124 226 36210 17283980 5488 0 276 0 0 0
    if (fscanf(f, "cpu %lu %lu %lu %lu %lu %lu %lu %lu\n", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) == 8)
        found++;

    out->total = user+nice+system+idle+iowait+irq+softirq+steal;
    out->idle = idle+iowait;
    fclose(f);

    return found == 1 ? 0 : -1;
}

double ReturnCPUPourc()
{
    CpuInfo a, b;

    if(ReadCPUInfo(&a) != 0)
    {
        fprintf(stderr, "Impossible de lire le cpu\n");
        return 1;
    }

    usleep(100000);

    if(ReadCPUInfo(&b) != 0)
    {
        fprintf(stderr, "Impossible de lire le cpuB\n");
        return -1;
    }


    unsigned long long total = b.total - a.total;
    unsigned long long idle = b.idle - a.idle;

    if (total == 0)
       return 0.0;
    return 100.0 * (double)(total - idle) / (double)total;
}


int GetCPUTicks()
{
    FILE *pipe = popen("getconf CLK_TCK", "r");
    if (!pipe) 
    {
        perror("popen failed");
        return 0;
    }

    int nbTick;
    if(fscanf(pipe, "%d", &nbTick) != 1)
        return 0;
        
    fclose(pipe);
    return nbTick;
}