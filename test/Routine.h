#pragma once
//Routine:20121009
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>

namespace neuria{
namespace test{


template<class ValueType>
auto GetInput(const std::string& prompt) -> ValueType
{
	std::cout << prompt << std::flush;
	std::string input;
	std::getline(std::cin, input);
	try
	{
		return boost::lexical_cast<ValueType>(input);
	}
	catch(boost::bad_lexical_cast& e)
	{
		std::cout << "error. retry." << "\n";
		return GetInput<ValueType>(prompt);	
	}
}

}
}

