#include "processQueue.hpp"


processQueue::~processQueue(){
    while (!queueObj.empty()) {
        delete queueObj.top();
        queueObj.pop();
    }
}

processQueue::processQueue(){
    isActive = false;
}

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
    return(queueObj.top());
}

void processQueue::push(PCB * newPCB){
    queueObj.push(newPCB);
}

void processQueue::pop(){
    queueObj.pop();
}
