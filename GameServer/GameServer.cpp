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
	Knight() {  }
	Knight(int hp) :_hp(hp){}
	int _hp;
};
int main()
{
	Knight* knight = xnew<Knight>();
	Knight* knight2 = xnew<Knight>(11);

	xdelete(knight);
	xdelete(knight2);

}

