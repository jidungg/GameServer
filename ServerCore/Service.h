#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = function<SessionRef(void)>;
class NetService : public enable_shared_from_this<NetService>
{
public : 
	NetService(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~NetService();

	virtual bool Start() abstract;
	bool CanStart() { return _sessionFactory != nullptr; }
	
	virtual void CloseService();
	void SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	void Broadcast(SendBufferRef sendBuffer);
	SessionRef CreateSession();
	void AddSession(SessionRef session);
	void ReleaseSession(SessionRef session);
	int32 GetCurrentSessionCount() { return _sessionCount; }
	int32 GetMaxSessionCount() { return _maxSessionCount; }

public :
	ServiceType GetServiceType() { return _type; }
	NetAddress GetNetAddresss() { return _netAddrss; }
	IocpCoreRef& GetIocpCore() { return _iocpCore; }


protected:
	USE_LOCK;
	ServiceType _type;
	NetAddress _netAddrss = { };
	IocpCoreRef _iocpCore;

	Set<SessionRef> _sessions;
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;
	SessionFactory _sessionFactory;

};

class ClientNetService : public NetService
{
public:
	ClientNetService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientNetService() {}

	virtual bool Start() override;
};

class ServerNetService : public NetService
{
public:
	ServerNetService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerNetService(){}

	virtual bool Start() override;
	virtual void CloseService()override;
private:
	ListenerRef _listener = nullptr;
};