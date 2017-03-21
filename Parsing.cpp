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


struct PCB {
	int pid, arrTime, burst_time, priority, startTime, currentWaitStart, cumulativeWait, quantumTime, quantumsUsed;
	thread * currentThread;
	string processName;
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
		pcbTemp->currentThread = new thread;
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



int main() {
	string filename = "C:\\Users\\Marc\\Dropbox\\Year 4\\COEN 346\\Code\\COEN346-ASS2\\input.txt";
	ifstream input(filename);
	queue<PCB *> jobs = createJobQueue(input, filename);
	float sumArrTime = 0;
	PCB * temp;
	while (jobs.size() > 0) {
		temp = jobs.front();	
		cout << "StartTime: " << temp->startTime << "\tarrTime: " << temp->arrTime << endl;
		cout << "PID: " << temp->pid << "\tprocessName: " << temp->processName << "\tpriority: " << temp->priority << "\tquantumTime: " << temp->quantumTime << endl;
		cout << "arrTime: " << temp->arrTime << "\tburstTime: " << temp->burst_time << endl << endl;
		Sleep(temp->arrTime - sumArrTime);
		sumArrTime += temp->arrTime;
		jobs.pop();
	}

	system("PAUSE");

}