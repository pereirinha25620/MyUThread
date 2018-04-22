#pragma once

#include <Windows.h>
#include "UThread.h"
#include "List.h"

/* Define a 64 KB stack frame for the thread */
#define STACK_SIZE	(16 * 4 * 1024)

typedef struct _THREAD_CTX {
	ULONG EDI;
	ULONG ESI;
	ULONG EBX;
	ULONG EBP;
	VOID(*RetAddr)();
} THREAD_CTX, *PTHREAD_CTX;

typedef struct _UTHREAD {
	PTHREAD_CTX		ThreadContext;
	LIST_ENTRY		Link;
	UT_FUNCTION		Function;
	UT_ARGUMENT		Arg;
	LPBYTE			Stack;
} UTHREAD, *PUTHREAD;
