
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
    
public:
    ~processScheduler();
    processScheduler();
    void shortTermScheduler();
    void longTermScheduler();
    void flipQueues();
	list<string *> parseProcesses();
	void createJobQueue();
    time_t getStartupTime();
	void displayJobs();
	//void outputLog(ofstream &, PCB &, bool); // bool for update priority
    
private:
    const time_t schedulerStartupTime;
    processQueue * queueArray[2];
    mutex queueMutex[2];
    const string inputDirectory = "C:\\Users\\Marc\\Dropbox\\Year 4\\COEN 346\\Code\\COEN346-ASS2\\input.txt";
    ifstream inputFile;
    mutex inputMutex;
    const string outputDirectory = "C:\\Users\\Marc\\Dropbox\\Year 4\\COEN 346\\Code\\COEN346-ASS2\\output.txt";
    ofstream outputFile;
    mutex outputMutex;
	queue<PCB *> jobQueue;
    
};

#endif /* processScheduler_hpp */
