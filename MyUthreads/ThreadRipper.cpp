#include <Windows.h>
#include <stdio.h>
#include "UThread.h"


VOID function_thread1(UT_ARGUMENT arg) {
	printf("Running Thread 1 with Argument call: %d\n", (int)arg);

	

	UtYield();
}

int main() {
	UtInit();

	printf("Main has initilized and no thread was created yet.\n");

	UtCreate(function_thread1, (UT_ARGUMENT)(1));
	printf("Thread 1 was created but not launched.\n\n\n");

	printf("Starting to process thread:\n");
	UtRun();
	printf("*************** \n\n\n");

	printf("No more threads to process. Press any key to finish.\n");
	getchar();


}