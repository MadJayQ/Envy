#pragma once
#include "process.h"

enum ADDR_TYPE
{
	ADDR_32BIT,
	ADDR_64BIT
};


class AddressProperties
{
public:
	explicit AddressProperties(
		Process p
	) :
		m_parent(p) 
	{
		
	}

	AddressProperties() {}

private:
#ifdef _M_AMD64
	ADDR_TYPE m_type = ADDR_64BIT;
#else
	ADDR_TYPE m_type = ADDR_32BIT;
#endif
	Process m_parent;
};
