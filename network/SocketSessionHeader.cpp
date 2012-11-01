#ifdef SOCKETSESSIONHEADER_UNIT_TEST
#include "SocketSessionHeader.h"
#include <iostream>
#include "../utility/Utility.h"

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	/*
	std::cout << pow(2,4) << std::endl;
	std::cout << pow(2,8) << std::endl;
	std::cout << pow(2,16) << std::endl;
	std::cout << pow(2,32) << std::endl;
	std::cout << pow(2,64) << std::endl;
	std::cout << pow(2,128) << std::endl;
	*/
	{
		SocketSessionHeader header8(pow(2,8)-42);
		std::cout << header8 << std::endl;
		std::cout << header8.Serialize() << std::endl;
		std::cout << SocketSessionHeader::Parse(header8.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		SocketSessionHeader header16(pow(2,16)-42);
		std::cout << header16 << std::endl;
		std::cout << header16.Serialize() << std::endl;
		std::cout << SocketSessionHeader::Parse(header16.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		SocketSessionHeader header32(pow(2,32)-42);
		std::cout << header32 << std::endl;
		std::cout << header32.Serialize() << std::endl;
		std::cout << SocketSessionHeader::Parse(header32.Serialize()) << std::endl;
		std::cout << "\n";
	}
	{
		SocketSessionHeader header64(pow(2, 62));
		std::cout << header64 << std::endl;
		std::cout << header64.Serialize() << std::endl;
		std::cout << SocketSessionHeader::Parse(header64.Serialize()) << std::endl;
	}
    return 0;
}

#endif
