#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct processInfo{
    int pid;
    char* name;
    char state;
    unsigned long memoryInsideRAM;
    unsigned long nbTicksA;
    unsigned long nbTicksB;
    double cpuPer;
    struct processInfo* next;
}ProcessInfo;

ProcessInfo* GetProcessList();
int AddEndProcessList(ProcessInfo* head, ProcessInfo* newNode);
void FreeProcessList(ProcessInfo* head);
unsigned long GetMemoryInsideRAM(int PID);
unsigned long ReadStat(int PID);
void GetTicksB(ProcessInfo* head);
double CalculCPUPer(ProcessInfo* process, int cpuTicks);
void PrintProcesses(ProcessInfo* head);
#endif