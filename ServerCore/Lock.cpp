#include "pch.h"
#include "Lock.h"

//다른 스레드의 writelcok은 막고, 같은 스레드의 writelock은 허용
void Lock::WriteLock()
{
	//같은 스레드일 경우 wirtecount 증가
	const uint32 threadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == threadId)
	{
		++_writeCount;
		return;
	}

	//아무도 소유하고 있지 않거나 다른 스레드가 소유중일 경우 경합
	const int64 beginTick = GetTickCount64();
	const uint32 desired = (LThreadId << 16) & WRITE_THREAD_MASK;
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			//lockFlag가 EMPTY_FLAG일 때까지 대기 -> write중인 스레드도 없고 read중인 스레드도 없는 상태.
			//아무도 없으면 자신의 ID를 넣음.
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				++_writeCount;
				return;
			}
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	//readLock이 다 풀려야 writeunlock 가능.
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
	{
		CRASH("INVALID_UNLOCK_ORDER");
	}
	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG);
	}
}
//읽을 떄 자신 말고는 아무도 wrieteLock 중이지 않아야 함. 
void Lock::ReadLock()
{
	//같은 스레드일 경우 readcount 증가
	const uint32 threadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == threadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	const int64 beginTick = GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = _lockFlag & READ_COUNT_MASK;
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}
		}
		if (GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)//fetch_sub는 계산 이전 값을 반환하기 때문에 0이면 이상함
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}
