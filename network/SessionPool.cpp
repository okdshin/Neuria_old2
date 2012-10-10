#ifdef SESSIONPOOL_UNIT_TEST
#include "SessionPool.h"
#include <iostream>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	auto pool = SessionPool::Create();
	
    return 0;
}

#endif
