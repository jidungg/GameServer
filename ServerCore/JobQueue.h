#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::*memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
	}

	void DoTimer(uint64 tickCount,CallbackType&& callback)
	{
		JobRef job = ObjectPool<Job>::MakeShared(std::move(callback));
		GJobTimer->Reserve(tickCount, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickCount,Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickCount, owner, job);
	}

	void ClearJobs() { _jobs.Clear(); }

public:
	void Push(JobRef job, bool pushOnly = false);
	void Execute();
protected:
	LockQueue<JobRef> _jobs;
	Atomic<int32> _jobCount = 0;
};

