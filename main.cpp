#include <iostream>
#include <ctime>
#include <windows.h>
#include <chrono>
#include "processScheduler.hpp"

/*
Mainly experimentation going on in here
ctime doesn't have ms level accuracy, looking into chrono
I THINK I've replaced all thread objects with HANDLE... no promises
*/

using namespace std;

static void outputTest(){
}

int main(){
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

	ass2.createJobQueue(); // parses input and sets the jobQueue
	ass2.displayJobs();
	system("pause");
    return(0);
}
