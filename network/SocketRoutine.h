#pragma once
//SocketRoutine:20121011
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace neuria{
namespace network{

inline auto CreateSocketNodeId(const std::string& hostname, int port) -> NodeId {
	return hostname + "/" + boost::lexical_cast<std::string>(port);	
}


}
}
