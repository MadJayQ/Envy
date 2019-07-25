#include "triggerbot.h"
#include "mathfuncs.h"


#include "subsystem/inputsys.h"

#include <random>

using namespace Envy::SourceEngine;

static float shootTime = -1.f;

namespace Envy
{
	bool Triggerbot::ProcessTriggerBot(SourceEngine::QAngle safeAngle, bool& sendpacket)
	{
		//auto traceSys = g_Subsystems->Get<TraceSubsystem>();
		auto trace = Interfaces::Instance()->GetInterface<IEngineTrace>();
		auto inputSys = g_Subsystems->Get<InputSubsystem>();

		auto playerPos = g_LocalPlayer->GetEyePos();
		auto playerWeapon = g_LocalPlayer->m_hActiveWeapon().Get();

		if (!playerWeapon)
			return false;

		auto weaponData = playerWeapon->GetCSWeaponData();
		if (!weaponData)
			return false;

		Vector dst = Vector();
		Vector forward = Vector();
		Math::AngleVectors(safeAngle, forward);

		float range = *(float*)((uintptr_t)weaponData + 0x0108);
		forward *= range;

		dst = playerPos + forward;

		CGameTrace gameTrace;
		CTraceFilter filter;
		filter.pSkip = g_LocalPlayer;
		Ray_t ray;

		float time = (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>())->curtime;

		ray.Init(playerPos, dst);

		(*trace)->TraceRay(
			ray,
			0x4600400B,
			&filter,
			&gameTrace
		);

		auto entity = gameTrace.hit_entity;
		if (!entity)
		{
			shootTime = -1.f;
			return false; //Not looking at anyone anymore
		}

		auto clientClass = entity->GetClientClass();
		if (!clientClass) return false;

		if (clientClass->m_ClassID != 40) return false;

		C_BasePlayer* player = reinterpret_cast<C_BasePlayer*>(entity);

		if (!player->IsAlive() || player->IsDormant() || player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			shootTime = -1.f;
			return false;
		}

		if (gameTrace.hitgroup != HITGROUP_HEAD)
		{
			shootTime = -1.f;
			return false;
		}

		if (inputSys->m_keyMap[Options::Instance()->toggle_triggerbot()] != Envy::KeyState::Down) {
			shootTime = -1.f;
			return false;
		}

		if (!playerWeapon->CanFire()) return false;

		bool canHit = true;

		float delay = 0.f;

		if (Options::Instance()->triggerbot_delay_random())
		{
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_real_distribution<> dist(
				Options::Instance()->triggerbot_delay_min(),
				Options::Instance()->triggerbot_delay_max()
			);

			delay = dist(eng);
		}
		else
		{
			delay = Options::Instance()->triggerbot_delay();
		}

		if (shootTime < 0)
		{
			shootTime = time + (delay * (float)(1 / 1000));
		}
		else if (shootTime > 0.f)
		{
			if (time < shootTime)
			{
				canHit = false;
			}
		}

		if (!canHit) return false;

		return true;
	}
}
