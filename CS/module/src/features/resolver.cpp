#include "resolver.h"

#include "lagcompsys.h"

using namespace Envy::SourceEngine;
namespace Envy
{
	void Resolver::UpdateResolverData(C_BasePlayer* player, const LagRecord& record)
	{
		int idx = player->EntIndex();

		auto lastRecord = m_LatestUpdateRecords[idx];
		if (player->m_flLowerBodyYawTarget() != lastRecord.m_flLowerBodyYawTarget && player->m_vecVelocity().Length() > 0.1)
		{
			m_LBYTUpdateRecords[idx] = record;
			m_LBYTUpdateRecords[idx].m_vecAngles.yaw = player->m_flLowerBodyYawTarget();
		}

		m_LatestUpdateRecords[idx] = record;

		if (Options::Instance()->resolver_force_lby())
		{
			m_LatestUpdateRecords[idx].m_vecAngles.yaw = player->m_flLowerBodyYawTarget();
		}
	}
	LagRecord Resolver::GetResolverRecord(C_BasePlayer* player)
	{
		auto lbyrecord = m_LBYTUpdateRecords[player->EntIndex()];
		if (g_Subsystems->Get<LagCompensationSubsystem>()->IsRecordValid(lbyrecord) && Options::Instance()->resolver_last_lby())
		{
			return lbyrecord;
		}

		return m_LatestUpdateRecords[player->EntIndex()];
	}
}
