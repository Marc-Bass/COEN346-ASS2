
#ifndef PCB_hpp
#define PCB_hpp

#include <stdio.h>
#include <thread>
#include <iostream>
#include <cmath>

using namespace std;

enum state { running, ready, terminated}; //new?

class PCB
{
public:
    
    ~PCB();
    PCB(string, time_t, thread *, unsigned int);
    pid_t getdPID();
    string getName();
    thread * getProcessThread();
    time_t getArrivalTime();
    time_t getBurstTime();
    time_t getQuantumTime();
    void setQuantumTime(time_t);
    unsigned int getPriority();
    void setPriority(unsigned int);
    unsigned int getCPUCycles();
    void incCPUCycles();
    time_t getLastRun();
    void updateLastRun();
    state getProcessState();
    void setProcessState(state);
    
private:
    const pid_t PID;
    const string processName;
    thread * processThread;
    time_t arrivalTime;
    time_t burstTime;
    time_t quantumTime;
    unsigned int priority;
    unsigned int cpuCycles;
    time_t lastRun;
    static pid_t processCounter;
    state processState;
    
};
#endif /* PCB_hpp */
