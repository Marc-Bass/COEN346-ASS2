#pragma once
class shortTermScheduler
{
public:
	shortTermScheduler();
	~shortTermScheduler();
	list<string *> parseProcesses(ifstream &, string);
	queue<PCB *> createJobQueue(ifstream &, string);


private:

};

