#include <cmath>
#include <list>
#include <string>
#include "processScheduler.hpp"
#include <chrono>



processScheduler::~processScheduler(){
	//Triggers deletion of dynamic memory
	delete queueArray[1];
	delete queueArray[0];

	//While loops used to brute-force unlock mutex or trigger crash if another thread locked it
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

	//Closing files & unlocking all mutexes
	inputFile.close();
	outputFile.close();

	queueMutex[0].unlock();
	queueMutex[1].unlock();
	queueMutex[2].unlock();

	outputMutex.unlock();
	inputMutex.unlock();
}

processScheduler::processScheduler() :
schedulerStartupTime(clock::now()) //Startup set on initialization
{
    queueArray[0] = new processQueue;
    queueArray[1] = new processQueue;
	jobQueue = new 	priority_queue<PCB *, vector<PCB *>, arrivalComparison>; //jobQueue priority based on scheduled arrival time
    inputFile.open(inputDirectory);
    outputFile.open(outputDirectory);
	queueArray[0]->setActive(true); //Start with queueArray[0] as active queue
	shortTermStart = false; 
}


void processScheduler::shortTermScheduler(){
	int activeQueue;
	int expiredQueue;
	PCB * activeProcess;
	float CPUTime;
	float remainingBurst;
	float processQuantum;

	//Busy wait until longTermScheduler indicates it's ready
	while (!shortTermStart) {
		//busyWait
	}

	//Begin shared queue manipulation
	queueMutex[0].lock();
	queueMutex[1].lock();

	while (!queueArray[0]->empty() || !queueArray[1]->empty() || !jobQueue->empty()) { //Short term quits when all three queues are empty

		//active queue control system. Reassigns variables and flips queues when necessary
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
			continue; //Restart loop
		}

		queueMutex[0].unlock();
		queueMutex[1].unlock();
		//End of shared queue manipulation

		//Keep a copy of the new PCB
		activeProcess = queueArray[activeQueue]->top();
		//Calculate the remaining burst time before the process completes
		remainingBurst = activeProcess->getBurstTime().count() - activeProcess->getCumulativeRunTime();
		//Access the pre-calculated time slice
		processQuantum = activeProcess->getQuantumTime().count();
		//If the remaining burst is less than the quantum time, scheduler terminates when burst is complete
		CPUTime = min(remainingBurst, processQuantum);


		if (activeProcess->getCumulativeRunTime() == 0) {
			outputLog(STARTED, activeProcess);
		}
		else {
			outputLog(RESUMED, activeProcess);
		}
		activeProcess->setProcessState(running);

		//Adds the time since the last run to the cumulative wait time
		activeProcess->addCumulativeWaitTime( (activeProcess->getLastRun() - clock::now()).count());

		//Resumes thread, sleeps scheduler for duration of CPUTime, then suspends the thread.
		ResumeThread(*(activeProcess->getProcessThread()));
		Sleep(CPUTime);
		SuspendThread(*(activeProcess->getProcessThread()));

		//Prepare to take this process out of context, including some house keeping
		activeProcess->setProcessState(ready);
		activeProcess->addCumulativeRunTime(CPUTime);
		activeProcess->incrementCPUCycles();
		activeProcess->setLastRun(clock::now());

		//Begin shared queue manipulation
		queueMutex[0].lock();
		queueMutex[1].lock();

		//Lists the process as terminated and deletes it if the process has reached, or exceeded its burst time
		if (activeProcess->getCumulativeRunTime() >= activeProcess->getBurstTime().count()) {
			activeProcess->setProcessState(terminated);
			outputLog(TERMINATED, activeProcess);
			queueArray[activeQueue]->pop();
			delete activeProcess;
		}
		
		//Otherwise, treat it as paused and evaluate priority and time slice
		else {
			outputLog(PAUSED, activeProcess);
			//Update priority and time slice(quantum) if process has run twice since the last update
			if (activeProcess->getCPUCycles() % 2 == 0) {
				int newPriority;
				int bonus;
				int oldPriority;
				int priorityMultiplier;
				duration newQuantum;

				oldPriority = activeProcess->getPriority();
				bonus = ceil((10 * activeProcess->getCumulativeWaitTime()) / (clock::now() - schedulerStartupTime).count());
				newPriority = max(100, min(oldPriority - bonus + 5, 139));
				activeProcess->setPriority(newPriority);
				priorityMultiplier = (140 - newPriority);
				if (newPriority < 100) {
					newQuantum = static_cast<duration>(priorityMultiplier * 20);
				}
				else {
					newQuantum = static_cast<duration>(priorityMultiplier * 5);
				}
				activeProcess->setQuantumTime(newQuantum);
				outputLog(UPDATED, activeProcess);
			}
			//Pop process off active and into expired
			queueArray[activeQueue]->pop();
			queueArray[expiredQueue]->push(activeProcess);
		}
		
	}
	//Unlock queue before exiting
	queueMutex[0].unlock();
	queueMutex[1].unlock();
}

//Flips active and expired queue flags
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
	// Uses two functions to parse the input and create the jobQueue, then displays
	createJobQueue();
	displayQueue(2);
	PCB * nextPCB;

	// Variables
	duration systemRunTime;
	float sleepTime;
	// Allow the shortTermScheduler to start
	shortTermStart = true;

	while (!jobQueue->empty()) { // For the entire job Queue
		systemRunTime = clock::now() - schedulerStartupTime; // Update systemRunTime before sleep time needs to be considered

		nextPCB = jobQueue->top();
		sleepTime = max(0, nextPCB->getScheduledStart().count() - systemRunTime.count()); // Can't be negative, hence max
		cout << "Wait for: " << sleepTime << endl;

		Sleep(sleepTime);

		queueMutex[0].lock();
		queueMutex[1].lock();

		// Set PCB variables for calculating bonus and time quantums
		nextPCB->setStartTime(clock::now());
		nextPCB->setLastRun(clock::now());
		
		// This checks for the expired array to push into
		if (queueArray[0]->checkActive()) {
			queueArray[1]->push(nextPCB);
		}
		else {
			queueArray[0]->push(nextPCB);
		}
		queueMutex[0].unlock();
		queueMutex[1].unlock();

		// Output to the log. There is a mutex built into the log.
		outputLog(ARRIVED, nextPCB);
		// Empty queue
		jobQueue->pop();
	}
}

processScheduler::clock::time_point processScheduler::getStartupTime(){
    return(schedulerStartupTime);
}

list<string *> processScheduler::parseProcesses() {
	// Takes the input file line by line and returns a string list of seperated arguments of the file
	list<string *> argsList; // Will contain the arguments for the parser
	string line;
	string * currentArg = new string;
	while (true) { // continue to try lock until open
		if (inputMutex.try_lock()) {
			if (inputFile.is_open()) { // check if the file opens properly
				while (getline(inputFile, line)) { // Takes each line to be paresed
					for (size_t i = 0; i < line.length(); i++) {
						if (line[i] == ' ' || line[i] == '\t') { // space is encountered or args are seperated with tabs, add to the list
							if (currentArg->length() > 0) { // dont blanks (double spaces)
								argsList.push_back(currentArg);
								currentArg = new string;
							}
							continue; // skip adding the space
						}
						*currentArg += line[i]; // add each character by character
					}
					// add last argument
					if (currentArg->length() > 0) { // dont blanks
						argsList.push_back(currentArg);
						currentArg = new string;
					}
				}
				inputFile.close();
			}
			else { // In case the file fails
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
	// Takes the parsed file's argument list and creates the PCBs for the jobs in the file
	list<string *> jobList = parseProcesses();
	if (jobList.empty()) {
		cerr << "Empty job list or input parse failed\n";
		return;
	}

	// Start the list
	list<string *>::iterator jobIterator = jobList.begin();
	unsigned int vars[3] = { 0, 0, 0 }; // holds arrival time, burst time, priority
	string name;
	int numberOfProcesses = stoi(**jobIterator++); // grabs the first argument, which is the # of processes.
	// numberOfProcesses supercedes the list limit. If the list is longer, only the first numberOfProcesses will be allowed. A shorter list will supercede the numberOfProcesses.
	PCB * pcbTemp; 
	HANDLE * tempHandle = new HANDLE(); // HANDLE for windows threads

	while (true) {
		if (queueMutex[2].try_lock()) { // queueMutex[2] is for the jobQueue
			while (jobIterator != jobList.end() && numberOfProcesses-- > 0) { //use list iteration to collect the values
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
				pcbTemp = new PCB(name, duration(vars[0]), duration(vars[1]), tempHandle, vars[2]); // Create the PCB and push into queue
				jobQueue->push(pcbTemp);
			}
			queueMutex[2].unlock();
			break;
		}
	}
	return;
}


void processScheduler::outputLog(STATES state, PCB * process) {
	outputMutex.lock();
	outputFile << fixed << setprecision(0); // Formatting
	switch (state) { // Allows for a varied input to outputLog for easy output
	case ARRIVED: {
		duration arrivalTime = process->getStartTime() - schedulerStartupTime;
		outputFile << "Time: "  << arrivalTime.count() << ",\t" << process->getName() << ",\tArrived\n";
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
	
	//Empty thread - can be filled if necessary but there's no point for this assignment

}



// Display functions for testing and viewing the queues
void processScheduler::displayQueue(int index) { // 0/1 for active/expired, 2 for jobQueue
	if (index == 2) { // since the two queues are priority different, we required this. 
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
		tempQueue->push_back(temp); // Collects them, as they would be auto sorted put back into the same queue
	}
	while (!tempQueue->empty()) { // swap back, allow sorting
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