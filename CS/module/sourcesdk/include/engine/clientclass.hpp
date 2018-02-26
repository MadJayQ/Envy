#pragma once

#include "recvtable.h"
#include "clientnetworkable.hpp"

namespace Envy
{
	namespace SourceEngine
	{
		class ClientClass;

		using CreateClientClassFn = IClientNetworkable*(*)(int, int);
		using CreateEventFn = IClientNetworkable*(*)();

		class ClientClass
		{
		public:
			CreateClientClassFn      m_pCreateFn;
			CreateEventFn            m_pCreateEventFn;
			char*                    m_pNetworkName;
			RecvTable*               m_pRecvTable;
			ClientClass*             m_pNext;
			int                      m_ClassID;
		};
	}
}