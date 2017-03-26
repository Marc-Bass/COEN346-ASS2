
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

	enum STATES { ARRIVED, RESUMED, PAUSED, TERMINATED, STARTED, UPDATED };
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;

public:
    ~processScheduler();
    processScheduler();
    void shortTermScheduler();
    void longTermScheduler();
    void flipQueues();
	list<string *> parseProcesses();
	void createJobQueue();
	void displayQueue(int); // 0/1 active/expired, 2 = jobQueue
	void displayJobQueue(); // called by displayQueue
	void outputLog(STATES, PCB *); // bool for update priority
	clock::time_point getStartupTime();
	static void testFunction();
   
private:

    processQueue * queueArray[2];
    mutex queueMutex[3]; // 0 and 1 are queueArray, 2 is for jobQueue
	const string inputDirectory = "input.txt";
    ifstream inputFile;
    mutex inputMutex;
	const string outputDirectory = "output.txt";
    ofstream outputFile;
    mutex outputMutex;
	const clock::time_point schedulerStartupTime;
	priority_queue<PCB *, vector<PCB *>, arrivalComparison> * jobQueue;
	bool shortTermStart;
    
};



#endif /* processScheduler_hpp */
