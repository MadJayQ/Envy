#pragma once
#include <stdint.h>

#include "addressproperties.hpp"
#include "rwbase.hpp"


template<typename Addr, typename Rw> 
class MemoryAddressBase
{
	using _Self = MemoryAddressBase<Addr, Rw>;
public:
	MemoryAddressBase() {}
	Addr ptr() { return m_address; }

	Rw* operator ->() { return &m_readwrite; }

	MemoryAddressBase<Addr, Rw> operator = (const uintptr_t& other)
	{
		_Self copy = *this;
		copy.m_address = other;
		copy.m_readwrite.m_addr = 0ther;
		return copy;
	}

	MemoryAddressBase<Addr, Rw> operator + (const uintptr_t& other)
	{
		_Self copy = *this;
		copy.m_address += other;
		copy.m_readwrite.m_addr += other;
		return copy;
	}

protected:
	Addr m_address;
	AddressProperties m_properties;
	Rw m_readwrite;

};


#if defined(ENVY_EXTERNAL)
template<typename Addr>
class _MemoryAddress : public MemoryAddressBase<Addr, External>
{
	using Super = MemoryAddressBase<Addr, External>;
public:
	_MemoryAddress(Addr addr, Process p)
	{
		Super::m_address = addr;
		m_properties = AddressProperties(
			p
		);
		this->m_readwrite = External(addr, p);
	}
};

typedef _MemoryAddress<uintptr_t> MemoryAddress;
#else
template<typename Addr>
class _MemoryAddress : public MemoryAddressBase<Addr, Internal>
{
	using Super = MemoryAddressBase<Addr, Internal>;
public:
	_MemoryAddress(Addr addr)
	{
		Super::m_address = addr;
		m_properties = AddressProperties(
			m_readwrite.GetProcess()
		);
		this->m_readwrite = Internal(addr);
	}
};
typedef _MemoryAddress<uintptr_t> MemoryAddress;
#endif

typedef uintptr_t Address;
typedef uint16_t Ordinal;