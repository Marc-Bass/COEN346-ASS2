// COEN 346
// Assignment 2
// 
// 03/27/17
// Christopher Simpson
// Marc Bass


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
	cout << "Welcome to our process scheduler.\n";
	cout << "This scheduler will read the input.txt file from your working directory.\n";
	cout << "The job queue will be created and displayed after the input file is parsed.\n";
	cout << "The arrival in the epired queue and termination of each PCB will be displayed.\n\n";
	system("PAUSE");

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
	cout << "Thank you for using our process scheduler.\n";
	cout << "Created by Christopher Simpson and Marc Bass. 2017\n";
	system("PAUSE");
	delete assignment2;

    return(0);
}
