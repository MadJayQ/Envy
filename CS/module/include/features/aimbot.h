#pragma once

#include "csgosdk.h"
#include "structs.h"

namespace Envy
{

	struct AimbotItr
	{
		float fov;
		float distance;
		int hp;
		float threat; //TODO
		SourceEngine::C_BasePlayer* player;
	};
	struct HitscanItr
	{
		float fov;
		SourceEngine::Vector pos;
		int hitbox;
		std::vector<SourceEngine::Vector> points;
	};
	struct HitscanArgs
	{
		std::vector<HitscanItr> hitscans;
		HitscanItr* bestHitscan;
		float bestDamage;
		SourceEngine::C_BasePlayer* player;
	};
	class Aimbot
	{
	public:
		Aimbot() 
		{
			m_angViewAngles = SourceEngine::QAngle(0.f, 0.f, 0.f);
			m_angAimAngles = SourceEngine::QAngle(0.f, 0.f, 0.f);
			m_pTarget = nullptr;
		}

		bool ProcessAimbot(SourceEngine::QAngle& aimangle, bool& sendpacket);
		SourceEngine::Vector GetHitboxPosition(SourceEngine::C_BasePlayer* player, int hitbox_num, SourceEngine::matrix3x4_t* bones);
		void CompensateRecoil(SourceEngine::QAngle& angle);
		void ApplySmooth(SourceEngine::QAngle& viewangles, SourceEngine::QAngle& aimangles, int divisor);
		HitscanArgs* Hitscan(SourceEngine::C_BasePlayer* player, const std::vector<int>& hitboxes, SourceEngine::matrix3x4_t* bones);
		void PerformHitscan(HitscanArgs* args);

		SourceEngine::QAngle GetAimAngle() const { return m_angAimAngles; }

		bool Valid();

		void ProcessAutoPistol(SourceEngine::CUserCmd* cmd);

	private:
		SourceEngine::QAngle m_angViewAngles;
		SourceEngine::QAngle m_angAimAngles;
		SourceEngine::C_BasePlayer* m_pTarget;

	};
}