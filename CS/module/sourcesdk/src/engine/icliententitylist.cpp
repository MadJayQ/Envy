#include "icliententitylist.hpp"
#include "structs.h"

using namespace Envy::SourceEngine;
namespace Envy
{
	C_BaseEntity * Envy::IClientEntityList::GetEntityByIndex(int idx)
	{
		return static_cast<C_BaseEntity*>(m_data->GetClientEntity(idx));
	}
	C_BaseEntity * IClientEntityList::GetEntityByHandle(CBaseHandle h)
	{
		return static_cast<C_BaseEntity*>(m_data->GetClientEntityFromHandle(h));
	}
	C_BasePlayer * IClientEntityList::GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}
	std::vector<C_BasePlayer*> IClientEntityList::GetAllPlayers()
	{
		//TODO(Jake): Cache this shit lmfao
		std::vector<C_BasePlayer*> ret;
		for (auto i = 1; i <= m_data->GetHighestEntityIndex(); i++)
		{
			auto ent = GetEntityByIndex(i);
			if (!ent) continue;
			if (!ent->IsPlayer()) continue;
			ret.push_back(reinterpret_cast<C_BasePlayer*>(ent));
		}

		return ret;
	}
}
