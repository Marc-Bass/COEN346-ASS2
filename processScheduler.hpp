// COEN 346
// Assignment 2
// 
// 03/27/17
// Christopher Simpson
// Marc Bass


#ifndef processScheduler_hpp
#define processScheduler_hpp

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "processQueue.hpp"
#include <queue>
#include <list>


using namespace std;

class processScheduler{

	//States used for output.txt
	enum STATES { ARRIVED, RESUMED, PAUSED, TERMINATED, STARTED, UPDATED };
	//typdef's for timing analysis
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
	void outputLog(STATES, PCB *);
	clock::time_point getStartupTime();
	static void testFunction();
   
private:

    processQueue * queueArray[2]; //Contains expired and active queue
    mutex queueMutex[3]; // 0 and 1 are queueArray, 2 is for jobQueue
	const string inputDirectory = "input.txt";
    ifstream inputFile;
    mutex inputMutex;
	const string outputDirectory = "output.txt";
    ofstream outputFile;
    mutex outputMutex;
	const clock::time_point schedulerStartupTime; // Time the processScheduler object is created
	priority_queue<PCB *, vector<PCB *>, arrivalComparison> * jobQueue; // Uses arrivalComparison struct from PCB header to compare arrival times
	bool shortTermStart; // Allows the short term scheduler to start, triggers from long term scheduler.
    
};



#endif /* processScheduler_hpp */
