#pragma once
#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	//스레드 실행 함수
	void Launch(function<void(void)> callback);
	//스레드 join 함수
	void Join();
	//TLS 관리 함수
	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			_lock;
	vector<thread>	_threads;

};

