#include "pch.h"
#include "GlobalInstances.h"
#include "GameSessionManager.h"
#include "Room.h"

GameSessionManager* GSessionManager = nullptr;
shared_ptr<Room> GRoom;

void GlobalInstances::Instantiate()
{
	GSessionManager = new GameSessionManager();
	GRoom = MakeShared<Room>();
}

GlobalInstances::~GlobalInstances()
{
	delete GSessionManager;
}

