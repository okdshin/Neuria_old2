#ifdef SOCKETSERVER_UNIT_TEST
#include "SocketServer.h"
#include "../utility/Utility.h"
#include <iostream>

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	int server_port = 54321;
	if(argc == 2)
	{
		server_port = boost::lexical_cast<int>(argv[1]);
	}

	const int buffer_size = 128;
	auto session_pool = SessionPool::Create();
	auto server = SocketServer::Create(service, server_port, buffer_size, std::cout);
	
	auto on_accepted_func = Server::OnAcceptedFunc([&session_pool](
			Session::Pointer session){
		std::cout << "on_accept_func called:" << std::endl;
		session_pool->Add(session);

		auto on_received_func = Session::OnReceivedFunc([](
				Session::Pointer session, const ByteArray& byte_array){ 
			std::cout << "on receive from accepted session:" 
				<< utility::ByteArray2String(byte_array) << std::endl;
		});
		session->StartReceive(on_received_func);
	});

	auto on_failed_accept_func = Server::OnFailedAcceptFunc([](
			const ErrorCode& error_code){
		//nothing to do	
	});
	
	auto on_closed_func = Session::OnClosedFunc([&session_pool](
			Session::Pointer session){
		session_pool->Erase(session);
	});

	server->StartAccept(on_accepted_func, on_failed_accept_func, on_closed_func);

	std::cout << "accept port is " << server_port << std::endl;
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	
	t.join();

    return 0;
}

#endif
