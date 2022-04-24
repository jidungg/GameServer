#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}

void* StompAllocator::Alloc(int32 size)
{
	//페이지 갯수 구하기
	const int pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	//오버플로우 방지용 오프셋 설정
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	//메모리 예약과 동시에 사용, 읽기쓰기 가능
	void* baseAddress = VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	//할당했던 메모리 첫 부분으로 이동
	const int64 baseAddress = address - (address % PAGE_SIZE);
	VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}
