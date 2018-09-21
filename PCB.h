#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

struct PCB{
    int pid;
    int procArrivalTime;
    int procServiceTime;
    int remainingTime;
    int timeOnReady;
    int timeDone;
    int timeInRunning;
    int finishedTime;
    };

#endif // PCB_H_INCLUDED
