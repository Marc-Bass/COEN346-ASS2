
#include "PCB.hpp"
#include <ctime>



PCB::~PCB(){
    processThread->join();
    delete processThread;
}

PCB::PCB(string name, time_t burst, thread * runThread, unsigned int initialPriority):
PID(processCounter++), arrivalTime(time(0)), processName(name)
{
    burstTime = burst;
    processThread = runThread;
    priority = initialPriority;
    cpuCycles = 0;
    lastRun = arrivalTime;
    if (priority < 100){
        quantumTime = (140-priority)*20;
    }
    else{
        quantumTime = (140-priority)*5;
    }
}

pid_t PCB::getdPID(){
    return(PID);
}

string PCB::getName(){
    return(processName);
}

thread * PCB::getProcessThread(){
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

time_t PCB::getLastRun(){
    return(lastRun);
}

void PCB::updateLastRun(){
    lastRun = time(0);
}

state PCB::getProcessState(){
    return(processState);
}
void PCB::setProcessState(state newState){
    processState = newState;
}
