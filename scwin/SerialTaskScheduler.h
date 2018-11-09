#pragma once

#include <queue>

namespace Concurrency
{
	class SerialTaskScheduler : public scheduler_interface
	{
		HANDLE hThread;
		HANDLE hEvent;
		DWORD threadId;
		std::priority_queue<tuple<TaskProc_t, void*>> queue;

	private:
		static DWORD WINAPI ThreadProc(LPVOID lpParam);

	public:
		SerialTaskScheduler();
		~SerialTaskScheduler();
	
		virtual void schedule(TaskProc_t, void*) = 0;

	};

}