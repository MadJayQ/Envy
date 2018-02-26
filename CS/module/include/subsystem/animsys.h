#pragma once

#include "subsystem.h"
#include "basehandle.hpp"
namespace Envy
{

	namespace SourceEngine
	{
		class C_BasePlayer;
		class QAngle;
		class Vector;
		class matrix3x4_t;
		class CCSGOPlayerAnimState;

	}
	namespace AnimationState
	{
		void UpdateAnimationState(SourceEngine::CCSGOPlayerAnimState* anim, SourceEngine::QAngle angle);
		void ResetAnimationState(SourceEngine::CCSGOPlayerAnimState* anim);
		void CreateAnimationState(SourceEngine::C_BasePlayer* player, SourceEngine::CCSGOPlayerAnimState* state);
	}
	class AnimationSubsystem : public ICheatSubsystem
	{
	public:
		AnimationSubsystem() 
		{
			m_pLocalPlayerState = nullptr;
			m_angRealAngle = QAngle(0.f, 0.f, 0.f);
			m_angFakeAngle = QAngle(0.f, 0.f, 0.f);
			m_localHandle = 0UL;
			m_flLowerBodyYaw = 0.f;
			m_flNextTurnTime = 0.f;
			m_flLowerBodyYawDelta = 0.f;
			m_bNextSet = false;
		}
		virtual void Init() override;
		bool SetupBones(
			SourceEngine::C_BasePlayer*,
			SourceEngine::QAngle,
			SourceEngine::Vector,
			SourceEngine::matrix3x4_t*,
			float,
			int
		);
		void SimulateServerAnimationCode(SourceEngine::QAngle angle, float currtime);
		bool& IsSet() { return m_bNextSet; }
		float GetNextTurnTime() const { return m_flNextTurnTime; }
		float GetLBYDelta() const { return m_flLowerBodyYawDelta; }
	private:
		SourceEngine::CCSGOPlayerAnimState* m_pLocalPlayerState;
		SourceEngine::QAngle m_angRealAngle;
		SourceEngine::QAngle m_angFakeAngle;
		SourceEngine::CBaseHandle m_localHandle;
		float m_flLowerBodyYaw;
		float m_flSpawnTime;
		float m_flNextTurnTime;
		float m_flLowerBodyYawDelta;
		bool m_bNextSet;

		int m_nLastAnimFrame = 0;
		float m_flLastAnimTime = 0.f;
	};
}
