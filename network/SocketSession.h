#pragma once
//SocketSession:20120816
#include <iostream>
#include <cassert>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Session.h"
#include "SocketRoutine.h"
#include "SocketSessionHeader.h"
#include <fstream>

namespace neuria{
namespace network{

class SocketSession : public Session, public boost::enable_shared_from_this<SocketSession>
{
public:
	using Pointer = boost::shared_ptr<SocketSession>;

	~SocketSession(){
		this->os << "SESSION DELETED." << std::endl;
		this->os << "socket close...." << std::endl;
		this->sock.close();
		this->os << "socket closed." << std::endl;

	}

	static auto Create(
		boost::asio::io_service& service, int buffer_size,
		OnReceivedFunc on_receive_func, OnClosedFunc on_close_func,
		std::ostream& os) -> SocketSession::Pointer {
			
		auto session =  Pointer(new SocketSession(
			service, buffer_size, on_receive_func, on_close_func, os));
		os << session << " socket created." << std::endl;
		return session;
	}

	static auto GetRemoteAddressStr(SocketSession::Pointer session) -> std::string {
		return session->GetSocketRef().remote_endpoint().address().to_string();	
	}

	static auto GetRemotePort(SocketSession::Pointer session) -> int {
		return session->GetSocketRef().remote_endpoint().port();
	}
	
	auto GetSocketRef() -> boost::asio::ip::tcp::socket& {
		return this->sock;
	}

private: //over ridden
	auto DoGetNodeId() -> NodeId {
		try{
			return CreateSocketNodeId(
				GetRemoteAddressStr(this->shared_from_this()), 
				GetRemotePort(this->shared_from_this()));	
		}
		catch(...){
			assert(!"socket is not connected.");
		}
	}

	auto DoStartReceive() -> void {
		this->os << "start receive" << std::endl;
		HandlOnReceiveHeader();
	}

	auto DoSend(const ByteArray& byte_array,
			Session::OnSendFinishedFunc on_send_finished_func) -> void {
		assert(this->sock.is_open());
		bool is_empty = this->send_byte_array_queue.empty();
		this->send_byte_array_queue.push_back(byte_array);
		if(is_empty) { //start new
			this->HandlOnSend(on_send_finished_func);
		}
	}
	
	auto DoClose() -> void {
		assert(this->sock.is_open());
		this->sock.get_io_service().dispatch(
			boost::bind(&SocketSession::HandlClose, shared_from_this()));		
	}

private:
	SocketSession(boost::asio::io_service& service, int buffer_size,
		OnReceivedFunc on_receive_func,
		OnClosedFunc on_close_func,
		std::ostream& os)
			:sock(service), on_receive_func(on_receive_func), on_close_func(on_close_func), 
			part_of_array(buffer_size), received_byte_array(), //on_send_strand(service), 
			os(os){
			this->os << "SESSION CREATED" << std::endl;	
		}

	auto HandlOnReceiveHeader() -> void {
		this->sock.async_read_some(
			boost::asio::buffer(part_of_array),
			boost::bind(
				&SocketSession::OnReceiveHeader, 
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

	}

	auto OnReceiveHeader(const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		auto temp_part_of_array = ByteArray();
		std::copy(this->part_of_array.begin(), 
			this->part_of_array.begin() + bytes_transferred, 
			std::back_inserter(temp_part_of_array));
		if(!error_code){
			/*
			this->os << "partly received(header): \"" 
				<< std::string(this->part_of_array.begin(), this->part_of_array.begin()+bytes_transferred) << "\"\n" 
				<< temp_part_of_array
				<< std::endl;
			*/
			std::copy(part_of_array.begin(), part_of_array.begin()+bytes_transferred, 
				std::back_inserter(this->received_byte_array));
			if(SocketSessionHeader::IsEnableParse(this->received_byte_array)){
				this->header = SocketSessionHeader::Parse(this->received_byte_array);
				this->os << "header: " << this->header << std::endl;
				this->os << this->received_byte_array << std::endl;
				OnReceiveBodyProxy(error_code, bytes_transferred);
			}
			else{
				HandlOnReceiveHeader();	
			}
		}
		else if(this->sock.is_open()){
			std::cout << "peer socket closed" << std::endl;
			//this->HandlClose();		
		}
		else {
			this->os << "receiving stop" << std::endl;
		}
	}
	
	auto HandlOnReceiveBody() -> void {
		this->sock.async_read_some(
			boost::asio::buffer(part_of_array),
			boost::bind(
				&SocketSession::OnReceiveBody, 
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

	}

	auto OnReceiveBody(const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(!error_code){
			auto temp_part_of_array = ByteArray();
			std::copy(this->part_of_array.begin(), 
				this->part_of_array.begin() + bytes_transferred, 
				std::back_inserter(temp_part_of_array));
			/*
			this->os << "partly received(body): \"" 
				<< std::string(this->part_of_array.begin(), this->part_of_array.begin()+bytes_transferred) << "\"\n" 
				<< temp_part_of_array
				<< std::endl;
			*/
			std::copy(part_of_array.begin(), part_of_array.begin()+bytes_transferred, 
				std::back_inserter(this->received_byte_array));
			OnReceiveBodyProxy(error_code, bytes_transferred);
		}
		else if(this->sock.is_open()){
			std::cout << "peer socket closed" << std::endl;
			//this->HandlClose();		
		}
		else {
			this->os << "receiving stop" << std::endl;
		}
	}
	
	auto OnReceiveBodyProxy(const boost::system::error_code& error_code, 
			std::size_t bytes_transferred) -> void {
		if(!error_code){
			if(this->received_byte_array.size() > 
					header.GetBodySize()+header.GetHeaderSize()){
				this->os << "OOOOO" << std::endl;	
			}
			if(this->received_byte_array.size() >= 
					this->header.GetHeaderSize()+this->header.GetBodySize()){
				this->os << "whole bytes received:" 
					<< this->received_byte_array.size() 
					<< " bytes transffered\n\"" 
					<<"(...)"//<< utility::ByteArray2String(this->received_byte_array) 
					<< "\"" << std::endl;
				auto body_byte_array = ByteArray();
				std::copy(this->received_byte_array.begin()+header.GetHeaderSize(), 
					this->received_byte_array.begin()
						+header.GetHeaderSize()
						+header.GetBodySize(), 
					std::back_inserter(body_byte_array));
				this->os << "body : \"" 
					<< utility::ByteArray2String(body_byte_array) 
					<< "\"" << std::endl;
				this->sock.get_io_service().dispatch(boost::bind(
					this->on_receive_func, this->shared_from_this(), 
					body_byte_array));
				auto temp_byte_array = neuria::ByteArray();
				std::copy(
					this->received_byte_array.begin()
						+header.GetHeaderSize()
						+header.GetBodySize(),
					this->received_byte_array.end(),
					std::back_inserter(temp_byte_array));
				this->received_byte_array = temp_byte_array;
				HandlOnReceiveHeader();
			}
			else{
				HandlOnReceiveBody();
			}
		}
		else if(this->sock.is_open()){
			std::cout << "peer socket closed" << std::endl;
			//this->HandlClose();		
		}
		else {
			this->os << "receiving stop" << std::endl;
		}
	}


	auto HandlOnSend(Session::OnSendFinishedFunc on_send_finished_func) -> void {
		const auto body_byte_array = send_byte_array_queue.front();
		
		const auto header = SocketSessionHeader(body_byte_array.size());
		std::cout << "send header:" << header << std::endl;
		const auto header_byte_array = header.Serialize();
		
		this->header_and_body_byte_array.clear();
		std::copy(header_byte_array.begin(), header_byte_array.end(), 
			std::back_inserter(header_and_body_byte_array));
		std::copy(body_byte_array.begin(), body_byte_array.end(), 
			std::back_inserter(header_and_body_byte_array));
		
		{
			std::ofstream ofs("./log.txt");
			ofs << "send:\"" << header_and_body_byte_array << "\"" << std::endl;
		}
		
		boost::asio::async_write(
			this->sock, 
			boost::asio::buffer(this->header_and_body_byte_array),
			boost::bind(
				&SocketSession::OnSend,
				shared_from_this(), 
				on_send_finished_func,
				boost::asio::placeholders::error
			)
		);
	}

	auto OnSend(Session::OnSendFinishedFunc on_send_finished_func, 
			const boost::system::error_code& error_code) -> void {
		std::cout << "on send called." << std::endl;
		if(!error_code){
			if(!this->send_byte_array_queue.empty()){
				this->send_byte_array_queue.pop_front();
				if(!this->send_byte_array_queue.empty()){
					this->HandlOnSend(on_send_finished_func);
				}
				else{
					on_send_finished_func(this->shared_from_this());
				}
			}
		}
		else
		{
			this->os << "send failed. error code is "<< error_code.message() << std::endl;
			//this->HandlClose();
		}
	}

	auto HandlClose() -> void {
		//this->os << GetRemoteAddressStr(this->shared_from_this()) << ":"
		//	<< GetRemotePort(this->shared_from_this()) << " closed" << std::endl;
		//assert(this->sock.is_open());
		if(this->sock.is_open()){
			//this->sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			this->sock.close();
		}
		else{
			std::cout << "!!!!!dual close!!!!!" << std::endl;	
		}
		this->os << this->shared_from_this() << " socket close" << std::endl;
		this->on_close_func(this->shared_from_this());
	}

	boost::asio::ip::tcp::socket sock;
	OnReceivedFunc on_receive_func;
	OnClosedFunc on_close_func;
	ByteArray part_of_array;
	ByteArray received_byte_array;
	std::deque<ByteArray> send_byte_array_queue;
	SocketSessionHeader header;
	ByteArray header_and_body_byte_array;
	//boost::asio::strand on_send_strand;
	std::ostream& os;

};

inline auto CreateTestSocketSession(boost::asio::io_service& service) -> SocketSession::Pointer {
	return SocketSession::Create(service, 128, 
		[](Session::Pointer session, const ByteArray&)
			{ std::cout << "on receive !!!" << std::endl; }, 
		[](Session::Pointer){ std::cout << "on close !!!" << std::endl; }, 
		std::cout);
}

}
}

