
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
	time_t getStartupTime();
   
private:

    processQueue * queueArray[2];
    mutex queueMutex[2];
    const string inputDirectory = "U:\\COEN346\\COEN346-ASS2\\COEN346-ASS2\\input.txt";
    ifstream inputFile;
    mutex inputMutex;
    const string outputDirectory = "output.txt";
    ofstream outputFile;
    mutex outputMutex;
	queue<PCB *> jobQueue;
	const chrono::high_resolution_clock::time_point startTime;
    
};

#endif /* processScheduler_hpp */
