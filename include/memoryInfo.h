#ifndef MEMORY_INFO_H_
#define MEMORY_INFO_H_

typedef struct {
    unsigned long totalKb;
    unsigned long availableKb;
}MemoryInfo;


int ReadMemoryInfo(MemoryInfo* out);

#endif