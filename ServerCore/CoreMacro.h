#pragma once

//������ ������ �ƴ� ���������� CRASH�� ���� ���� ��Ȳ�� ���.
#define CRASH(cause)			\
{								\
	uint32* crash = nullptr;	\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;		\
}								

//���ǿ� ���� CRASH�� ���� ���� �� ���.
#define ASSERT_CRASH(expr)		\\
{								\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}							\
}								