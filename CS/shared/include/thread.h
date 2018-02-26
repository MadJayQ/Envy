#pragma once
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class ThreadBase
{
protected:
	HANDLE m_hThread;
	DWORD m_thID;
	HANDLE m_hInterrupt;
	HANDLE m_hSingleStart;
};

template<typename T, typename Ret = void, typename Args = void*>
class Thread : public ThreadBase
{
	using Task = Ret(T::*)(Args);
private:
	static Ret exec(void* threadobj)
	{
		Thread<T, Ret, Args>* thread = static_cast<Thread<T, Ret, Args>*>(threadobj);
		return (thread->m_pObject->*thread->m_Task)(thread->m_Args);
	}

	Thread(const Thread<T, Ret, Args>& other) {}
	Thread<T, Ret, Args>& operator =(const Thread<T, Ret, Args>& other) {}

	T* m_pObject;
	Task m_Task;
	Args m_Args;

public:
	Thread() {}
	explicit Thread(T* object, Ret(T::* method)(Args), Args args)
	{
		this->m_hThread = NULL;
		this->m_pObject = object;
		this->m_Task = method;
		this->m_hThread = 0;
		this->m_hInterrupt = CreateSemaphore(NULL, 1, 1, NULL);
		this->m_hSingleStart = CreateMutex(NULL, FALSE, NULL);
		this->m_Args = args;
	}

	~Thread()
	{
		if (m_hInterrupt)
			CloseHandle(m_hInterrupt);
		if (m_hThread)
			CloseHandle(m_hThread);
	}

	bool Start()
	{
		__try
		{
			if (WaitForSingleObject(m_hSingleStart, 0) != WAIT_OBJECT_0)
				return false;
			if (m_hThread)
			{
				if (WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT)
				{
					return false;
				}
			}
			WaitForSingleObject(m_hInterrupt, 0);
			m_hThread = CreateThread(
				NULL,
				0,
				(LPTHREAD_START_ROUTINE)Thread<T, Ret, Args>::exec,
				this,
				0,
				&this->m_thID
			);
			if (m_hThread)
				return true;
			return false;
		}
		__finally
		{
			ReleaseMutex(m_hSingleStart);
		}
	}
	inline void Join()
	{
		WaitForSingleObject(m_hThread, INFINITE);
	}

	inline bool Interrupt()
	{
		if (m_hInterrupt)
		{
			return (!(ReleaseSemaphore(m_hInterrupt, 1, NULL) == FALSE));
		}
		return false;
	}
	inline bool IsInterrupted(uintptr_t ms = 0)
	{
		if (WaitForSingleObject(m_hInterrupt, ms) == WAIT_TIMEOUT)
		{
			return false;
		}
		ReleaseSemaphore(m_hInterrupt, 1, NULL);
		return true;
	}

	inline bool IsRunning()
	{
		auto exitcode = 0UL;
		if (m_hThread)
			GetExitCodeThread(m_hThread, &exitcode);
		
		return (exitcode == STILL_ACTIVE);
	}
	__declspec(property(get = GetThreadHandle)) HANDLE ThreadHandle;
	inline HANDLE GetThreadHandle()
	{
		return m_hThread;
	}
	// -----------------------------------------------------------------------------
	__declspec(property(get = GetThreadID)) DWORD ThreadID;
	inline DWORD GetThreadID()
	{
		return m_thID;
	}
};
