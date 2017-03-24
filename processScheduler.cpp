#include <cmath>
#include <list>
#include <string>
#include "processScheduler.hpp"
#include <chrono>



processScheduler::~processScheduler(){
	if (!queueArray[1]->empty())
		//delete queueArray[1];
	if (!queueArray[0]->empty())
		//delete queueArray[0];
	inputFile.close();
	outputFile.close();
}

processScheduler::processScheduler() :
schedulerStartupTime(clock::now())
{
    queueArray[0] = new processQueue;
    queueArray[1] = new processQueue;
	jobQueue = new 	priority_queue<PCB *, vector<PCB *>, arrivalComparison>;
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

void processScheduler::flipQueues() {

	if (!queueArray[0]->checkActive()) {
		queueArray[0]->setActive(true);
		queueArray[1]->setActive(false);
	}
	else {
		queueArray[0]->setActive(false);
		queueArray[1]->setActive(true);
	}

}

void processScheduler::longTermScheduler(){

	PCB * temp;
	int sumArrTime = 0;
	while (!jobQueue->empty()) {
		temp = jobQueue->top();

		cout << "Wait for: " << temp->getArrivalTime().count() - sumArrTime << endl;
		Sleep(temp->getArrivalTime().count() - sumArrTime);
		sumArrTime = temp->getArrivalTime().count();

		temp->setStartTime(clock::now());
		temp->setLastRun(clock::now());

		while (true) {
			if (queueMutex[0].try_lock()) {
				if (!queueArray[0]->checkActive()) { // if index 0 is expired queue
					queueArray[0]->push(temp);
				}
				queueMutex[0].unlock();
			}
			else {
				if (!queueArray[1]->checkActive()) {
					queueArray[1]->push(temp);
				}
				queueMutex[1].unlock();
			}
		}
		outputLog(ARRIVED, temp, false);
		jobQueue->pop();
	}
}

processScheduler::clock::time_point processScheduler::getStartupTime(){
    return(schedulerStartupTime);
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
				else if (line[i] == '\t') { // skip tabs
					if (currentArg->length() > 0) { // dont blanks
						argsList.push_back(currentArg);
						currentArg = new string;
					}
					continue;
				}
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
	else
		cerr << "InputFile failed to open\n";
	return argsList;
}

void processScheduler::createJobQueue() {

	list<string *> jobList = parseProcesses();
	if (jobList.empty()) {
		cerr << "Empty job list or input parse failed\n";
		return;
	}

	// Start the list
	list<string *>::iterator jobIterator = jobList.begin();

	float vars[3] = { 0, 0, 0 }; // holds pid, arrival time, burst time, priority
	string name;
	jobIterator++; // skips # of processes for now

	PCB * pcbTemp;
	HANDLE tempHandle = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, NULL);
	// Adjust this with new constructors.
	while (jobIterator != jobList.end()) {
		name = **jobIterator++;
		vars[0] = stoi(**jobIterator++);
		vars[1] = stoi(**jobIterator++);
		vars[2] = stoi(**jobIterator++);
		pcbTemp = new PCB( name, duration(vars[0]), duration(vars[1]), &tempHandle, vars[2] );
		jobQueue->push(pcbTemp);
	}
	return;
}

void processScheduler::displayQueue(int index) { // 0/1 for active/expired, 2 for jobQueue
	if (index == 2) {
		displayJobQueue();
		return;
	}
	PCB * temp;
	if (queueArray[index]->empty()) {
		cout << "Empty\n";
		return;
	}
	processQueue * queue = queueArray[index];
	list<PCB *> * tempQueue = new list<PCB *>;
	while (!queue->empty()) {
		temp = queue->top();
		cout << "PID: " << temp->getdPID() << "\tprocessName: " << temp->getName() << "\tpriority: " << temp->getPriority() << "\tquantumTime: " << temp->getQuantumTime().count() << endl;
		cout << "arrTime: " << temp->getArrivalTime().count() << "\tburstTime: " << temp->getBurstTime().count() << endl;
		cout << "LastRun: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << "\tStartTime: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << endl << endl;
		queue->pop();
		tempQueue->push_back(temp);
	}
	while (!tempQueue->empty()) {
		queue->push(tempQueue->front());
		tempQueue->pop_front();
	}	
}

void processScheduler::displayJobQueue() {
	list<PCB *> * tempQueue = new list<PCB *>;
	PCB * temp;
	while (!jobQueue->empty()) {
		temp = jobQueue->top();
		cout << "PID: " << temp->getdPID() << "\tprocessName: " << temp->getName() << "\tpriority: " << temp->getPriority() << "\tquantumTime: " << temp->getQuantumTime().count() << endl;
		cout << "arrTime: " << temp->getArrivalTime().count() << "\tburstTime: " << temp->getBurstTime().count() << endl;
		cout << "LastRun: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << "\tStartTime: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << endl << endl;
		jobQueue->pop();
		tempQueue->push_back(temp);
	}
	while (!tempQueue->empty()) {
		jobQueue->push(tempQueue->front());
		tempQueue->pop_front();
	}
}

void processScheduler::outputLog(STATES state, PCB * process, bool update) {
	if (update) {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tpriority updated to " << process->getPriority() << endl;
	}
	switch (state) {
	case ARRIVED: {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tArrived\n";
		break;
	}
	case PAUSED: {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tPaused\n";
		break;
	}
	case STARTED: {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tStarted, Granted " << process->getQuantumTime().count() << endl;
		break;
	}
	case RESUMED: {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tResumed, Granted " << process->getQuantumTime().count() << endl;
		break;
	}
	case TERMINATED: {
		outputFile << "Time: " << process->getArrivalTime().count() << ",\t" << process->getName() << ",\tTerminated\n";
		break;
	}
	default: {
		cerr << "Invalid State or QUEUED\n";
		break;
	}
	}

}

