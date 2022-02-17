#pragma once

#define OUT

#define USE_MANY_LOCKS(count) Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)
//컴파일 에러가 아닌 인위적으로 CRASH를 내고 싶은 상황에 사용.
#define CRASH(cause)			\
{								\
	uint32* crash = nullptr;	\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;		\
}								

//조건에 따라 CRASH를 내고 싶을 때 사용.
#define ASSERT_CRASH(expr)		\\
{								\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}							\
}								