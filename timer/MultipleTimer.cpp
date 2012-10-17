#ifdef MULTIPLETIMER_UNIT_TEST
#include "MultipleTimer.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace neuria;
using namespace neuria::timer;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	boost::asio::io_service::work work{service};
	boost::thread thread(boost::bind(&boost::asio::io_service::run, &service));

	auto multi_timer = MultipleTimer::Create(service);

	multi_timer->AddCallbackFuncAndStartTimer(2, 
		[](){
			std::cout << "hello" << std::endl;
			return IsContinue(true);
		}
	);

	sleep(1);
	multi_timer->AddCallbackFuncAndStartTimer(2, 
		[](){
			std::cout << "morning" << std::endl;
			return IsContinue(false);
		}
	);

	//service.run();
	thread.join();

    return 0;
}

#endif
