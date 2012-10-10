#ifdef ROUTINE_UNIT_TEST
#include "Routine.h"
#include <iostream>
#include "../utility/Utility.h"

using namespace neuria;
using namespace neuria::hash;

int main(int argc, char* argv[])
{
	std::cout << CalcRoutineStr(utl::String2ByteArray("string")) << std::endl;
	std::cout << CalcRoutineStr(utl::String2ByteArray("strinh")) << std::endl;
	std::cout << CalcRoutineStr(utl::String2ByteArray("")) << std::endl;
    return 0;
}

#endif
