#pragma once

#include "subsystem.h"
#include "csgosdk.h"


namespace Envy
{
	namespace SourceEngine
	{
		using Bonecache = CUtlVector<matrix3x4a_t, CUtlMemoryAligned<matrix3x4a_t, 16>>;
		using Bonearray = std::array<matrix3x4a_t, 128>;
	}
	namespace BulletPenetration
	{
		bool TraceToExit(
			SourceEngine::Vector& end,
			SourceEngine::Vector& start,
			SourceEngine::Vector& dir, 
			SourceEngine::trace_t* enter,
			SourceEngine::trace_t* exit
		);
		bool HandleBulletPenetration(
			SourceEngine::surfacedata_t* enter_surface,
			SourceEngine::trace_t* enter_trace,
			SourceEngine::Vector direction,
			SourceEngine::Vector* origin,
			float penetration,
			int& penetration_count,
			float& bulletdamage
		);
		float ScaleDamage(SourceEngine::C_BasePlayer* player, float dmg, float ratio, int hitgroup);
		bool IsBreakableEntity(SourceEngine::C_BaseEntity* entity);
	}

	class TraceSubsystem : public ICheatSubsystem
	{
	public:
		TraceSubsystem()
		{

		}

		virtual void Init() override;
		bool PerformTrace(
			const SourceEngine::Vector& src, 
			const SourceEngine::Vector& dst,
			uint32_t mask, 
			float tolerance, 
			SourceEngine::C_BaseEntity* skip, 
			SourceEngine::C_BaseEntity* ent = NULL
		);
		void GetPointDamage(
			SourceEngine::C_BasePlayer* player,
			SourceEngine::Vector& start,
			SourceEngine::Vector& end,
			float& finaldmg
		);

		void EndCustomTrace();
		void BeginCustomTrace(
			SourceEngine::C_BasePlayer* player,
			SourceEngine::matrix3x4_t* matrix
		);

	private:
		SourceEngine::C_BasePlayer* m_pCustomTracePlayer;
		SourceEngine::Bonecache* m_pBoneCache;
		SourceEngine::Bonearray m_BonecacheBackup;
		int m_iMostRecentBoneCounterBackup;
		int* m_iMostRecentBoneCounterPtr;
	};
}