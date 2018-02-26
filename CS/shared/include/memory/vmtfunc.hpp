#pragma once
#include "function.hpp"

template<typename Ret, CallingConvention Call, typename ... Args>
class VMTFunction
{
	using Func = Function<Ret, Call, Args...>;
public:
	VMTFunction(uintptr_t addr, uintptr_t oldaddr) : Super(addr)
	{
		m_original = Func(oldaddr);
		m_new = Func(addr);
	}
private:
	Func m_original;
	Func m_new;
};