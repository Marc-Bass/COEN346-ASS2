
#ifndef processScheduler_hpp
#define processScheduler_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "processQueue.hpp"
#include <queue>
#include <list>

using namespace std;

class processScheduler{

	enum STATES { ARRIVED, RESUMED, PAUSED, TERMINATED, STARTED };

public:
    ~processScheduler();
    processScheduler();
    void shortTermScheduler();
    void longTermScheduler();
    void flipQueues();
	list<string *> parseProcesses();
	void createJobQueue();
	void displayJobs();
	void displayQueue(int);
	void outputLog(STATES, PCB *, bool); // bool for update priority
	chrono::high_resolution_clock::time_point getStartupTime();
   
private:

    processQueue * queueArray[2];
    mutex queueMutex[2];
    const string inputDirectory = "U:\\coen346\\COEN346-ASS2\\input.txt";
    ifstream inputFile;
    mutex inputMutex;
    const string outputDirectory = "U:\\coen346\\COEN346-ASS2\\output.txt";
    ofstream outputFile;
    mutex outputMutex;
	const chrono::high_resolution_clock::time_point schedulerStartupTime;
	priority_queue<PCB *, vector<PCB *>, priorityComparaison> jobQueue;

    
};



#endif /* processScheduler_hpp */
