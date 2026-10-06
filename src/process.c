#include"../include/process.h"
#include"../include/cpu.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


ProcessInfo *GetProcessList()
{
    int cpuTickNb = GetCPUTicks();

    FILE *pipe = popen("ps -ely", "r");
    if (!pipe) 
    {
        perror("popen failed");
        return NULL;
    }

    ProcessInfo* head = (ProcessInfo*)malloc(sizeof(ProcessInfo));
    if(!head)
    {
        pclose(pipe);
        return NULL;
    }

    head->next = NULL;
    head->name = NULL;

    char buffer[500];
    fgets(buffer, sizeof(buffer), pipe);


    while (fgets(buffer, sizeof(buffer), pipe) != NULL) 
    {
        ProcessInfo* process = (ProcessInfo*)malloc(sizeof(ProcessInfo));
        if(!process)
            break;
        process->next = NULL;
        char state;
        int uid, pid, ppid, c, pri, ni, rss, sz;
        char wchan[50], tty[50], time[50], name[50];
        if(sscanf(buffer, "%c %d %d %d %d %d %d %d %d %s %s %s %s",&state, &uid, &pid, &ppid, &c, &pri, &ni, &rss, &sz, wchan, tty, time, name) == 13)
        {
            process->state = state;
            process->pid = pid;
            process->name = strdup(name);

            process->nbTicksA = ReadStat(process->pid);
            process->memoryInsideRAM = GetMemoryInsideRAM(process->pid);

            if(!AddEndProcessList(head, process))
            {   
                free(process->name);
                free(process);
            }
        }
        else
            free(process);
    }

    pclose(pipe);
    return head;
}

int AddEndProcessList(ProcessInfo* head, ProcessInfo* newNode)
{
    if(!head || !newNode)
    {
        return 0;
    }

    ProcessInfo* tmp = head;

    if(!head->next)
    {
        head->next = newNode;
        return 1;
    }

    while(tmp->next)
    {
        tmp = tmp->next;
    }

    tmp->next = newNode;
    return 1;
}

unsigned long GetMemoryInsideRAM(int PID)
{
    unsigned long memory = 0;
    char path[50];
    snprintf(path, sizeof(path), "/proc/%d/status", PID);

    FILE *status = fopen(path, "r");
    if(!status)
        return 0;
    char line[256];
    while(fgets(line, sizeof(line), status))
    {
        if (sscanf(line, "VmRSS: %lu kB", &memory) == 1)
            break;
    }
    fclose(status);
    return memory;
}

unsigned long  ReadStat(int PID)
{
    char path[50];
    snprintf(path, sizeof(path), "/proc/%d/stat", PID);

    FILE *stat = fopen(path, "r");
    if(!stat)
        return 0;
    
    char line[1000];
    int pid, ppid, pgrp, session, tty_nr, tpgid;
    unsigned int flags;
    unsigned long minflt, cminflt, majflt, cmajflt;
    unsigned long long utime = 0, stime = 0;
    char name[50];
    char state;

    while(fgets(line, sizeof(line), stat))
    {
        char *open  = strchr(line, '(');
        char *close = strrchr(line, ')');
        if (!open || !close || close < open)
        {
            fclose(stat);
             return 0;
        }

        if (sscanf(close+2, "%c %d %d %d %d %d %u %lu %lu %lu %lu %llu %llu",
            &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime) == 13)
            break;
    }

    fclose(stat);
    return utime + stime;
}

void GetTicksB(ProcessInfo* head)
{
    if(!head)
        return;
    ProcessInfo* crrnt = head->next;
    int cpuTicks = GetCPUTicks();
    while(crrnt)
    {
        crrnt->nbTicksB = ReadStat(crrnt->pid);
        crrnt->cpuPer = CalculCPUPer(crrnt, cpuTicks);
        crrnt = crrnt->next;
    }
}

double CalculCPUPer(ProcessInfo* process, int cpuTicks)
{
    if(process->nbTicksB < process->nbTicksA)
        return 0;
    
    return 100 * (double)(process->nbTicksB - process->nbTicksA) / cpuTicks;
}

void PrintProcesses(ProcessInfo* head)
{
     if(!head)
        return;
    ProcessInfo* crrnt = head->next;
    // while(crrnt)
    // {
    //     usleep(100000);
    //     printf("PROCESS: %s, PID: %d, STATE: %c, NBTicks: %lu, Memory: %lu, CPU USAGE: %f\n", crrnt->name, crrnt->pid, crrnt->state, crrnt->nbTicksB, crrnt->memoryInsideRAM, crrnt->cpuPer);
    //     crrnt = crrnt->next;
    // }
    ProcessInfo** sorted = SortByUsage(head); 
    for(int i = 0; i < 15; i++)
    {
        printf("PROCESS: %s, PID: %d, STATE: %c, NBTicks: %lu, Memory: %lu, CPU USAGE: %f\n", sorted[i]->name, sorted[i]->pid, sorted[i]->state, sorted[i]->nbTicksB, sorted[i]->memoryInsideRAM, sorted[i]->cpuPer);
    }
}

ProcessInfo** SortByUsage(ProcessInfo* head)
{

    ProcessInfo ** sorted = (ProcessInfo**)malloc(sizeof(ProcessInfo*)*15);
    
    for(int i = 0; i < 15; i++)
    {
        sorted[i] = (ProcessInfo*)malloc(sizeof(ProcessInfo));
    }

    if(!head)
        return NULL;
    ProcessInfo* crrnt = head->next;
    while(crrnt)
    {
        for(int i = 0; i < 15; i++)
        {
            if(crrnt->cpuPer > sorted[i]->cpuPer)
            {
                ProcessInfo* tmp = sorted[i];
                sorted[i] = crrnt;
                for(int j = i+1;j < 14; j++)
                {
                    ProcessInfo* tmp2 = sorted[j+1];
                    sorted[j+1] = sorted[j];
                    sorted[j] = tmp;
                    tmp = tmp2;
                }
                break;
            }
        }
        crrnt = crrnt->next;
    }

    return sorted;
}

void FreeProcessList(ProcessInfo* head)
{
    ProcessInfo* tmp, *crrnt = head;
    while(crrnt)
    {
        tmp = crrnt;
        crrnt = crrnt->next;
        if(tmp->name)
            free(tmp->name);
        free(tmp);
    }
}