#include "..\..\include\subsystem\lagcompsys.h"
#include <algorithm>


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

using namespace Envy::SourceEngine;
namespace Envy
{

	CACHED ConVar* cl_interp_ratio;
	CACHED ConVar* cl_updaterate;
	CACHED ConVar* sv_maxunlag;
	void Envy::LagCompensationSubsystem::Init()
	{
		auto icvar = Interfaces::Instance()->GetInterface<ICvar>();
		cl_interp_ratio = (*icvar)->FindVar("cl_interp_ratio");
		cl_updaterate = (*icvar)->FindVar("cl_updaterate");

		m_flLerpTime = (cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
	}
	void LagCompensationSubsystem::OnFrameStageNotify(
		ClientFrameStage_t stage
	)
	{
		auto entitylist = Interfaces::Instance()->GetInterface<IClientEntityList>();
		auto globals = Interfaces::Instance()->GetInterface<CGlobalVarsBase>();
		auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>();
		if (!(*engine)->IsConnected() || !(*engine)->IsInGame())
			return;
		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			m_flLerpTime = (cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
			auto playerlist = entitylist->GetAllPlayers();
			for (auto& player : playerlist)
			{
				bool ignore = false;
				auto i = player->EntIndex();
				auto track = &m_PlayerTrack[i - 1];

				if (player == g_LocalPlayer) ignore = true;
				if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) ignore = true;
				if (player->IsDormant() || !player->IsAlive()) ignore = true; //Don't perform lag compensation on dead or dormant players

				if (ignore)
				{
					if (track->Count() > 0)
					{
						track->RemoveAll();
					}
					continue;
				}

				int tailidx = track->Tail();
				while (track->IsValidIndex(tailidx))
				{
					LagRecord& tail = track->Element(tailidx);
					if (IsRecordValid(tail))
					{
						break;
					}
					track->Remove(tailidx);
					tailidx = track->Tail();
				}

				if (track->Count() > 0)
				{
					LagRecord& head = track->Element(track->Head());

					if (head.m_flSimulationTime >= player->m_flSimulationTime()) //Player simulation time has not advanced, perhaps he is fakelagging? 
						continue; 
				}

				LagRecord& record = track->Element(track->AddToHead());
				record.m_fFlags = 0;
				record.m_vecAngles = player->m_angEyeAngles();
				record.m_vecOrigin = player->m_vecOrigin();
				record.m_vecMaxsPreScaled = player->m_Collision()->m_vecMaxs();
				record.m_vecMinsPreScaled = player->m_Collision()->m_vecMins();
				record.m_flSimulationTime = player->m_flSimulationTime();
				record.m_tickcount = (*globals)->tickcount;

				m_pResolver->UpdateResolverData(player, record);
			}
		}
	}
	bool LagCompensationSubsystem::IsRecordValid(const LagRecord & record)
	{
		float correct = 0.f;
		auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>();
		auto globals = Interfaces::Instance()->GetInterface<CGlobalVarsBase>();

		correct += (*engine)->GetNetChannelInfo()->GetAvgLatency(FLOW_INCOMING);
		correct += (*engine)->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING);
		correct += m_flLerpTime; //Lerp

		std::max(0.f, std::min(correct, 1.f));

		float delta = correct - ((*globals)->curtime - record.m_flSimulationTime);

		if (fabsf(delta) > 0.2f)
		{
			return false;
		}

		return true;
	}
	void LagCompensationSubsystem::RewindPlayer(
		C_BasePlayer* player, 
		Vector& origin,
		QAngle& angles, 
		SourceEngine::Vector& mins,
		SourceEngine::Vector& maxs,
		float simtime,
		int& tickcount
	)
	{

		LagRecord& record = m_pResolver->GetResolverRecord(player);
		origin = record.m_vecOrigin;
		angles = record.m_vecAngles;
		simtime = record.m_flSimulationTime;
		mins = record.m_vecMinsPreScaled;
		maxs = record.m_vecMaxsPreScaled;
		tickcount = record.m_tickcount + TIME_TO_TICKS(m_flLerpTime);
	}
}
