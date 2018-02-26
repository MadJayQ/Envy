#pragma once
#include <stdint.h>

#include "rwbase.hpp"

#ifdef CDECL
#undef CDECL
#endif

enum CallingConvention { STDCALL, CDECL, FASTCALL, THISCALL, VECTORCALL };
constexpr uintptr_t INVALID = 0xFFFFFFFF;

template <typename Addr> struct _ccall_impl_base
{
	_ccall_impl_base(const Addr& addr) : m_addr(addr) {}

	void assign(Addr addr) { m_addr = addr; }
	Addr& ptr() { return m_addr; }
protected:
	Addr m_addr;
};

template<typename Addr, CallingConvention Call> struct _ccall_impl_ : public _ccall_impl_base<Addr>
{ 
	using Super = _ccall_impl_base<Addr>;
	_ccall_impl_(const Addr& addr) : Super(addr) {}
};
template < >  struct _ccall_impl_<uintptr_t, STDCALL> : public _ccall_impl_base<uintptr_t>
{
	using Super = _ccall_impl_base<uintptr_t>;
	_ccall_impl_(const uintptr_t& addr) : Super(addr) {}
	template<typename Ret, typename ... Args>
	Ret call(Args ... args)
	{
		using callable_t = Ret(__stdcall *)(Args...);
		return reinterpret_cast<callable_t>(m_addr)(std::forward<Args>(args)...);
	}
private:

};
template < >  struct _ccall_impl_<uintptr_t, CDECL> : public _ccall_impl_base<uintptr_t>
{
	using Super = _ccall_impl_base<uintptr_t>;
	_ccall_impl_(const uintptr_t& addr) : Super(addr) {}
	template<typename Ret, typename ...Args>
	Ret call(Args ... args)
	{
		using callable_t = Ret(__cdecl *)(Args...);
		return reinterpret_cast<callable_t>(m_addr)(std::forward<Args>(args)...);
	}
};

template < > struct _ccall_impl_<uintptr_t, THISCALL> : public _ccall_impl_base<uintptr_t>
{
	using Super = _ccall_impl_base<uintptr_t>;
	_ccall_impl_(const uintptr_t& addr) : Super(addr) {}
	template<typename Ret, typename ...Args>
	Ret call(Args ... args)
	{
		using callable_t = Ret(__thiscall *)(Args...);
		return reinterpret_cast<callable_t>(m_addr)(std::forward<Args>(args)...);
	}
};
template<
	typename Addr,
	typename Rw, 
	CallingConvention Call,
	typename Ret,
	typename ... Args
>
class FunctionBase
{
public:
	FunctionBase(Addr addr, Rw rw) : m_callable(addr)
	{

	}
	Ret operator ()(Args... args)
	{
		return m_callable.call<Ret, Args...>(std::forward<Args&&>(args)...);
	}

	bool operator == (const Addr& addr)
	{
		return (m_callable.ptr() == addr);
	}

	bool operator != (const Addr& addr)
	{
		return (m_callable.ptr() != addr);
	}

	void operator = (const Addr& addr)
	{
		m_callable.ptr() = addr;
	}

	_ccall_impl_<Addr, Call>* operator -> () { return &m_callable; }

protected:
	Addr m_addr;
	Rw m_readwrite;
	_ccall_impl_<Addr, Call> m_callable;

};

template<
	typename Ret, 
	CallingConvention Call = STDCALL, 
	typename ... Args
>
struct Function : public FunctionBase<uintptr_t, Internal, Call, Ret, Args ...> 
{
	using Super = FunctionBase<uintptr_t, Internal, Call, Ret, Args ...>;
	explicit Function(uintptr_t addr) : Super(addr, Internal(addr))
	{

	}

	Function() : Super(INVALID, Internal(INVALID))
	{

	}

};
