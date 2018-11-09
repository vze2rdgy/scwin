#include "pch.h"
#include "SerialTaskScheduler.h"


using namespace Concurrency;


SerialTaskScheduler::SerialTaskScheduler()
{
	//hThread = CreateThread(NULL, 0, &SerialTaskScheduler::ThreadProc, &sharedData, CREATE_SUSPENDED, &threadId);
}


SerialTaskScheduler::~SerialTaskScheduler()
{
	if (hThread)
	{
		CloseHandle(hThread);
	}
}

void Concurrency::SerialTaskScheduler::schedule(TaskProc_t task, void * pParam)
{
	// task processes are added to a queue.
	// the thread will execute items from the queue.
}


DWORD Concurrency::SerialTaskScheduler::ThreadProc(LPVOID lpParam)
{
	return 0;
}
