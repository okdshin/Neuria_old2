#pragma once
//Client:20120908
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "NodeId.h"
#include "Session.h"
#include "SessionPool.h"
#include "ErrorCode.h"

namespace neuria{
namespace network{

class Client{
public:
	using Pointer = boost::shared_ptr<Client>;
	using OnConnectedFunc = boost::function<void (Session::Pointer)>;
	using OnFailedConnectFunc = boost::function<void (const ErrorCode&)>;

	virtual auto Connect(const NodeId& node_id, 
			OnConnectedFunc on_connect_func, 
			OnFailedConnectFunc on_failed_connect_func,
			Session::OnReceivedFunc on_receive_func, 
			Session::OnClosedFunc on_close_func) -> void {
		this->DoConnect(node_id, on_connect_func, on_failed_connect_func, 
			on_receive_func, on_close_func);	
	}

    virtual ~Client(){}
	
private:
	
	virtual auto DoConnect(const NodeId& node_id, 
		OnConnectedFunc on_connect_func, OnFailedConnectFunc on_failed_connect_func,
		Session::OnReceivedFunc on_receive_func, Session::OnClosedFunc on_close_func) -> void = 0;
};

inline auto Connect(Client::Pointer client, 
		const NodeId& node_id, SessionPool::Pointer pool, 
		Client::OnFailedConnectFunc on_failed_connect_func, 
		Session::OnReceivedFunc on_receive_func) -> void {
	client->Connect(node_id, 
		[pool](Session::Pointer session){ pool->Add(session); }, 
		on_failed_connect_func,
		on_receive_func, 
		[pool](Session::Pointer session){ pool->Erase(session); });	
}

inline auto Communicate(Client::Pointer client, const NodeId& node_id, 
		Client::OnFailedConnectFunc on_failed_connect_func, 
		const ByteArray& byte_array, Session::OnReceivedFunc on_receive_func,
		Session::OnSendFinishedFunc on_send_finished_func) -> void {
	client->Connect(node_id, 
		[byte_array, on_send_finished_func](Session::Pointer session){ 
			session->Send(byte_array, on_send_finished_func); }, 
		on_failed_connect_func,
		on_receive_func,
		[](Session::Pointer){});	
}

inline auto Send(Client::Pointer client, 
		const NodeId& node_id, const ByteArray& byte_array,
		Client::OnFailedConnectFunc on_failed_connect_func) -> void {
	client->Connect(node_id,
		[byte_array](Session::Pointer session){
			session->Send(byte_array, 
				[](Session::Pointer session){ session->Close(); }
			);
		}, 
		on_failed_connect_func,
		[](Session::Pointer, const ByteArray&){}, 
		[](Session::Pointer){});				
}

}
}

