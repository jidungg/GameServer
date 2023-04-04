#pragma once


class AllocateStrategy
{
public:
	virtual void* Allocate(int32 size);
	virtual void Release(void* ptr);
};
class BaseAllocate :public AllocateStrategy
{
public:
	virtual void* Allocate(int32 size);
	virtual void Release(void* ptr);
};

class StompAllocate :public AllocateStrategy
{
	enum { PAGE_SIZE = 0x1000 };
public:
	virtual void* Allocate(int32 size);
	virtual void Release(void* ptr);
};


class MemoryPool;
class PoolAllocate : public AllocateStrategy
{
	enum
	{
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096,
	};
public:
	PoolAllocate();
	~PoolAllocate();

	virtual void* Allocate(int32 size);
	virtual void Release(void* ptr);
private:
	vector<MemoryPool*> _pools;
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

class ObjectPoolAllocate : public AllocateStrategy
{
public:
	ObjectPoolAllocate(int32 size) { pool = new MemoryPool(size); allocSize = size; }
	virtual void* Allocate(int32 size);
	virtual void Release(void* ptr);
private:
	int32 allocSize;
	MemoryPool* pool;
};
