
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
    duration scheduledStart; //Time process is to be introduced to expired queue
	clock::time_point startTime; //Time process is *actually* pushed into expired queue
	duration burstTime; //Called "time slice" in assignment definition
	duration quantumTime; 
	float cumulativeRunTime;
	float cumulativeWaitTime;
    unsigned int priority;
    unsigned int cpuCycles;
	clock::time_point lastRun;
    state processState;
    
};

//Structs used for priority_queue
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
