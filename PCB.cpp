
#include "PCB.hpp"
#include <ctime>

unsigned int PCB::processCounter = 0;

//Simplifies quitting.
//When queue objects delete, PCB deconstructors trigger
//When PCB deconstructor triggers threads are joined then deleted
PCB::~PCB(){
	WaitForSingleObject(processThread, INFINITE);
    delete processThread;
}

//PCB constructor automates most initial values
//name, burstTime, thread, and priority need to be supplied
PCB::PCB(string name, time_t arrival, time_t burst,  HANDLE * runThread, unsigned int initialPriority):
PID(processCounter++), arrivalTime(arrival), processName(name)
{
    burstTime = burst;
    processThread = runThread;
    priority = initialPriority;
    cpuCycles = 0;
    processState = newProcess;
    startSignal.lock();
    if(priority >139){
        priority = 0;
    }
    if (priority < 100){
        quantumTime = (140-priority)*20;
    }
    else{
        quantumTime = (140-priority)*5;
    }
}

//Gets and sets almost entirely self-explanatory
unsigned int PCB::getdPID(){
    return(PID);
}

string PCB::getName(){
    return(processName);
}

HANDLE * PCB::getProcessThread(){
    return(processThread);
}

time_t PCB::getArrivalTime(){
    return(arrivalTime);
}

time_t PCB::getBurstTime(){
    return(burstTime);
}

time_t PCB::getQuantumTime(){
    return(quantumTime);
}

void PCB::setQuantumTime(time_t newQuantum){
    quantumTime = newQuantum;
}

unsigned int PCB::getPriority(){
    return(priority);
}

void PCB::setPriority(unsigned int newPriority){
    priority = newPriority;
}

unsigned int PCB::getCPUCycles(){
    return(cpuCycles);
}

void PCB::incCPUCycles(){
    cpuCycles++;
}

chrono::high_resolution_clock::time_point PCB::getLastRun(){
    return(lastRun);
}

void PCB::updateLastRun(){
    //lastRun = time(0); // not now
}

state PCB::getProcessState(){
    return(processState);
}
void PCB::setProcessState(state newState){
    processState = newState;
}

void PCB::setLastRun(chrono::high_resolution_clock::time_point time){
	lastRun = time;
}

chrono::high_resolution_clock::time_point PCB::getStartTime(){
	return startTime;
}

void PCB::setStartTime(chrono::high_resolution_clock::time_point time){
	startTime = time;
}

/*
bool PCB::operator<(PCB * compPCB){
	return(true);
	
	if (this->getPriority() < compPCB -> getPriority()){
		return(false);
	}
	else{
		return(true);
	}
	

}*/

