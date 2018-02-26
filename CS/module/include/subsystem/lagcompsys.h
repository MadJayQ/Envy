#pragma once

#include "subsystem.h"
#include "csgosdk.h"
#include "structs.h"
#include "utllinkedlist.hpp"

#include "resolver.h"
namespace Envy
{
	class LagCompensationSubsystem : public ICheatSubsystem
	{
	public:
		LagCompensationSubsystem() 
		{
			m_pResolver = std::make_unique<Resolver>();
		}

		virtual void Init() override;


		void OnFrameStageNotify(
			SourceEngine::ClientFrameStage_t stage
		);

		bool IsRecordValid(const SourceEngine::LagRecord& record);
		void RewindPlayer(
			SourceEngine::C_BasePlayer* player, 
			SourceEngine::Vector& origin, 
			SourceEngine::QAngle& angles, 
			SourceEngine::Vector& mins,
			SourceEngine::Vector& maxs,
			float simtime,
			int& tickcount
		);

		static inline int TIME_TO_TICKS(float time) 
		{
			return (int)(0.5f + (float)(time) / (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>())->interval_per_tick);
		}

		static inline float TICKS_TO_TIME(int tick) 
		{
			return (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>())->interval_per_tick * tick;
		}

	private:
		SourceEngine::CUtlFixedLinkedList<SourceEngine::LagRecord> m_PlayerTrack[64];
		std::unique_ptr<Resolver> m_pResolver;
		float m_flLerpTime;
	};
}