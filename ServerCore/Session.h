#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class NetService;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class NetService;

	enum 
	{
		BUFFER_SIZE = 0x10000,//64KB
	};
public:
	Session();
	virtual ~Session();
public:
	void				Send(SendBufferRef sendBuffer);
	bool				Connect();
	void				Disconnect(const WCHAR* cause);

	shared_ptr<NetService> GetService() { return _service.lock(); }
	void				SetService(shared_ptr<NetService> service) { _service = service; }
public:
	/* 정보 관련 */
	void		SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress	GetAddress() { return _netAddress; }
	SOCKET		GetSocket() { return _socket; }
	bool		IsConnected(){ return _connected; }
	SessionRef  GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	/* 인터페이스 구현 */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 전송 관련 */
	bool		RegisterConnect();
	bool		RegisterDisonnect();
	void		RegisterRecv();
	void		RegisterSend();
	
	void		ProcessConnect();
	void		ProcessRecv(int32 numofBytes);
	void		ProcessSend(int32 numofBytes);
	void		ProcessDisconnect();
	void		HandleError(int32 errorCode);

protected:
	/*콘텐츠에서 오버로딩*/
	virtual void	OnConnected(){}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected() {}

private:
	weak_ptr<NetService> _service;
	SOCKET			_socket = INVALID_SOCKET;
	NetAddress		_netAddress = {};
	Atomic<bool>	_connected = false;

private:
	USE_LOCK;
	RecvBuffer				_recvBuffer;

	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;
private:
	ConnectEvent	_connectEvent;
	DisconnectEvent	_disconnectEvent;
	RecvEvent		_recvEvent;
	SendEvent		_sendEvent;
};

struct PacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};