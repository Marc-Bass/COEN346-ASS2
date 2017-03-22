
#ifndef PCB_hpp
#define PCB_hpp

#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <mutex>

using namespace std;

enum state { newProcess, running, ready, terminated};

class PCB
{
    static unsigned int processCounter;
    
public:
    
    ~PCB();
    PCB(string, time_t, time_t, HANDLE *, unsigned int);
    unsigned int getdPID();
    string getName();
    HANDLE * getProcessThread();
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
    const unsigned int PID;
    const string processName;
    HANDLE * processThread;
    time_t arrivalTime;
    time_t burstTime;
    time_t quantumTime;
	time_t startTime;
    unsigned int priority;
    unsigned int cpuCycles;
    time_t lastRun;
    state processState;
    mutex startSignal;
    
};
#endif /* PCB_hpp */
