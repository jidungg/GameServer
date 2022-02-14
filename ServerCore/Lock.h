#pragma once
#include "Types.h"

//ReaderWriter SpinLock
class Lock
{
	enum : uint32
	{
		EMPTY_FLAG = 0x00000000,
		WRITE_THREAD_MASK = 0xFFFF0000,
		READ_COUNT_MASK = 0x0000FFFF,
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000
	};
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();
private:
	atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0; //ÁßÃ¸ writeLock ÃßÀû¿ë
};

class ReadLockGuard
{
public :
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnlock(); }
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }
private:
	Lock& _lock;
};