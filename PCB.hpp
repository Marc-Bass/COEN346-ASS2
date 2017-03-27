
#ifndef PCB_hpp
#define PCB_hpp

#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <mutex>
#include <chrono>

using namespace std;

//States assigned to PCB's
enum state { newProcess, running, ready, terminated};

class PCB
{
	//static int used for unique pid's
    static unsigned int processCounter;
	//typedef's for time analysis
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
    
public:
    
    ~PCB();
    PCB(string, duration, duration, HANDLE *, unsigned int);
	//Almost entirely self-explanatory get/set functions
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
	clock::time_point getLastRun();
	void setLastRun(clock::time_point);
    state getProcessState();
    void setProcessState(state);
	clock::time_point getStartTime();
	void setStartTime(clock::time_point);
	float getCumulativeRunTime();
	void addCumulativeRunTime(float);
	float getCumulativeWaitTime();
	void addCumulativeWaitTime(float);

    
private:
    const unsigned int PID;
    const string processName;
    HANDLE * processThread;
    duration scheduledStart; // Time scheduled to arrived in expired queue from job Queue, written in the input.
	clock::time_point startTime; // Time process is *actually*(vs scheduled) pushed into expired queue.
	duration burstTime; // Total run time required by process
	duration quantumTime; // Called "time slice" in assignment definition
	float cumulativeRunTime;
	float cumulativeWaitTime;
    unsigned int priority;
    unsigned int cpuCycles;
	clock::time_point lastRun; // Time point of the end of the last run time, to calculate wait time. lastRun = startTime for the first cycle.
    state processState;
    
};

//Structs used for priority_queue, expired queue.
struct priorityComparaison{
	bool operator () (PCB * left, PCB * right) const{
		return (left->getPriority() > right->getPriority());
	}
};
// Used for job Queue
struct arrivalComparison {
	bool operator () (PCB * left, PCB * right) const {
		return (left->getScheduledStart() > right->getScheduledStart());
	}
};

#endif /* PCB_hpp */
