#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

class TestLock
{
	USE_LOCK;
public :
	int32 TestRead()
	{
		READ_LOCK;
		if (_que.empty() == false)
		{
			return _que.front();
		}
		else
		{
			return -1;
		}
	}
	void TestPush()
	{
		WRITE_LOCK;
		_que.push(1);
	}
	void TestPop()
	{
		WRITE_LOCK;
		if (_que.empty() == false)
		{
			_que.pop();
		}
	}
private:
	queue<int32> _que;
};

TestLock testLock;
void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
	}
}
void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value<< endl;
		this_thread::sleep_for(1ms);
	}
}
int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}
	GThreadManager->Join();
}

