#pragma once

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