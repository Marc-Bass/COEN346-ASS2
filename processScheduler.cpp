#include <cmath>
#include <list>
#include <string>
#include "processScheduler.hpp"


processScheduler::~processScheduler(){
    delete queueArray[1];
    delete queueArray[0];
}

processScheduler::processScheduler() :
schedulerStartupTime(time(0))
{
    queueArray[0] = new processQueue;
    queueArray[1] = new processQueue;
    inputFile.open(inputDirectory);
    outputFile.open(outputDirectory);
}

void processScheduler::shortTermScheduler(){
	while (true) {
		queueMutex[0].lock();
		queueMutex[1].lock();
		if (queueArray[0]->empty()) {
			queueMutex[0].unlock();
			flipQueues();
			queueMutex[0].unlock();
			queueMutex[1].unlock();
			continue;
		}
		queueMutex[0].unlock();
		queueMutex[1].unlock();



	}
}

void processScheduler::longTermScheduler(){
	PCB * temp;
	for (int i = 0; i < jobQueue.size(); i++) {
		temp = jobQueue.front();
		Sleep(temp->getArrivalTime());
		//send somehow to active queue here
		jobQueue.pop();
	}
}

void processScheduler::flipQueues(){
	
	if (!queueArray[0]->checkActive()) {
		queueArray[0]->setActive(true);
		queueArray[1]->setActive(false);
	}
	else {
		queueArray[0]->setActive(false);
		queueArray[1]->setActive(true);
	}
	
}

time_t processScheduler::getStartupTime(){
    return(NULL);
}

list<string *> processScheduler::parseProcesses() {
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

void processScheduler::createJobQueue() {

	// if priority queue, it could accomodate an un-ordered input file given some modifications
	list<string *> jobList = parseProcesses();

	// Start the list
	list<string *>::iterator jobIterator = jobList.begin();

	int vars[4] = { 0, 0, 0, 0 }; // holds pid, arrival time, burst time, priority
	string name;
	jobIterator++; // skips # of processes for now

	PCB * pcbTemp;
	HANDLE tempHandle = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, NULL);
	// Adjust this with new constructors.
	while (jobIterator != jobList.end()) {
		name = **jobIterator++;
		vars[1] = stoi(**jobIterator++);
		vars[2] = stoi(**jobIterator++);
		vars[3] = stoi(**jobIterator++);
		pcbTemp = new PCB(name, vars[1], vars[2], &tempHandle, vars[3]);
		jobQueue.push(pcbTemp);
	}
	return;
}

void processScheduler::displayJobs() {
	PCB * temp;
	for (int i = 0; i < jobQueue.size(); i++) {
		temp = jobQueue.front();
		cout << "At " << temp->getArrivalTime() << " ms, this one goes.\n";
		cout << "LastRun: " << temp->getLastRun() << endl;
		cout << "PID: " << temp->getdPID() << "\tprocessName: " << temp->getName() << "\tpriority: " << temp->getPriority() << "\tquantumTime: " << temp->getQuantumTime() << endl;
		cout << "arrTime: " << temp->getArrivalTime() << "\tburstTime: " << temp->getBurstTime() << endl << endl;
		jobQueue.pop();
		jobQueue.push(temp);
	}
}


// Need to see if we can implement using states or other ways
//void processScheduler::outputLog(ofstream &output, PCB &process, bool update) {
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

