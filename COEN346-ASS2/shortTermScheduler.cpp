#include "shortTermScheduler.h"
#include <list>
#include <string>
#include <fstream>
#include <queue>
#include "PCB.hpp"
#include "processQueue.hpp"
using namespace std;

shortTermScheduler::shortTermScheduler()
{
}




shortTermScheduler::~shortTermScheduler()
{
}


list<string *> parseProcesses(ifstream &inputFile, string filename) {
	// Assumes processes are already in order of priority

	list<string *> argsList;
	string line;
	string * currentArg = new string;

	if (inputFile.is_open()) {
		while (getline(inputFile, line)) {

			for (size_t i = 0; i < line.length(); i++) {
				if (line[i] == ' ') { // space is encountered, or the next character is the newline character
					if (currentArg->length() > 0) { // dont blanks
						argsList.push_back(currentArg);
						currentArg = new string;
					}
					continue; // skip adding the space
				}
				else if (line[i] == '\t') // skip tabs
					continue;
				*currentArg += line[i];
			}
			// add last argument
			if (currentArg->length() > 0) { // dont blanks
				argsList.push_back(currentArg); // after the first command, add the full argument to the arguments array from temp;
				currentArg = new string;
			}

		}
		inputFile.close();
	}
	return argsList;
}

queue<PCB *> createJobQueue(ifstream &inputFile, string filename) {
	// if priority queue, it could accomodate an un-ordered input file given some modifications
	list<string *> jobList = parseProcesses(inputFile, filename);
	if (jobList.size() < 1) {
		cerr << "No processes\n";
		queue<PCB *> empty;
		return empty;
	}
	// Start the list
	list<string *>::iterator jobIterator = jobList.begin();
	queue<PCB *> jobs;

	int vars[6] = { 0, 0, 0, 0, 0, 0 };
	
	jobIterator++; // skips # of processes for now
	PCB * pcbTemp = new PCB();
	//while (jobIterator != jobList.end()) {
		//pcbTemp->pid = vars[0]++;
		//pcbTemp->startTime = time(NULL);
		//pcbTemp->currentWaitStart = 0;
		//pcbTemp->cumulativeWait = 0;
		//pcbTemp->quantumsUsed = 0;
		//pcbTemp->currentState = QUEUED;
		//pcbTemp->currentThread = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, NULL);
		//// insert try and catch for erroneous inputs that are incomplete
		//pcbTemp->processName = **jobIterator++;
		//pcbTemp->arrTime = stoi(**jobIterator++);
		//pcbTemp->burst_time = stoi(**jobIterator++);
		//pcbTemp->priority = stoi(**jobIterator++);

		//pcbTemp->quantumTime = pcbTemp->priority >= 100 ? ((140 - pcbTemp->priority) * 5) : ((140 - pcbTemp->priority) * 20);
		//jobs.push(pcbTemp);
		//pcbTemp = new PCB;
	}
	return jobs;
}
