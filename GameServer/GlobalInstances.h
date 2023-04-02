#pragma once

extern class GameSessionManager* GSessionManager;
extern shared_ptr<class Room> GRoom;

class GlobalInstances
{
public:
	~GlobalInstances();
	static void Instantiate();
};

