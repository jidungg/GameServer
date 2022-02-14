#include "pch.h"
#include "Lock.h"

//�ٸ� �������� writelcok�� ����, ���� �������� writelock�� ���
void Lock::WriteLock()
{
	//���� �������� ��� wirtecount ����
	const uint32 threadId = _lockFlag.load() & WRITE_THREAD_MASK >> 16;
	if (LThreadId == threadId)
	{
		++_writeCount;
		return;
	}

	//�ƹ��� �����ϰ� ���� �ʰų� �ٸ� �����尡 �������� ��� ����
	const int64 beginTick = GetTickCount64();
	const uint32 desired = (LThreadId << 16) & WRITE_THREAD_MASK;
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			//lockFlag�� EMPTY_FLAG�� ������ ��� -> write���� �����嵵 ���� read���� �����嵵 ���� ����.
			//�ƹ��� ������ �ڽ��� ID�� ����.
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
	//readLock�� �� Ǯ���� writeunlock ����.
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
//���� �� �ڽ� ����� �ƹ��� wrieteLock ������ �ʾƾ� ��. 
void Lock::ReadLock()
{
	//���� �������� ��� readcount ����
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
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)//fetch_sub�� ��� ���� ���� ��ȯ�ϱ� ������ 0�̸� �̻���
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}
