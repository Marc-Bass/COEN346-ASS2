
#ifndef PCB_hpp
#define PCB_hpp

#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <mutex>
#include <chrono>

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
	chrono::high_resolution_clock::time_point getLastRun();
    void updateLastRun();
	void setLastRun(chrono::high_resolution_clock::time_point);
    state getProcessState();
    void setProcessState(state);
	bool operator<(PCB &);
	chrono::high_resolution_clock::time_point getStartTime();
	void setStartTime(chrono::high_resolution_clock::time_point);


    
private:
    const unsigned int PID;
    const string processName;
    HANDLE * processThread;
    time_t arrivalTime;
    time_t burstTime;
    time_t quantumTime;
	chrono::high_resolution_clock::time_point startTime;
    unsigned int priority;
    unsigned int cpuCycles;
	chrono::high_resolution_clock::time_point lastRun;
    state processState;
    mutex startSignal;
    
};

struct priorityComparaison
{
	bool operator () (PCB * left, PCB * right) const
	{
		return (left->getPriority() > right->getPriority());
	}
};

#endif /* PCB_hpp */
