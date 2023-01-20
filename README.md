# Description
This procject is a server core program for MMORPG game. You can use this to make a server & client for something. It doesn't have to be a MMORPG game. 
I made this repo by referring to the Inflearn lecture.
[Inflearn lecture]https://www.inflearn.com/course/%EC%96%B8%EB%A6%AC%EC%96%BC-3d-mmorpg-4/dashboard
Please refer to my blog for more information.
[blog]https://ddukddaksudal.tistory.com/113

# Environment
* Operating System : Windows11
* IDE : Visual Studio 2022
* Language: C++ 17 & Python 3
* DB : ODBC, SQL Server(can modify)

# How to Build
Intruduce how to build and run Example Projects(GameServer, DummyClient)
1. Open the solution file with VisualStudio
2. Build ServerCore project
3. Run ./Tools/ProcedureGenerator/MakeExe.bat
4. Move the result (GenProcs.exe) to ./Common/Procedures
5. Run ./Tools/PacketGenerator/MakeExe.bat
6. Move the result (GenPackets.exe) to ./Common/Protobuf/bin
7. Build GameServer & DummyClient project(maybe twice)
8. Run GameServer & DummyClient project

# How to Use
Introduce how to use ServerCore. There are several functions in ServerCore.
1. DataBase
2. Networking
3. MultiThreading
4. Job Queueing
Make sure to include **CorePch.h** before you use its functions.

## Using DB
1. include **DBConnectionPool.h , DBBind.h, DBSynchronizer.h**.
3. Make XML files describe DataBase. For example
	```xml
	<GameDB>
		<Table name="Gold" desc="골드 테이블">
			<Column name="id" type="int" notnull="true" identity="1,1" />
			<Column name="gold" type="int" notnull="false" />
			<Column name="name" type="nvarchar(50)" notnull="false" />
			<Column name="createDate" type="datetime" notnull="false" />
			<Index type="clustered">
				<PrimaryKey/>
				<Column name="id" />
			</Index>
		</Table>

		<Procedure name="spGetGold">
			<Param name="@gold" type="int"/>
			<Body>
				<![CDATA[
				SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (@gold)
				]]>
			</Body>
		</Procedure>
	</GameDB>
	```
4. Call **GDBConnectionPool->Connect**(int32 connectCount, const WCHAR* connectString) to Link DBMS to ODBC and make **DBConnection**'s in GDBConnectionPool. You can adjust **connectionCount** and **connectString**. ConnectionCount is the number of connections that will included into the pool. ConnectString is the ODBC connection string of DBMS. 
	 ```c++
	 ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
	 ```
5. **Pop** a DBConnection from the GDBConnectionPool.
	```c++
	DBConnection* dbConn = GDBConnectionPool->Pop();
	```
6. Create a **DBSynchronizer** with the DBConnection you poped  in step 3. 
	```c++
	DBSynchronizer dbSync(*dbConn);
	```
7. Call DBSynchronizer.Synchronize(const WCHAR* path) with the XMLfile you made in step 1. This task will match the DataBase to XML.
	```c++
	dbSync.Synchronize(L"GameDB.xml"); 
	```
8.  Use **DBBind** to query to the DataBase. You can also use Procedure described in XML file. 
	```c++
	DBBind<3,0> dbBind(dbConn,  L"{CALL dbo.spGetGold(?)}");
	int32 id = 0;
	int32 gold = 100;
	WCHAR name[100];
	TIMESTAMP_STRUCT date;
	dbBind.BindParam(0,gold);
	dbBind.BindCol(0,id);
	dbBind.BindCol(1,gold);
	dbBind.BindCol(2,name);
	dbBind.BindCol(2,date);

	dbBind.Execute();
	dbBind.Fetch();
	GConsoleLogger->WriteStdOut(Color::BLUE,
				L"ID[%d] Gold[%d] Name[%s]\n", id, gold, name);
	```
	
## Use Networking
1. include **Service.h**.
2. Create **ClientService** object on client program, **ServerService** object on server program. (Both classes are derived form **Service** class.)
```c++
//==========Server=============================
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(7777);

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(addr),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		1);
//==========Client================================
	wstring  ip;
	cout << "Input Server IP: ";
	wcin >> ip;

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(ip,7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);
```
2. Start service.
```c++
	ASSERT_CRASH(service->Start());
```
3. Call **Dispatch** to receive & process IOCP jobs.
```c++
	while (true)
	{
		service->GetIocpCore()->Dispatch();
	}
```

4. Send packets 
```c++
	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(u8"HelloWorld!");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	service->Broadcast(sendBuffer);
```

## Use Multi Threading
1. Include **ThreadManager.h**.
2. Call **GThreadManager->Launch()** to make and run a new thread.
```c++
GThreadManager->Launch([&service]()
	{
		DoWorkerJob(service);
	});
```
3. Call **GThreadManager->Join()** to join every created threads.
```c++
GThreadManager->Join();
```
## Use JobQueue
1. Include **JobQueue.h**. Its also in CorePch.h.
2. Create a **JobQueue object**. Or creates an object for the class that inherits the jobQueue.
```c++
class RoomManager : public JobQueue
{
private:
	void TryEnterRoom(uint64 roomNum, UserProfileRef user);
	...
};
```
3. Call **DoAsync()** to push job to the job queue. Or you can also use **DoTimer()** to push a job after a given time to the job queue.
```c++
	DoAsync(&RoomManager::TryEnterRoom, roomNum, user);
```
4. Call **ThreadManager::DistributeReservedJobs()** and **ThreadManager::DoGlobalQueueWork()** in any thread that you want to process the jobs that in the job queue.
```c++
void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;
		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();
		ThreadManager::DoGlobalQueueWork();
	}
}
```

# Files

 - Binary : Result files.(DummyClient, GameServer)
 - Common : Files about tools execute before compile.
 - DummyClient : Example client program using the ServerCore.
 - GameServer : Example server program using the ServerCore.
 - Libraries : About used libraries.
 - ServerCore : The main project. Network communication  using IOCP.
 - Tools : Tools for automation.
	 - PacketGenerator : Automation Tool generating packet. Using Protobuf (python)
	  - ProcedureGenerator : Automation Tool generating DataBase procedure. (python)

# Used Libraries

 - rapidXML(c++)
 - jinja(python) 
 - protobuf(c++)

