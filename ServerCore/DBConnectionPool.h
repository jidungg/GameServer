#pragma once
#include "DBConnection.h"

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connectCount, const WCHAR* connectString);
	void Clear();

	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV _environment = SQL_NULL_HANDLE;
	Vector<DBConnection*> _connections;
};

