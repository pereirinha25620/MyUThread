#include <Windows.h>
#include "UThreadInternal.h"

static
VOID __fastcall ContextSwitch(PUTHREAD CurrentThread, PUTHREAD NextThread);

static
VOID __fastcall InternalExit(PUTHREAD Thread, PUTHREAD NextThread);

PUTHREAD RunningThread;
PUTHREAD MainThread;

static LIST_ENTRY ReadyQueue;
DWORD RunningThreads;

static FORCEINLINE
PUTHREAD ExtractNextReadyThread() {
	return (IsListEmpty(&ReadyQueue) ?
		MainThread : CONTAINING_RECORD(RemoveHeadList(&ReadyQueue), UTHREAD, Link));
}

static VOID Schedule() {
	PUTHREAD NextThread;
	NextThread = ExtractNextReadyThread();
	ContextSwitch(RunningThread, NextThread);
}

static VOID InternalStart() {
	RunningThread->Function(RunningThread->Arg);
	UtExit();
}

VOID UtInit() {
	InitializeListHead(&ReadyQueue);
}

VOID UtRun() {
	/* Create a dummy thread to represent the underlying OS. */
	UTHREAD SysThread;

	/* If the ready queue is empty, return to main thread. */
	if (IsListEmpty(&ReadyQueue)) {
		return;
	}

	MainThread = &SysThread;
	RunningThread = MainThread;
	Schedule();
}

VOID UtYield() {
	if (!IsListEmpty(&ReadyQueue)) {
		InsertTailList(&ReadyQueue, &RunningThread->Link);
		Schedule();
	}
}

VOID UtExit() {
	RunningThreads -= 1;
	InternalExit(RunningThread, ExtractNextReadyThread());
}

VOID UtActivate(HANDLE thread) {
	InsertTailList(&ReadyQueue, &((PUTHREAD)thread)->Link);
}

HANDLE UtCreate(UT_FUNCTION function, UT_ARGUMENT argument) {
	PUTHREAD thread;
	
	/* Create a pointer to the new thread */
	//thread = (PUTHREAD)malloc(sizeof(UTHREAD));
	
	/* Create a new stack frame for the thread */
	//thread->Stack = (LPBYTE)malloc(STACK_SIZE);

	//memset(thread->Stack, 0, STACK_SIZE);

	thread = (PUTHREAD)VirtualAlloc(
		NULL,
		sizeof(UTHREAD) + 4*4096,
		MEM_RESERVE,
		PAGE_READWRITE
	);

	VirtualAlloc(
		thread,
		sizeof(UTHREAD),
		MEM_COMMIT,
		PAGE_READWRITE
	);

	thread->Stack = (LPBYTE) VirtualAlloc(
		(LPBYTE)thread + 2*4096,
		2*4096,
		MEM_COMMIT,
		PAGE_READWRITE
	);


	/* Hold the thread context withn the thread's stack frame */
	//thread->ThreadContext = (PTHREAD_CTX)(
	//	thread->Stack + STACK_SIZE				// The pointer is at the end of the stack frame
	//	- sizeof(ULONG)							// Ensure 4 bytes set to zero because of VS debugger
	//	- sizeof(THREAD_CTX)					// Save space for the ThreadContext
	//);
	thread->ThreadContext = (PTHREAD_CTX)(
		(LPBYTE)thread + 4 * 4096				// The pointer is at the end of the stack frame
		- sizeof(ULONG)							// Ensure 4 bytes set to zero because of VS debugger
		- sizeof(THREAD_CTX)					// Save space for the ThreadContext
	);

	thread->ThreadContext->EDI = 0x00000000;
	thread->ThreadContext->ESI = 0x11111111;
	thread->ThreadContext->EBX = 0x22222222;
	thread->ThreadContext->EBP = 0x33333333;
	thread->ThreadContext->RetAddr = InternalStart;

	thread->Function = function;
	thread->Arg = argument;

	RunningThreads += 1;
	UtActivate(thread);

	return (HANDLE)thread;
}

//
// Performs a context switch from CurrentThread to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX and NextThread in EDX.
// __declspec(naked) directs the compiler to omit any prologue or epilogue.
//
static
__declspec(naked)
VOID __fastcall ContextSwitch(PUTHREAD CurrentThread, PUTHREAD NextThread) {
	__asm {
		// Switch out the running CurrentThread, saving the execution context on the thread's own stack.   
		// The return address is atop the stack, having been placed there by the call to this function.
		//
		push	ebp
		push	ebx
		push	esi
		push	edi
		//
		// Save ESP in CurrentThread->ThreadContext.
		//
		mov		dword ptr[ecx].ThreadContext, esp
		//
		// Set NextThread as the running thread.
		//
		mov     RunningThread, edx
		//
		// Load NextThread's context, starting by switching to its stack, where the registers are saved.
		//
		mov		esp, dword ptr[edx].ThreadContext
		pop		edi
		pop		esi
		pop		ebx
		pop		ebp
		//
		// Jump to the return address saved on NextThread's stack when the function was called.
		//
		ret
	}
}

static
VOID __fastcall CleanupThread(PUTHREAD thread) {
	free(thread->Stack);
	free(thread);
}

//
// Frees the resources associated with CurrentThread and switches to NextThread.
// __fastcall sets the calling convention such that CurrentThread is in ECX and NextThread in EDX.
// __declspec(naked) directs the compiler to omit any prologue or epilogue.
//
__declspec(naked)
VOID __fastcall InternalExit(PUTHREAD CurrentThread, PUTHREAD NextThread) {
	__asm {

		//
		// Set NextThread as the running thread.
		//
		mov     RunningThread, edx

		//
		// Load NextThread's stack pointer before calling CleanupThread(): making the call while
		// using CurrentThread's stack would mean using the same memory being freed -- the stack.
		//
		mov		esp, dword ptr[edx].ThreadContext

		call    CleanupThread

		//
		// Finish switching in NextThread.
		//
		pop		edi
		pop		esi
		pop		ebx
		pop		ebp
		ret
	}
}