#pragma once
#include "Types.h"
#include "MemoryPool.h"
#include "AllocateStrategy.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		MemoryHeader* header;
		header = reinterpret_cast<MemoryHeader*>(s_allocStrategy->Allocate(s_allocSize));
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(header, s_allocSize));
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}
	static void Push(Type* obj)
	{
		obj->~Type();

		s_allocStrategy->Release(MemoryHeader::DetachHeader(obj));

	}
	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}
private:
	static int32 s_allocSize;
	static AllocateStrategy* s_allocStrategy;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

#ifdef _STOMP
template<typename Type>
AllocateStrategy* ObjectPool<Type>::allocStrategy = new class StompAllocate();
#else
template<typename Type>
AllocateStrategy* ObjectPool<Type>::s_allocStrategy = new class ObjectPoolAllocate(s_allocSize);
#endif // _STOMP
