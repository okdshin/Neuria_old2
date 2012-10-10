#ifdef UTILITY_UNIT_TEST
#include "Utility.h"
#include <iostream>

using namespace neuria;
using namespace neuria::utility;

void TestRandom()
{
	for(unsigned int i = 0; i < 100; i++)
	{
		std::cout << Random(10, 100) << std::endl;	
	}
}

int main(int argc, char* argv[])
{
	//TestCalcSimilarity();
	TestRandom();
    return 0;
}

#endif
