#ifndef CPU_INFO_H_
#define CPU_INFO_H_

typedef struct {
    unsigned long long total;
    unsigned long long idle;
}CpuInfo;


int ReadCPUInfo(CpuInfo* out);
double ReturnCPUPourc();

int GetCPUTicks();

#endif