#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
public :
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:

	void DFS(int32 index);

	unordered_map<const char*, int32> _nameToId; //lock의 ID를 보관
	unordered_map<int32, const char*> _idToName; //lock의 ID를 보관
	stack<int32> _lockStack; // 현재 걸려있는 lock의 id들 보관
	map<int32, set<int32>> _lockHistory; // 간선 정보

	Mutex _lock;

	//DFS 관련
	vector<int32> _discoveredOrder; //노드가 발견된 순서를 기록
	int32 _discoveredCount = 0; //노드가 발견된 순서 추적
	vector<bool> _finished; //DFS가 종료됐는지 확인
	vector<int32> _parent; //부모님이 누구인지



};

