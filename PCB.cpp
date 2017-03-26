
#include "PCB.hpp"
#include <ctime>

unsigned int PCB::processCounter = 0;

//Simplifies quitting.
//When queue objects delete, PCB deconstructors trigger
//When PCB deconstructor triggers threads are joined then deleted
PCB::~PCB(){
	
	ResumeThread(*processThread);
	WaitForSingleObject(*processThread, INFINITE);
	TerminateThread(*processThread, 0);
	CloseHandle(*processThread);
	delete processThread;
}

//PCB constructor automates most initial values
//name, burstTime, thread, and priority need to be supplied
PCB::PCB(string name, duration scheduledArrival, duration burst,  HANDLE * runThread, unsigned int initialPriority):
PID(processCounter++), scheduledStart(scheduledArrival), processName(name)
{
    burstTime = burst;
    processThread = runThread;
    priority = initialPriority;
    cpuCycles = 0;
    processState = newProcess;
	time_point2 init(chrono::time_point_cast<chrono::milliseconds>(clock::now()));
	startTime = init;
	lastRun = init;
	cumulativeRunTime = 0;
	cumulativeWaitTime = 0;
    if(priority >139){
        priority = 0;
    }
    if (priority < 100){
        quantumTime = duration((140-priority)*20);
    }
    else{
        quantumTime = duration((140-priority)*5);
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

PCB::duration PCB::getScheduledStart(){
    return(scheduledStart);
}

PCB::duration PCB::getBurstTime(){
    return(burstTime);
}

PCB::duration PCB::getQuantumTime(){
    return(quantumTime);
}

void PCB::setQuantumTime(duration newQuantum){
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

void PCB::incrementCPUCycles(){
    cpuCycles++;
}

PCB::time_point2 PCB::getLastRun(){
    return(lastRun);
}

state PCB::getProcessState(){
    return(processState);
}
void PCB::setProcessState(state newState){
    processState = newState;
}

void PCB::setLastRun(time_point2 time){
	lastRun = time;
}

PCB::time_point2 PCB::getStartTime(){
	return startTime;
}

void PCB::setStartTime(time_point2 time){
	startTime = time;
}

float PCB::getCumulativeRunTime(){
	return(cumulativeRunTime);
}

void PCB::addCumulativeRunTime(float extraTime) {
	cumulativeRunTime += extraTime;
}

float PCB::getCumulativeWaitTime() {
	return(cumulativeWaitTime);
}
void PCB::addCumulativeWaitTime(float extraTime) {
	cumulativeWaitTime += extraTime;
}

void PCB::setCPUCycles(unsigned int cycles) {
	cpuCycles = cycles;
}