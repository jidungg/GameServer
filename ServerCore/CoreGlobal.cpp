#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DBConnectionPool.h"
#include "ConsoleLog.h"
#include "AllocateStrategy.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;

DeadLockProfiler* GDeadLockProfiler = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
ConsoleLog* GConsoleLogger = nullptr;

void CoreGlobal::Instantiate()
{
#ifdef _STOMP
	Allocator::SetStrategy(static_cast<AllocateStrategy*>(new StompAllocate()));
#else
	Allocator::SetStrategy(static_cast<AllocateStrategy*>(new PoolAllocate()));
#endif

	GThreadManager = new ThreadManager();
	GSendBufferManager = new SendBufferManager();
	GGlobalQueue = new GlobalQueue();
	GJobTimer = new JobTimer();

	GDeadLockProfiler = new DeadLockProfiler();
	GDBConnectionPool = new DBConnectionPool();
	GConsoleLogger = new ConsoleLog();
	SocketUtils::Init();

}
CoreGlobal::CoreGlobal()
{
}
CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GSendBufferManager;
	delete GGlobalQueue;
	delete GJobTimer;
	delete GDeadLockProfiler;
	delete GDBConnectionPool;
	delete GConsoleLogger;
	SocketUtils::Clear();

}
