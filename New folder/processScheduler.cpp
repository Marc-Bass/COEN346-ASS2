#include <cmath>
#include "processScheduler.hpp"


processScheduler::~processScheduler(){
    delete queueArray[1];
    delete queueArray[0];
}

processScheduler::processScheduler() :
schedulerStartupTime(time(0))
{
    queueArray[0] = new processQueue;
    queueArray[1] = new processQueue;
    inputFile.open(inputDirectory);
    outputFile.open(outputDirectory);
}

void processScheduler::shortTermScheduler(){
	while (true) {
		queueMutex[0].lock();
		queueMutex[1].lock();
		if (queueArray[0]->empty()) {
			queueMutex[0].unlock();
			flipQueues();
			queueMutex[0].unlock();
			queueMutex[1].unlock();
			continue;
		}
		queueMutex[0].unlock();
		queueMutex[1].unlock();



	}
}

void processScheduler::longTermScheduler(){
    
}

void processScheduler::flipQueues(){
	
	if (!queueArray[0]->checkActive()) {
		queueArray[0]->setActive(true);
		queueArray[1]->setActive(false);
	}
	else {
		queueArray[0]->setActive(false);
		queueArray[1]->setActive(true);
	}
	
}

time_t processScheduler::getStartupTime(){
    return(NULL);
}
