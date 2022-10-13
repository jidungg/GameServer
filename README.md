
# Description
There are 5 projectes in the solution.
 - ServerCore : The main project. Network communication  using IOCP.
 - GameServer : Example server program using the ServerCore.
 - DummyClient : Example client program using the ServerCore.
 - PacketGenerator : Automation Tool generating packet. Using Protobuf (python)
 - ProcedureGenerator : Automation Tool generating DataBase procedure. (python)

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
Introduce how to use ServerCore.
### Using DB
1. Make XML files describe DataBase. For example
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
2. Use **GDBConnectionPool->Connect**(int32 connectCount, const WCHAR* connectString) to Link DBMS to ODBC and make **DBConnection**'s in GDBConnectionPool. You can adjust **connectionCount** and **connectString**. ConnectionCount is the number of connections that will included into the pool. ConnectString is the ODBC connection string of DBMS. 
	 ```c++
	 ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
	 ```
3. **Pop** a DBConnection from the GDBConnectionPool.
	```c++
	DBConnection* dbConn = GDBConnectionPool->Pop();
	```
4. Create a **DBSynchronizer** with the DBConnection you poped  in step 3. 
	```c++
	DBSynchronizer dbSync(*dbConn);
	```
5. Call DBSynchronizer.Synchronize(const WCHAR* path) with the XMLfile you made in step 1. This task will match the DataBase to XML.
	```c++
	dbSync.Synchronize(L"GameDB.xml"); 
	```
6.  Use **DBBind** to query to the DataBase. You can also use Procedure described in XML file. 
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
	```
	
### Use Networking
# Files
### ServerCore
ServerCore using IOCP. 
### GameServer
Example server using ServerCore
  - Protobuf : generating protocol.
  - 
### DummyClient
Example client using ServerCore
### Tools
Tools for automation.
  - PacketGenerator
  - ProcedureGenerator

# Used Libraries

 - rapidXML(c++)
 - jinja(python) 
 - protobuf(c++)

### Reference
