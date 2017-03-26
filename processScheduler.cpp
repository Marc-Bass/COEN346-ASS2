#include <cmath>
#include <list>
#include <string>
#include "processScheduler.hpp"
#include <chrono>



processScheduler::~processScheduler(){
	delete queueArray[1];
	delete queueArray[0];
	while (!inputMutex.try_lock()) {
		inputMutex.unlock();
	}
	while (!outputMutex.try_lock()) {
		outputMutex.unlock();
	}
	while (!queueMutex[0].try_lock()) {
		queueMutex[0].unlock();
	}
	while (!queueMutex[1].try_lock()) {
		queueMutex[1].unlock();
	}
	while (!queueMutex[2].try_lock()) {
		queueMutex[2].unlock();
	}
	inputFile.close();
	outputFile.close();

	queueMutex[0].unlock();
	queueMutex[1].unlock();
	queueMutex[2].unlock();

	outputMutex.unlock();
	inputMutex.unlock();
}

processScheduler::processScheduler() :
schedulerStartupTime(clock::now())
{
    queueArray[0] = new processQueue;
    queueArray[1] = new processQueue;
	jobQueue = new 	priority_queue<PCB *, vector<PCB *>, arrivalComparison>;
    inputFile.open(inputDirectory);
    outputFile.open(outputDirectory);
	queueArray[0]->setActive(true);
	shortTermStart = false;
}


void processScheduler::shortTermScheduler(){
	int activeQueue;
	int expiredQueue;
	PCB * activeProcess;
	float CPUTime;
	float remainingBurst;
	float processQuantum;

	while (!shortTermStart) {
		//busyWait
	}

	queueMutex[0].lock();
	queueMutex[1].lock();

	while (!queueArray[0]->empty() || !queueArray[1]->empty() || !jobQueue->empty()) { //Short term quits when all three queues are empty
		if (queueArray[0]->checkActive()) {
			activeQueue = 0;
			expiredQueue = 1;
		}
		else {
			activeQueue = 1;
			expiredQueue = 0;
		}
		if (queueArray[activeQueue]->empty()) { //Checks if active queue is empty and flips them
			flipQueues();
			expiredQueue = activeQueue;
			activeQueue = (activeQueue + 1) % 2;
			queueMutex[0].unlock();
			queueMutex[1].unlock();
			queueMutex[0].lock();
			queueMutex[1].lock();
			continue;
		}

		queueMutex[0].unlock();
		queueMutex[1].unlock();

		activeProcess = queueArray[activeQueue]->top();
		remainingBurst = activeProcess->getBurstTime().count() - activeProcess->getCumulativeRunTime();
		processQuantum = activeProcess->getQuantumTime().count();
		CPUTime = min(remainingBurst, processQuantum); //Ends early if burst time remaining < quantumTime


		if (activeProcess->getCumulativeRunTime() == 0) {
			outputLog(STARTED, activeProcess);
		}
		else {
			outputLog(RESUMED, activeProcess);
		}
		activeProcess->setProcessState(running);
		activeProcess->addCumulativeWaitTime( (activeProcess->getLastRun() - clock::now()).count());

		cout << "Priority: " << activeProcess->getPriority() << "\tTs: " << activeProcess->getQuantumTime().count() << "\tWait: " << activeProcess->getCumulativeWaitTime() << endl;
		ResumeThread(*(activeProcess->getProcessThread()));
		Sleep(CPUTime);
		SuspendThread(*(activeProcess->getProcessThread()));


		activeProcess->setProcessState(ready);
		activeProcess->addCumulativeRunTime(CPUTime);
		activeProcess->incrementCPUCycles();
		activeProcess->setLastRun(clock::now());

		queueMutex[0].lock();
		queueMutex[1].lock();
		if (activeProcess->getCumulativeRunTime() >= activeProcess->getBurstTime().count()) {
			activeProcess->setProcessState(terminated);
			outputLog(TERMINATED, activeProcess);
			queueArray[activeQueue]->pop();
			delete activeProcess;
		}
		else {
			outputLog(PAUSED, activeProcess);
			if (activeProcess->getCPUCycles() == 2) {
				int newPriority;
				int bonus;
				int oldPriority;
				
				oldPriority = activeProcess->getPriority();
				bonus = ceil((10 * activeProcess->getCumulativeWaitTime()) / (clock::now() - schedulerStartupTime).count() );
				newPriority = max(100, min(oldPriority - bonus + 5, 139));

				activeProcess->setPriority(newPriority);
				// need to update the amount of time given
				if (newPriority < 100) {
					activeProcess->setQuantumTime(duration((140 - newPriority) * 20));
				}
				else {
					activeProcess->setQuantumTime(duration((140 - newPriority) * 5));
				}
				activeProcess->setCPUCycles(0);
				outputLog(UPDATED, activeProcess);
				//Need to actually programming the update here
			}
			queueArray[activeQueue]->pop();
			queueArray[expiredQueue]->push(activeProcess);
		}
		
	}
	queueMutex[0].unlock();
	queueMutex[1].unlock();
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
	createJobQueue();
	displayQueue(2);
	PCB * nextPCB;
	duration systemRunTime;
	float sleepTime;
	shortTermStart = true;
	while (!jobQueue->empty()) {
		systemRunTime = clock::now() - schedulerStartupTime; //Update systemRunTime before sleep time needs to be considered
		nextPCB = jobQueue->top();
		sleepTime = max(0, nextPCB->getScheduledStart().count() - systemRunTime.count()); //Can't be negative, hence max
		cout << "Wait for: " << sleepTime << endl;

		Sleep(sleepTime);

		queueMutex[0].lock();
		queueMutex[1].lock();

		nextPCB->setStartTime(clock::now());
		nextPCB->setLastRun(clock::now());
		
		if (queueArray[0]->checkActive()) {
			queueArray[1]->push(nextPCB);
		}

		else {
			queueArray[0]->push(nextPCB);
		}
		queueMutex[0].unlock();
		queueMutex[1].unlock();
		outputLog(ARRIVED, nextPCB);
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
	while (true) {
		if (inputMutex.try_lock()) {
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
			else {
				cerr << "InputFile failed to open\n";
				inputMutex.unlock();
				break;
			}
			inputMutex.unlock();
			break;
		}
	}	
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

	unsigned int vars[3] = { 0, 0, 0 }; // holds pid, arrival time, burst time, priority
	string name;
	int numberOfProcesses = stoi(**jobIterator++);
	PCB * pcbTemp;
	HANDLE * tempHandle = new HANDLE();
	// Could be optimized by creating a string and int vector to collect terms then try the lock
	while (true) {
		if (queueMutex[2].try_lock()) {
			while (jobIterator != jobList.end() && numberOfProcesses-- > 0) {
				name = **jobIterator++;
				vars[0] = stoi(**jobIterator++);
				vars[1] = stoi(**jobIterator++);
				vars[2] = stoi(**jobIterator++);
				tempHandle = new HANDLE(CreateThread(
													NULL, 
													0, 
													(LPTHREAD_START_ROUTINE)testFunction,
													NULL, 
													CREATE_SUSPENDED, 
													NULL));
				pcbTemp = new PCB(name, duration(vars[0]), duration(vars[1]), tempHandle, vars[2]);
				jobQueue->push(pcbTemp);
			}
			queueMutex[2].unlock();
			break;
		}
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
		cout << "arrTime: " << temp->getScheduledStart().count() << "\tburstTime: " << temp->getBurstTime().count() << endl;
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
		cout << "arrTime: " << temp->getScheduledStart().count() << "\tburstTime: " << temp->getBurstTime().count() << endl;
		cout << "LastRun: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << "\tStartTime: " << chrono::duration_cast<chrono::milliseconds>(temp->getLastRun() - schedulerStartupTime).count() << endl << endl;
		jobQueue->pop();
		tempQueue->push_back(temp);
	}
	while (!tempQueue->empty()) {
		jobQueue->push(tempQueue->front());
		tempQueue->pop_front();
	}
}

void processScheduler::outputLog(STATES state, PCB * process) {
	static int count;
	outputMutex.lock();
	switch (state) {
	case ARRIVED: {
		duration arrivalTime = process->getStartTime() - schedulerStartupTime;
		outputFile << "Time: " << arrivalTime.count() << ",\t" << process->getName() << ",\tArrived\n";
		break;
	}
	case PAUSED: {
		duration pauseTime = process->getLastRun() - schedulerStartupTime;
		outputFile << "Time: " << pauseTime.count() << ",\t" << process->getName() << ",\tPaused\n";
		break;
	}
	case STARTED: {
		duration startTime = clock::now() - schedulerStartupTime;
		outputFile << "Time: " << startTime.count() << ",\t" << process->getName() << ",\tStarted, Granted " << process->getQuantumTime().count() << endl;
		break;
	}
	case RESUMED: {
		duration resumeTime = clock::now() - schedulerStartupTime;
		outputFile << "Time: " << resumeTime.count() << ",\t" << process->getName() << ",\tResumed, Granted " << process->getQuantumTime().count() << endl;
		break;
	}
	case TERMINATED: {
		duration terminationTime = process->getLastRun() - schedulerStartupTime;
		outputFile << "Time: " << terminationTime.count() << ",\t" << process->getName() << ",\tTerminated\n";
		break;
	}
	case UPDATED: {
		duration updateTime = process->getLastRun() - schedulerStartupTime;
		outputFile << "Time: " << updateTime.count() << ",\t" << process->getName() << ",\tpriority updated to " << process->getPriority() << endl;
		break;
	}
	default: {
		cerr << "Invalid State or QUEUED\n";
		break;
		}
	}
	outputMutex.unlock();

}

void processScheduler::testFunction() {
	
	cout << "HELLO WORLD\n\n";
	ExitThread(0);
}

