#pragma once
//MultipleTimer:20121015
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "Timer.h"

namespace neuria{
namespace timer{

class MultipleTimer{
public:
	using Pointer = boost::shared_ptr<MultipleTimer>;
	using OnEraseFunc = boost::function<void ()>;

    static auto Create(boost::asio::io_service& service) -> Pointer {
		return Pointer(new MultipleTimer(service));	
	}

	auto AddCallbackFuncAndStartTimer(unsigned int sec_interval, 
			Timer::CallbackFunc callback_func, 
			OnEraseFunc on_erase_func=[](){}) -> void {
		auto timer = Timer::Create(this->service, sec_interval, 
			callback_func, 
			[on_erase_func, this](Timer::Pointer timer){
				on_erase_func(); 
				this->timer_list.erase(std::find(
					this->timer_list.begin(), this->timer_list.end(), timer));
				std::cout << "timer was erased." << std::endl;
			}
		);
		timer_list.push_back(timer);
		timer->Start();
	}

private:
    MultipleTimer(boost::asio::io_service& service) : service(service){}
	
	std::vector<Timer::Pointer> timer_list;

	boost::asio::io_service& service;

};

}
}

