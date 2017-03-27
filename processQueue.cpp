#include "processQueue.hpp"


processQueue::~processQueue(){
	// Empties queue manually in case of early termination
	PCB * nextPCB;
    while (!queueObj.empty()) {
		nextPCB = queueObj.top();
        queueObj.pop();
		delete nextPCB;
    }
}

processQueue::processQueue(){
    isActive = false;
}

//processes either interface directly with queue function calls or manage boolean

bool processQueue::checkActive(){
    return(isActive);
}

void processQueue::setActive(bool newBool){
    isActive = newBool;
}

bool processQueue::empty(){
    return(queueObj.empty());
}

unsigned long processQueue::size(){
    return(queueObj.size());
}

PCB * processQueue::top(){
	if (queueObj.empty()) {
		return(NULL);
	}
    return(queueObj.top());
}

void processQueue::push(PCB * newPCB){
    queueObj.push(newPCB);
}

void processQueue::pop(){
    queueObj.pop();
}
