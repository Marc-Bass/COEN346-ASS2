#include <iostream>
#include <ctime>
#include <windows.h>
#include <chrono>
#include <thread>
#include "processScheduler.hpp"

/*
Mainly experimentation going on in here
ctime doesn't have ms level accuracy, looking into chrono
I THINK I've replaced all thread objects with HANDLE... no promises
*/

using namespace std;


void longTermStart(processScheduler * schedulerObj) {
	schedulerObj->longTermScheduler();
}

void shortTermStart(processScheduler * schedulerObj) {
	schedulerObj->shortTermScheduler();
}

int main() {
	//time_t before, after;
	//HANDLE testThread = CreateThread(NULL, 
	//								0, 
	//								(LPTHREAD_START_ROUTINE)outputTest,
	//								NULL, 
	//								CREATE_SUSPENDED, 
	//								NULL); 
	//ResumeThread(testThread);
	//before = time(NULL);
	//for (int i = 0; i < 5; i++) {
	//	cout << "timer\n";
	//	Sleep(10);
	//}
	//after = time(0);

	//cout << after << endl << before << endl << after - before << endl;



	//WaitForSingleObject(testThread, INFINITE);

	processScheduler * ass2;
	ass2 = new processScheduler();
	HANDLE shortTermHandle = HANDLE(CreateThread(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)shortTermStart,
												ass2,
												0,
												NULL));
	HANDLE longTermHandle = HANDLE(CreateThread(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)longTermStart,
												ass2,
												0,
												NULL));

	WaitForSingleObject(longTermHandle, INFINITE);
	WaitForSingleObject(shortTermHandle, INFINITE);

	delete ass2;

	system("pause");

    return(0);
}
