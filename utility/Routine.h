#pragma once
//Routine:20121009
#include <iostream>
#include <boost/random.hpp>
#include "../ByteArray.h"

namespace neuria{
namespace utility{

static auto RANDOM = boost::random::mt19937(static_cast<unsigned long>(std::time(0)));
using RandomRange = boost::random::uniform_int_distribution<>;
//RandomRange(1,6) is equalt to dice.

inline auto Random(int min, int max) -> int {
	return RandomRange(min, max)(RANDOM);
}

inline auto String2ByteArray(const std::string& str) -> ByteArray {
	return ByteArray(str.begin(), str.end());
}

inline auto ByteArray2String(const ByteArray& byte_array) -> std::string {
	return std::string(byte_array.begin(), byte_array.end());
}

class RandomElementSelector{
public:
	RandomElementSelector()
		: gen(boost::mt19937(static_cast<unsigned long>(std::time(0)))){}

	template<class Container>
	auto operator()(Container& container) -> typename Container::iterator::value_type {
		auto dst = boost::uniform_smallint<>(
			0, container.end()-container.begin()-1);//small_intで十分
		auto rand = boost::variate_generator<
			boost::mt19937&, boost::uniform_smallint<>>(this->gen, dst);
		return *(container.begin() + rand());
	}

private:
	boost::mt19937 gen;
};

inline auto StrJoin(const std::vector<std::string>& str_vect, std::string delim) -> std::string
{
	auto integrated = str_vect.front();
	for(unsigned int i = 1; i < str_vect.size(); i++)
	{
		integrated = integrated + delim + str_vect.at(i);
	}
	return integrated;
}

}
}

