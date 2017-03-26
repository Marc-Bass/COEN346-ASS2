
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
	typedef std::chrono::time_point<chrono::high_resolution_clock, chrono::milliseconds> time_point2;
	typedef std::chrono::duration<float, std::milli> duration;
	
    
public:
    
    ~PCB();
    PCB(string, duration, duration, HANDLE *, unsigned int);
    unsigned int getdPID();
    string getName();
    HANDLE * getProcessThread();
	duration getScheduledStart();
	duration getBurstTime();
	duration getQuantumTime();
    void setQuantumTime(duration);
    unsigned int getPriority();
    void setPriority(unsigned int);
    unsigned int getCPUCycles();
    void incrementCPUCycles();
	time_point2 getLastRun();
	void setLastRun(time_point2);
    state getProcessState();
    void setProcessState(state);
	time_point2 getStartTime();
	void setStartTime(time_point2);
	float getCumulativeRunTime();
	void addCumulativeRunTime(float);
	float getCumulativeWaitTime();
	void addCumulativeWaitTime(float);
	void setCPUCycles(unsigned int cycles);

    
private:
    const unsigned int PID;
    const string processName;
    HANDLE * processThread;
    duration scheduledStart;
	time_point2 startTime;
	duration burstTime;
	duration quantumTime;
	float cumulativeRunTime;
	float cumulativeWaitTime;
    unsigned int priority;
    unsigned int cpuCycles;
	time_point2 lastRun;
    state processState;
    
};

struct priorityComparaison{
	bool operator () (PCB * left, PCB * right) const{
		return (left->getPriority() > right->getPriority());
	}
};

struct arrivalComparison {
	bool operator () (PCB * left, PCB * right) const {
		return (left->getScheduledStart() > right->getScheduledStart());
	}
};

#endif /* PCB_hpp */
