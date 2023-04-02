#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"



enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerNetServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		//타이머 Job 분배
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	CoreGlobal::Instantiate();
	GlobalInstances::Instantiate();

	GConsoleLogger->WriteStdOut(Color::RED,L"start\n");

	//TODO : connectionString 하드코딩된거 항목별로 나눠서 파일로 보관하고 읽어서 호출.
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
	
	GConsoleLogger->WriteStdOut(Color::RED, L"Connect\n");
	
	DBConnection* dbConn = GDBConnectionPool->Pop();

	GConsoleLogger->WriteStdOut(Color::RED, L"Pop\n");

	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml"); 

	GConsoleLogger->WriteStdOut(Color::RED, L"Synchronize\n");

	{
		WCHAR name[] = L"김지완";

		SP::InsertGold insertGold(*dbConn);
		insertGold.In_Gold(100);
		insertGold.In_Name(name);
		insertGold.In_CreateDate(TIMESTAMP_STRUCT{ 2020, 6, 8 });
		insertGold.Execute();
	}

	{
		SP::GetGold getGold(*dbConn);
		getGold.In_Gold(100);

		int32 id = 0;
		int32 gold = 0;
		WCHAR name[100];
		TIMESTAMP_STRUCT date;

		getGold.Out_Id(OUT id);
		getGold.Out_Gold(OUT gold);
		getGold.Out_Name(OUT name);
		getGold.Out_CreateDate(OUT date);

		getGold.Execute();

		while (getGold.Fetch())
		{
			GConsoleLogger->WriteStdOut(Color::BLUE,
				L"ID[%d] Gold[%d] Name[%s]\n", id, gold, name);
		}
	}
	GConsoleLogger->WriteStdOut(Color::RED, L"DBEnd\n");

	ClientPacketHandler::Init();

	
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(7777);

	ServerNetServiceRef service = MakeShared <ServerNetService>(
		NetAddress(addr),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		1);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}
	
	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}