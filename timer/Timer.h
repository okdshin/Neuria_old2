#pragma once
//Timer:20121013
#include <iostream>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../Time.h"
#include "../utility/Utility.h"

namespace neuria{
namespace timer{

class IsContinueType{};
using IsContinue = utility::TypeWrapper<bool, IsContinueType>;

class Timer : public boost::enable_shared_from_this<Timer> {
public:
	using Pointer = boost::shared_ptr<Timer>;
	using CallbackFunc = boost::function<IsContinue ()>;
	using OnEraseFunc = boost::function<void (Pointer)>;

	static auto Create(boost::asio::io_service& service, 
			unsigned int interval_sec, 
			CallbackFunc callback_func, OnEraseFunc on_erase_func) -> Pointer {
		return Pointer(new Timer(service, boost::posix_time::seconds(interval_sec), 
			callback_func, on_erase_func));
	}

	auto Start() -> void {
		this->Call();
	}

private:
	Timer(boost::asio::io_service& service, 
		const boost::posix_time::time_duration& interval, 
		CallbackFunc callback_func, OnEraseFunc on_erase_func) 
		: service(service), timer(service, interval), interval(interval), 
		callback_func(callback_func), on_erase_func(on_erase_func){}

	auto Call() -> void {
		this->timer.expires_at(
			this->timer.expires_at() + this->interval);
		this->timer.async_wait([this](const boost::system::error_code&){
			const auto is_continue = this->callback_func();
			if(!is_continue()){
				this->on_erase_func(this->shared_from_this());
				return;
			}
			this->Call();	
		});	
	}

	boost::asio::io_service& service;
	boost::asio::deadline_timer timer;
	boost::posix_time::time_duration interval;
	CallbackFunc callback_func;
	OnEraseFunc on_erase_func;
};


}
}

