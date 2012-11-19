#pragma once
//SocketServer:20120905
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include "SocketSession.h"
#include "SessionPool.h"
#include "Server.h"
#include "../ByteArray.h"

namespace neuria{
namespace network{

class SocketServer : public Server, public boost::enable_shared_from_this<SocketServer> {
public:
	using Pointer = boost::shared_ptr<SocketServer>;

	static auto Create(boost::asio::io_service& service, 
			int port, int buffer_size, std::ostream& os) -> Pointer {
		return  Pointer(new SocketServer(service, port, buffer_size, os));	
	}

private:
    SocketServer(boost::asio::io_service& service, 
			int port, int buffer_size, std::ostream& os)
		: service(service), 
		acceptor(service, boost::asio::ip::tcp::endpoint(
			boost::asio::ip::tcp::v4(), port)), buffer_size(buffer_size), os(os){}
	
	auto DoStartAccept(
		Server::OnAcceptedFunc on_accepted_func,
		Server::OnFailedAcceptFunc on_failed_accept_func,
		Session::OnClosedFunc on_closed_func) -> void {
		auto new_session = SocketSession::Create(
			this->service, this->buffer_size, on_closed_func, this->os);

		this->acceptor.async_accept(
			new_session->GetSocketRef(),
			boost::bind(
				&SocketServer::OnAccepted, this->shared_from_this(), 
				on_accepted_func, on_failed_accept_func, on_closed_func,
				new_session, boost::asio::placeholders::error
			)
		);
	}

private:
	auto OnAccepted(
			Server::OnAcceptedFunc on_accepted_func, 
			Server::OnFailedAcceptFunc on_failed_accept_func, 
			Session::OnClosedFunc on_closed_func,
			Session::Pointer session, 
			const boost::system::error_code& error_code) -> void {
		if(!error_code){
			on_accepted_func(session);
		}
		else{
			this->os << "accept failure :" << error_code.message() << std::endl;
			on_failed_accept_func(ErrorCode(error_code));
		}
		this->StartAccept(on_accepted_func, on_failed_accept_func, on_closed_func);
	}

	boost::asio::io_service& service;
	boost::asio::ip::tcp::acceptor acceptor;
	int buffer_size;
	std::ostream& os;

};



}
}
