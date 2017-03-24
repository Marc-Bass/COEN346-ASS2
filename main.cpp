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

static void outputTest(){
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

	processScheduler ass2;

	/*thread threads[2];
	for (int i = 0; 1 < 2; i++) {
		threads[i] = std::thread(ass2.longTermScheduler());
	}*/
	ass2.longTermScheduler();
	cout << "Queue 0:\n";
	ass2.displayQueue(0);
	cout << "Queue 1:\n";
	ass2.displayQueue(1);

	system("pause");



    return(0);
}
