#include <stdio.h>
#include"../include/memoryInfo.h"

int ReadMemoryInfo(MemoryInfo *out)
{
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        perror("fopen /proc/meminfo");
        return -1;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof line, f)) {
        if (sscanf(line, "MemTotal: %lu kB", &out->totalKb) == 1)
            found++;
        else if (sscanf(line, "MemAvailable: %lu kB", &out->availableKb) == 1)
            found++;
    }
    fclose(f);

    return found == 2 ? 0 : -1;
}