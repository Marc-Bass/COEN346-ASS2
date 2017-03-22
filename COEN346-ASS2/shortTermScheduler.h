#pragma once
class shortTermScheduler
{
public:
	shortTermScheduler(ifstream &);
	~shortTermScheduler();
	list<string *> parseProcesses(ifstream &);
	queue<PCB *> createJobQueue(ifstream &);


private:
	queue<PCB *> jobQueue;
	ifstream inputFile;
};

