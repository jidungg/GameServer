#include "pch.h"
#include "AllocateStrategy.h"
#include "Memory.h"

void* AllocateStrategy::Allocate(int32 size)
{
	return nullptr;
}

void AllocateStrategy::Release(void* ptr)
{
}

void* BaseAllocate::Allocate(int32 size)
{
	return ::malloc(size);
}

void BaseAllocate::Release(void* ptr)
{
	::free(ptr);
}

void* StompAllocate::Allocate(int32 size)
{
	//페이지 갯수 구하기
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	//오버플로우 방지용 오프셋 설정
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	//메모리 예약과 동시에 사용, 읽기쓰기 가능
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);

}

void StompAllocate::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	//할당했던 메모리 첫 부분으로 이동
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}


PoolAllocate::PoolAllocate()
{
	int32 size = 0;
	int32 tableIndex = 0;
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

PoolAllocate::~PoolAllocate()
{
	for (MemoryPool* pool : _pools)
	{
		delete pool;

	}
	_pools.clear();
}

void* PoolAllocate::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		header = reinterpret_cast<MemoryHeader*>(_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		header = _poolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

void PoolAllocate::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		_aligned_free(header);
	}
	else
	{
		_poolTable[allocSize]->Push(header);
	}

}

void* ObjectPoolAllocate::Allocate(int32 size)
{
	return pool->Pop();
}

void ObjectPoolAllocate::Release(void* ptr)
{
	pool->Push(reinterpret_cast<MemoryHeader*>(ptr));
}
