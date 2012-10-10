#ifdef LABELEDSINK_UNIT_TEST
#include "LabeledSink.h"
#include <iostream>

using namespace neuria;
using namespace neuria::utility;

int main(int argc, char* argv[])
{
	LabeledSink::Stream los(LabeledSink("label"));
	los << "hello" << std::endl;
    return 0;
}

#endif
