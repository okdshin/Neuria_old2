#pragma once
//Server:20120906
#include <iostream>
#include "Session.h"
#include "SessionPool.h"

namespace neuria{
namespace network{

class Server {
public:
	using Pointer = boost::shared_ptr<Server>;
	using OnAcceptedFunc = boost::function<void (Session::Pointer)>;

	auto SetOnReceivedFunc(Session::OnReceivedFunc on_receive_func) -> void {
		this->DoSetOnReceivedFunc(on_receive_func);	
	}
	
	auto SetOnAcceptedFunc(OnAcceptedFunc on_accept_func) -> void {
		this->DoSetOnAcceptedFunc(on_accept_func);	
	}
	
	auto SetOnClosedFunc(Session::OnClosedFunc on_close_func) -> void {
		this->DoSetOnClosedFunc(on_close_func);	
	}
	
	auto StartAccept() -> void {
		this->DoStartAccept();	
	}

private:
	virtual auto DoSetOnReceivedFunc(Session::OnReceivedFunc on_receive_func) -> void = 0;
	virtual auto DoSetOnAcceptedFunc(OnAcceptedFunc on_accept_func) -> void = 0;
	virtual auto DoSetOnClosedFunc(Session::OnClosedFunc on_close_func) -> void = 0;

	virtual auto DoStartAccept() -> void = 0;
};

inline auto SetCallbacks(Server::Pointer target, 
		Server::OnAcceptedFunc on_accept, 
		Session::OnReceivedFunc on_receive, 
		Session::OnClosedFunc on_close) -> void {
	target->SetOnAcceptedFunc(on_accept);
	target->SetOnReceivedFunc(on_receive);
	target->SetOnClosedFunc(on_close);
}

inline auto SetCallbacks(Server::Pointer target, SessionPool::Pointer pool,
		Session::OnReceivedFunc on_receive) -> void {
	SetCallbacks(target, [pool](Session::Pointer session){ pool->Add(session); }, 
		on_receive, [pool](Session::Pointer session){ pool->Erase(session); });
}

inline auto SetOnReceivedFuncOnly(Server::Pointer target,
		Session::OnReceivedFunc on_receive) -> void {
	SetCallbacks(target, [](Session::Pointer){}, 
		on_receive, [](Session::Pointer){});		
}

}
}

