#pragma once
#include <stack>

extern thread_local uint32 LThreadId;
extern thread_local std::stack<int32> LLockStack; // 현재 걸려있는 lock의 id들 보관