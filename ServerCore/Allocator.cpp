#include "pch.h"
#include "Allocator.h"
#include "AllocateStrategy.h"

AllocateStrategy* Allocator::s_allocStratagy = nullptr;

void* Allocator::Allocate(int32 size)
{
	return s_allocStratagy->Allocate(size);
	//return GMemory->Allocate(size);
}

void Allocator::Release(void* ptr)
{
	s_allocStratagy->Release(ptr);
	//GMemory->Release(ptr);
}