#ifdef SOCKETCLIENT_UNIT_TEST
#include "SocketClient.h"
#include <iostream>
#include "../test/Test.h"

using namespace neuria;
using namespace neuria::network;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	const int buffer_size = 128;

	std::stringstream no_output;
	std::ostream& os = std::cout;

	auto client = neuria::network::SocketClient::Create(
		service, buffer_size, os);
	
	auto session_pool = neuria::network::SessionPool::Create();

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	
	shell.Register("link", "create new link.", 
		[client, session_pool](const neuria::test::CuiShell::ArgList& args){
			try{
				client->Connect(
					neuria::network::CreateSocketNodeId(
						args.at(1), 
						boost::lexical_cast<int>(args.at(2))
					),
					Client::OnConnectedFunc([session_pool](Session::Pointer session){
						session_pool->Add(session);	
					}),
					Client::OnFailedConnectFunc([](const ErrorCode& error_code){
						std::cout << "failed create link. : " << error_code << std::endl;
					}),
					Session::OnReceivedFunc([](Session::Pointer session, 
							const ByteArray& byte_array){
					}),
					Session::OnClosedFunc([session_pool](Session::Pointer session){
						session_pool->Erase(session);
					})
				);
			}
			catch(...){
				std::cout << "invalid node_id" << std::endl;
			}
		});
	shell.Register("close", "search key hash.", 
		[client, session_pool](const neuria::test::CuiShell::ArgList& args){
			for(auto session : *session_pool){
				session_pool->Erase(session);
			}
		});
	
	shell.Start();
	t.join();

    return 0;
}
#endif
