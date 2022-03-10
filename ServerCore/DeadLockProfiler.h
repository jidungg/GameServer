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

	unordered_map<const char*, int32> _nameToId; //lock�� ID�� ����
	unordered_map<int32, const char*> _idToName; //lock�� ID�� ����
	stack<int32> _lockStack; // ���� �ɷ��ִ� lock�� id�� ����
	map<int32, set<int32>> _lockHistory; // ���� ����

	Mutex _lock;

	//DFS ����
	vector<int32> _discoveredOrder; //��尡 �߰ߵ� ������ ���
	int32 _discoveredCount = 0; //��尡 �߰ߵ� ���� ����
	vector<bool> _finished; //DFS�� ����ƴ��� Ȯ��
	vector<int32> _parent; //�θ���� ��������



};

