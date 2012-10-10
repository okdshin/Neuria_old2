#pragma once
//TypeWrapper:20120920
#include <iostream>

namespace neuria{
namespace utility{ 

template<class Type, class Id>
class TypeWrapper
{
public:
	using WrappedType = Type;
	TypeWrapper(const Type& val) : val(val) {}
	auto operator()() -> Type& { return val; }
	auto operator()()const -> Type { return val; }

private:
	Type val;
};

template<class Type, class Id>
auto operator<<(std::ostream& os, 
		const TypeWrapper<Type, Id>& type_wrapper) -> std::ostream& {
	os << type_wrapper();
	return os;
}

}
}

