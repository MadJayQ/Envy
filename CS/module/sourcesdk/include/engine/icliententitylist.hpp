#pragma once

#include "ihandleentity.hpp"
#include "interfaces.h"
#include "basehandle.hpp"
namespace Envy
{
	namespace SourceEngine
	{
		class IClientNetworkable;
		class IClientEntity;
		class C_BaseEntity;
		class C_BasePlayer;

		class _IClientEntityList_
		{
		public:
			virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
			virtual void*                 vtablepad0x1(void) = 0;
			virtual void*                 vtablepad0x2(void) = 0;
			virtual IClientEntity*        GetClientEntity(int entNum) = 0;
			virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
			virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
			virtual int                   GetHighestEntityIndex(void) = 0;
			virtual void                  SetMaxEntities(int maxEnts) = 0;
			virtual int                   GetMaxEntities() = 0;
		};
	}

	class IClientEntityList : public SourceInterface<SourceEngine::_IClientEntityList_>
	{
	public:
		IClientEntityList() {}

		SourceEngine::C_BaseEntity* GetEntityByIndex(int idx);
		SourceEngine::C_BaseEntity* GetEntityByHandle(SourceEngine::CBaseHandle h);
		SourceEngine::C_BasePlayer* GetPlayerByIndex(int i);
		std::vector<SourceEngine::C_BasePlayer*> GetAllPlayers();

		virtual Interface* Init() override
		{
			m_data = reinterpret_cast<Interface*>(
				Interfaces::Instance()->GetInterfaceAddr("VClientEntityList")
			);
			SourceInterface<SourceEngine::_IClientEntityList_>::Init();
			return m_data;
		}
	};
}