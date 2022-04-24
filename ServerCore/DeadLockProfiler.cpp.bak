#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockId = 0;

	//아이디를 찾거나 새로 만듦
	auto foundIt = _nameToId.find(name);
	if (foundIt == _nameToId.end())
	{
		lockId = static_cast<int>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else 
	{
		lockId = foundIt->second;
	}

	//아이디를 lockStack에 넣음

	//하려는 lock이 사이클을 만드는지 확인

	if(_lockStack.empty() == false)
	{ 
		const int32 prevId = _lockStack.top();
		//재귀 락이 아닌 경우에만
		if (lockId != prevId) 
		{
			set<int32>& history = _lockHistory[prevId];
			//이미 추가된 노드가 아닌 경우에만(이미 걸린 락이 아닌 경우에만) 사이클 체크
			if (history.find(lockId) == history.end())   
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);
	if (_lockStack.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}


	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId)
	{
		CRASH("INVALID_UNLOCK");
	}


	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);
	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		DFS(lockId);
	}
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::DFS(int32 here)
{
	//이미 발견한 노드이면 스킵.
	if (_discoveredOrder[here] != -1)
		return;

	//순서 기록
	_discoveredOrder[here] = _discoveredCount++;

	//정점 순회 시작
	//연결된 노드가 없으면 finish
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;

	}
	//연결된 노드가 있으면 
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		//연결된 노드가 이미 발견한 노드가 아니면
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			DFS(there);
			continue;
		}

		//연결된 노드가 이미 발견한 노드면 
		//연결된 노드가 더 나중에 발견된 노드면 (순방향이면) 넘어감
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		//연결된 노드가 더 먼저 발견된 노드면 (역방향이면) 사이클 발생
		if (_finished[there] == false)
		{
			printf("%s->%s\n", _idToName[here], _idToName[there]);
			int32 now = here;
			while (true)
			{
				printf("%s->%s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}
			CRASH("DEADLOCK_DETECTED");
		}

	}
	_finished[here] = true;

}
