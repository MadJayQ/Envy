#pragma once
#include <memory>

template<class C>
class Singleton
{
public:
	Singleton(C const&) = delete;
	C& operator = (C const&) = delete;

	static C* Instance()
	{
		static std::shared_ptr<C> s_pInstance{ new C };
		return s_pInstance.get();
	}

protected:
	Singleton<C>() {}
};