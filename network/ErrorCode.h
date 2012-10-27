#pragma once
//ErrorCode:20121025
#include <iostream>
#include <boost/system/error_code.hpp>
#include "../utility/Utility.h"

namespace neuria{
namespace network{
class ErrorCodeType{};
using ErrorCode = utility::TypeWrapper<boost::system::error_code, ErrorCodeType>;

auto operator<<(std::ostream& os, const ErrorCode& error_code) -> std::ostream& {
	os << error_code().message();
	return os;	
}

}
}

