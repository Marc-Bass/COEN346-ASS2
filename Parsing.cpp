// parser


#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <queue>
#include <thread>
#include <ctime>
#include <iomanip>

#include <windows.h>

//#include <PCB.h>
using namespace std;

enum STATES { QUEUED, ARRIVED, PAUSED, STARTED, RESUMED, TERMINATED };

struct PCB {
	int pid, arrTime, burst_time, priority, startTime, currentWaitStart, cumulativeWait, quantumTime, quantumsUsed;
	HANDLE currentThread;
	string processName;
	STATES currentState;
};

list<string *> parseProcesses(ifstream &inputFile, string filename) {
	// Assumes processes are already in order of priority

	list<string *> argsList;
	string line;
	string * currentArg = new string;

	if (inputFile.is_open()){
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

	int vars[6] = {0, 0, 0, 0, 0, 0};
	PCB * pcbTemp = new PCB;
	jobIterator++; // skips # of processes for now
	while(jobIterator != jobList.end()){
		pcbTemp->pid = vars[0]++;
		pcbTemp->startTime = time(NULL);
		pcbTemp->currentWaitStart = 0;
		pcbTemp->cumulativeWait = 0;
		pcbTemp->quantumsUsed = 0;
		pcbTemp->currentState = QUEUED;
		pcbTemp->currentThread = CreateThread(NULL, 0, NULL, NULL, CREATE_SUSPENDED, NULL);
		// insert try and catch for erroneous inputs that are incomplete
		pcbTemp->processName = **jobIterator++;
		pcbTemp->arrTime = stoi(**jobIterator++);
		pcbTemp->burst_time = stoi(**jobIterator++);
		pcbTemp->priority = stoi(**jobIterator++);

		pcbTemp->quantumTime = pcbTemp->priority >= 100 ? ((140 - pcbTemp->priority) * 5) : ((140 - pcbTemp->priority) * 20);
		jobs.push(pcbTemp);
		pcbTemp = new PCB;
	}
	return jobs;
}


void runJobQueue(queue<PCB *> jobs) {
	PCB * temp;
	for (int i = 0; i < jobs.size(); i++) {
		temp = jobs.front();
		Sleep(temp->arrTime);
		//send somehow to active queue here
		jobs.pop();
	}
}
void displayJobs(queue<PCB *> jobs) {
	PCB * temp;
	int sumArrTime = 0;
	for (int i = 0; i < jobs.size(); i++) {
		temp = jobs.front();
		Sleep(temp->arrTime - sumArrTime);
		cout << "At " << temp->arrTime << " ms, this one goes.\n";
		cout << "StartTime: " << temp->startTime << "\tarrTime: " << temp->arrTime << endl;
		cout << "PID: " << temp->pid << "\tprocessName: " << temp->processName << "\tpriority: " << temp->priority << "\tquantumTime: " << temp->quantumTime << endl;
		cout << "arrTime: " << temp->arrTime << "\tburstTime: " << temp->burst_time << endl << endl;

		sumArrTime = temp->arrTime;
		jobs.pop();
		jobs.push(temp);
	}
}

void outputLog(ofstream &output, PCB &process, bool update) { 
	if (update) {
		output << "Time: " << process.arrTime << ",\t" << process.processName << ",\priority updated to " << process.priority << endl;
	}
	switch (process.currentState) {
		case ARRIVED: {
			output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tArrived\n";
			break;
		}
		case PAUSED: {
			output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tPaused\n";
			break;
		}
		case STARTED: {
			output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tStarted, Granted " << process.quantumTime << endl;
			break;
		}
		case RESUMED: {
			output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tResumed, Granted " << process.quantumTime << endl;
			break;
		}
		case TERMINATED: {
			output << "Time: " << process.arrTime << ",\t" << process.processName << ",\tTerminated\n";
			break;
		}
		default: {
			cerr << "Invalid State or QUEUED\n";
			break;
		}
	}

}

int main() {
	string filename = "C:\\Users\\Marc\\Dropbox\\Year 4\\COEN 346\\Code\\COEN346-ASS2\\input.txt";
	ifstream input(filename);
	queue<PCB *> jobs = createJobQueue(input, filename);
	char yn = 'n';
	do {
		displayJobs(jobs);
		cout << "Again? y or n?\n";
		cin >> yn;
	} while (yn == 'y' || yn == 'Y');


}