#pragma once
#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	//������ ���� �Լ�
	void Launch(function<void(void)> callback);
	//������ join �Լ�
	void Join();
	//TLS ���� �Լ�
	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			_lock;
	vector<thread>	_threads;

};

