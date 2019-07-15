#include "triggerbot.h"
#include "mathfuncs.h"


#include "subsystem/inputsys.h"

using namespace Envy::SourceEngine;

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

		ray.Init(playerPos, dst);

		(*trace)->TraceRay(
			ray,
			0x4600400B,
			&filter,
			&gameTrace
		);

		auto entity = gameTrace.hit_entity;
		if (!entity) return false;

		auto clientClass = entity->GetClientClass();
		if (!clientClass) return false;

		if (clientClass->m_ClassID != 40) return false;

		C_BasePlayer* player = reinterpret_cast<C_BasePlayer*>(entity);

		if (!player->IsAlive()) return false;
		if (player->IsDormant()) return false;
		if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) return false;

		if (gameTrace.hitgroup != HITGROUP_HEAD)
			return false;

		if (inputSys->GetKeyState(VK_LMENU) != Envy::KeyState::Pressed) {
			return false;

		}
		if (!playerWeapon->CanFire()) return false;

		bool canHit = true;

		if (!canHit) return false;

		return true;
	}
}
