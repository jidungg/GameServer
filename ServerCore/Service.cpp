#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

NetService::NetService(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	:_type(type), _netAddrss(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

NetService::~NetService()
{
}

void NetService::CloseService()
{
}
void NetService::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (const auto& session: _sessions)
	{
		session->Send(sendBuffer);
	}
}
SessionRef NetService::CreateSession()
{
	SessionRef session = _sessionFactory();
	session->SetService(shared_from_this());
	if (_iocpCore->Register(session) == false)
	{
		return nullptr;
	}
	return session;
}

void NetService::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

void NetService::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

ClientNetService::ClientNetService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	:NetService(ServiceType::Client, targetAddress,core,factory,maxSessionCount)
{
}

bool ClientNetService::Start()
{
	if (CanStart() == false)
		return false;

	const int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
			return false;
	}
	return true;
}

ServerNetService::ServerNetService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	:NetService(ServiceType::Server, address, core, factory, maxSessionCount)
{
}

bool ServerNetService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerNetServiceRef service = static_pointer_cast<ServerNetService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerNetService::CloseService()
{
	NetService::CloseService();
}
