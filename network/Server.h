#pragma once
//Server:20120906
#include <iostream>
#include "Session.h"
#include "ErrorCode.h"

namespace neuria{
namespace network{

class Server {
public:
	using Pointer = boost::shared_ptr<Server>;
	using OnAcceptedFunc = boost::function<void (Session::Pointer)>;
	using OnFailedAcceptFunc = boost::function<void (const ErrorCode&)>;

	auto StartAccept(
			Server::OnAcceptedFunc on_accepted_func,
			Server::OnFailedAcceptFunc on_failed_accept_func,
			Session::OnClosedFunc on_closed_func) -> void {
		this->DoStartAccept(on_accepted_func, on_failed_accept_func, on_closed_func);		
	}

private:
	virtual auto DoStartAccept(
		Server::OnAcceptedFunc on_accepted_func,
		Server::OnFailedAcceptFunc on_failed_accept_func,
		Session::OnClosedFunc on_closed_func) -> void = 0;
};


}
}

