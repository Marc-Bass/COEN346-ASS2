#include "jobsQueue.h"



jobsQueue::jobsQueue(){
	while (!jobQueueObj.empty()) {
		delete jobQueueObj.top();
		jobQueueObj.pop();
	}
}

jobsQueue::~jobsQueue() {
	
}


bool jobsQueue::empty() {
	return(jobQueueObj.empty());
}

unsigned long jobsQueue::size() {
	return(jobQueueObj.size());
}

PCB * jobsQueue::top() {
	return(jobQueueObj.top());
}

void jobsQueue::push(PCB * newPCB) {
	jobQueueObj.push(newPCB);
}

void jobsQueue::pop() {
	jobQueueObj.pop();
}