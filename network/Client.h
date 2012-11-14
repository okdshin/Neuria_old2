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

//create session and add it to pool.
inline auto Connect(Client::Pointer client, 
		const NodeId& node_id, SessionPool::Pointer pool, 
		Client::OnFailedConnectFunc on_failed_connect_func, 
		Session::OnReceivedFunc on_received_func) -> void {
	client->Connect(node_id, 
		Client::OnConnectedFunc([pool](Session::Pointer session){ 
			pool->Add(session); 
		}), 
		on_failed_connect_func,
		on_received_func, 
		Session::OnClosedFunc([pool](Session::Pointer session){ 
			pool->Erase(session); 
		})
	);	
}

//
inline auto Communicate(Client::Pointer client, const NodeId& node_id, 
		Client::OnFailedConnectFunc on_failed_connect_func, 
		const ByteArray& byte_array, Session::OnReceivedFunc on_received_func,
		Session::OnSendFinishedFunc on_send_finished_func) -> void {
	client->Connect(node_id, 
		Client::OnConnectedFunc([byte_array, on_send_finished_func](
				Session::Pointer session){ 
			session->StartReceive();
			session->Send(byte_array, on_send_finished_func); 
		}), 
		on_failed_connect_func,
		on_received_func,
		Session::OnClosedFunc([](Session::Pointer){
		})
	);	
}

//send once
inline auto Send(Client::Pointer client, 
		const NodeId& node_id, const ByteArray& byte_array,
		Client::OnFailedConnectFunc on_failed_connect_func) -> void {
	client->Connect(node_id,
		Client::OnConnectedFunc([byte_array](Session::Pointer session){
			session->Send(byte_array, 
				Session::OnSendFinishedFunc([](Session::Pointer session){ 
					session->Close(); 
				})
			);
		}), 
		on_failed_connect_func,
		Session::OnReceivedFunc([](Session::Pointer, const ByteArray&){
		}), 
		Session::OnClosedFunc([](Session::Pointer){
		})
	);				
}

}
}

