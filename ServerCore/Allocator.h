#pragma once
class AllocateStrategy;


class Allocator
{
public:
	static void* Allocate(int32 size);
	static void Release(void* ptr);

	static void SetStrategy(AllocateStrategy* strategy) { s_allocStratagy = strategy; }
private:
	static AllocateStrategy* s_allocStratagy;
};


template<typename T>
class STLAllocator
{
public:
	using value_type = T;

	STLAllocator() { }

	template<typename Other>
	STLAllocator(const STLAllocator<Other>&) {}

	T* allocate(size_t count);

	void deallocate(T* ptr, size_t count) ;
};

template<typename T>
T* STLAllocator<T>::allocate(size_t count)
{
	const int32 size = static_cast<int32>(count * sizeof(T));
	return static_cast<T*>(Allocator::Allocate(size));
}

template<typename T>
void STLAllocator<T>::deallocate(T* ptr, size_t count)
{
	Allocator::Release(ptr);
}
