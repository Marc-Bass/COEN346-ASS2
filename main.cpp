#include <iostream>
#include <ctime>
#include <windows.h>
#include <chrono>
#include <thread>
#include "processScheduler.hpp"

using namespace std;

//Functions used for initializing threads
void longTermStart(processScheduler * schedulerObj) {
	schedulerObj->longTermScheduler();
}
void shortTermStart(processScheduler * schedulerObj) {
	schedulerObj->shortTermScheduler();
}

int main() {

	processScheduler * assignment2;
	assignment2 = new processScheduler();

	//Create new thread for shortTermScheduler
	HANDLE shortTermHandle = HANDLE(CreateThread(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)shortTermStart,
												assignment2,
												0,
												NULL));

	//Create new thread for longTermScheduler
	HANDLE longTermHandle = HANDLE(CreateThread(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)longTermStart,
												assignment2,
												0,
												NULL));

	//Wait for both threads to complete
	WaitForSingleObject(longTermHandle, INFINITE);
	WaitForSingleObject(shortTermHandle, INFINITE);

	//Delete dynamic memory, and pause for the user
	delete assignment2;
	system("pause");

    return(0);
}
