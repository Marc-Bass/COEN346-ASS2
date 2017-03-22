#include "shortTermScheduler.h"
#include <list>
#include <string>
#include <fstream>
#include <queue>
#include "PCB.hpp"
#include "processQueue.hpp"
#include <windows.h>
using namespace std;

shortTermScheduler::shortTermScheduler(ifstream &input): inputFile(input)
{
	inputFile.open("input.txt");
	jobQueue = createJobQueue(inputFile);
}

shortTermScheduler::~shortTermScheduler()
{
}


list<string *> parseProcesses(ifstream &inputFile) {
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

queue<PCB *> createJobQueue(ifstream &inputFile) {
	// if priority queue, it could accomodate an un-ordered input file given some modifications
	list<string *> jobList = parseProcesses(inputFile);
	if (jobList.size() < 1) {
		cerr << "No processes\n";
		queue<PCB *> empty;
		return empty;
	}
	// Start the list
	list<string *>::iterator jobIterator = jobList.begin();
	queue<PCB *> jobs;

	int vars[4] = { 0, 0, 0, 0}; // holds pid, arrival time, burst time, priority
	string name;
	jobIterator++; // skips # of processes for now

	PCB * pcbTemp;

	// Adjust this with new constructors.
	while (jobIterator != jobList.end()) {
		name = **jobIterator++;
		vars[1] = stoi(**jobIterator++);
		vars[2] = stoi(**jobIterator++);
		vars[3] = stoi(**jobIterator++);
		pcbTemp = new PCB(name, vars[2], new thread, vars[3]);
		jobs.push(pcbTemp);
	}
	return jobs;
}

void runJobQueue(queue<PCB *> jobs) {
	PCB * temp;
	for (int i = 0; i < jobs.size(); i++) {
		temp = jobs.front();
		Sleep(temp->getArrivalTime());
		//send somehow to active queue here
		jobs.pop();
	}
}

void displayJobs(queue<PCB *> jobs) {
	PCB * temp;
	int sumArrTime = 0;
	for (int i = 0; i < jobs.size(); i++) {
		temp = jobs.front();
		cout << "At " << temp->getArrivalTime() << " ms, this one goes.\n";
		cout << "LastRun: " << temp->getLastRun() << endl;
		cout << "PID: " << temp->getdPID() << "\tprocessName: " << temp->getName() << "\tpriority: " << temp->getPriority() << "\tquantumTime: " << temp->getQuantumTime() << endl;
		cout << "arrTime: " << temp->getArrivalTime() << "\tburstTime: " << temp->getBurstTime() << endl << endl;
		jobs.pop();
		jobs.push(temp);
	}
}

//void outputLog(ofstream &output, PCB &process, bool update) {
//	if (update) {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\priority updated to " << process.priority << endl;
//	}
//	switch (process.currentState) {
//	case ARRIVED: {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tArrived\n";
//		break;
//	}
//	case PAUSED: {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tPaused\n";
//		break;
//	}
//	case STARTED: {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tStarted, Granted " << process.quantumTime << endl;
//		break;
//	}
//	case RESUMED: {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tResumed, Granted " << process.quantumTime << endl;
//		break;
//	}
//	case TERMINATED: {
//		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tTerminated\n";
//		break;
//	}
//	default: {
//		cerr << "Invalid State or QUEUED\n";
//		break;
//	}
//	}
//
//}
