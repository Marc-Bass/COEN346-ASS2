
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
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	
    
public:
    
    ~PCB();
    PCB(string, duration, duration, HANDLE *, unsigned int);
    unsigned int getdPID();
    string getName();
    HANDLE * getProcessThread();
	duration getArrivalTime();
	duration getBurstTime();
	duration getQuantumTime();
    void setQuantumTime(duration);
    unsigned int getPriority();
    void setPriority(unsigned int);
    unsigned int getCPUCycles();
    void incCPUCycles();
	clock::time_point getLastRun();
    void updateLastRun(clock::time_point);
	void setLastRun(clock::time_point);
    state getProcessState();
    void setProcessState(state);
	bool operator<(PCB &);
	clock::time_point getStartTime();
	void setStartTime(clock::time_point);


    
private:
    const unsigned int PID;
    const string processName;
    HANDLE * processThread;
    duration arrivalTime;
	duration burstTime;
	duration quantumTime;
	clock::time_point startTime;
    unsigned int priority;
    unsigned int cpuCycles;
	clock::time_point lastRun;
    state processState;
    mutex startSignal;
    
};

struct priorityComparaison{
	bool operator () (PCB * left, PCB * right) const{
		return (left->getPriority() > right->getPriority());
	}
};

struct arrivalComparison {
	bool operator () (PCB * left, PCB * right) const {
		return (left->getArrivalTime() > right->getArrivalTime());
	}
};

#endif /* PCB_hpp */
