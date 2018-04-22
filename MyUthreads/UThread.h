#pragma once

#include <Windows.h>

typedef VOID *UT_ARGUMENT;
typedef VOID(*UT_FUNCTION)(UT_ARGUMENT);


VOID UtInit();

/* 
 * Create a user thread to run a specific function. The thread will be placed
 * at the end of the Ready Queue.
 */
HANDLE UtCreate(UT_FUNCTION function, UT_ARGUMENT argument);


/*
 * Initialize the ReadyQueue
 */
VOID UtInit();

/*
 * Initializes the run of the user threads.  The initial operating system thread calls
 * a context switch to the first user thread and resumes execution only when there are
 * no more user threads left.
 */
VOID UtRun();

/*
 * The current thread yields it's running condition to the next thread in ReadyQueue.
 */
VOID UtYield();

/*
 * Releases all resources associated with the current thread.
 */
VOID UtExit();

/*
 * Activate a thread: Add the thread to the ReadyQueue tail, signaling that is ready 
 * to be executed.
 */
VOID UtActivate(HANDLE thread);
