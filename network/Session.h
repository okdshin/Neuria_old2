#pragma once
//Session:20120823
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include "NodeId.h"
#include "../ByteArray.h"
#include "../utility/Utility.h"

namespace neuria{
namespace network{

class Session
{
public:
	using Pointer = boost::shared_ptr<Session>;

	using OnReceivedFunc = boost::function<void (Pointer, const ByteArray&)>;
	using OnClosedFunc = boost::function<void (Pointer)>;
	using OnSendFinishedFunc = boost::function<void (Pointer)>;

	auto GetNodeId() -> NodeId {
		return DoGetNodeId();	
	}

	auto StartReceive(OnReceivedFunc on_received_func) -> void {
		DoStartReceive(on_received_func);	
	}
	
	auto Send(const ByteArray& byte_array, 
			OnSendFinishedFunc on_send_finished_func) -> void {
		/*
		std::cout << "send:\n\"" << utility::ByteArray2String(byte_array) 
			<< "\"" << std::endl;
		*/
		DoSend(byte_array, on_send_finished_func);		
	}
 	
	auto Close() -> void {
		DoClose();	
	}

private:
	virtual auto DoGetNodeId() -> NodeId = 0;
	virtual auto DoStartReceive(OnReceivedFunc on_received_func) -> void = 0;
	virtual auto DoSend(const ByteArray& byte_array, 
		OnSendFinishedFunc on_send_finished_func) -> void = 0;
 	virtual auto DoClose() -> void = 0;
};

inline auto Send(Session::Pointer session, const ByteArray& byte_array) -> void {
	session->Send(byte_array, [](Session::Pointer){});
};


}

}
