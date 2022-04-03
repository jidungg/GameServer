#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "Memory.h"

class Knight
{
public:
	Knight() { _hp = 100; }
	Knight(int hp) :_hp(hp){}
	~Knight() {}
	int _hp;
};
int main()
{
	Knight* test = new Knight();
	test->_hp = 100;
	delete test;
	test->_hp = 200;

}

