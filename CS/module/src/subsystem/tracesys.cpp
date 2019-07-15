#include "..\..\include\subsystem\tracesys.h"
#include "structs.h"
#include "function.hpp"
#include "mathfuncs.h"
#include "hooks.h"


using namespace Envy::SourceEngine;
namespace Envy
{
	void TraceSubsystem::Init()
	{
		m_pCustomTracePlayer = nullptr;
		m_pBoneCache = nullptr;
	}

	void TraceLine(Vector start, Vector end, unsigned int mask, C_BaseEntity* ignore, trace_t* trace) {
		Ray_t ray;
		ray.Init(start, end);
		CTraceFilter filter;
		filter.pSkip = ignore;

		(*Interfaces::Instance()->GetInterface<IEngineTrace>())->TraceRay(ray, mask, &filter, trace);
	}

	bool TraceSubsystem::PerformTrace(
		const Vector& src,
		const Vector& dst, 
		uint32_t mask, 
		float tolerance,
		C_BaseEntity* skip,
		C_BaseEntity* ent
	)
	{
		CGameTrace trace;
		Ray_t ray;
		CTraceFilter filter;
		if (skip != NULL) filter.pSkip = skip;

		ray.Init(src, dst);
		(*Interfaces::Instance()->GetInterface<IEngineTrace>())->TraceRay(
			ray,
			mask,
			&filter,
			&trace
		);

		if (ent != NULL) return (trace.hit_entity == ent || trace.fraction >= tolerance);
		return (trace.fraction >= tolerance);
	}

	static uintptr_t clptrtp = (uintptr_t)Peb::Instance()->GetModule("client_panorama.dll").FindPattern("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 0F 57 C9");
	static _declspec(naked) void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
	{
		__asm
		{
			push tr
			push filter
			push mask
			lea edx, vecAbsEnd
			lea ecx, vecAbsStart
			call clptrtp
			add esp, 0xC
		}
	}

	void TraceSubsystem::GetPointDamage(C_BasePlayer* player, Vector& start, Vector& end, float& finaldmg)
	{

		static auto UTIL_TraceLineIgnoreTwoEntities = [](const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, C_BaseEntity* ignore, C_BaseEntity* ignore2, int collisiongroup, trace_t* ptr) 
		{
			Ray_t ray;
			ray.Init(vecAbsStart, vecAbsEnd);
			CTraceFilterSkipTwoEntities traceFilter(ignore, ignore2, collisiongroup);
			(*Interfaces::Instance()->GetInterface<IEngineTrace>())->TraceRay(ray, mask, &traceFilter, ptr);
		};

		auto weapon = player->m_hActiveWeapon().Get();
		if (!weapon)
		{
			finaldmg = 0.f;
			return;
		}
		auto wpndata = weapon->GetCSWeaponData();
		if (!wpndata)
		{
			finaldmg = 0.f;
			return;
		}

		trace_t enter_tr;
		Vector dir = (end - start).Normalized();

		int hitsleft = 4;
		Vector result = start;
		float penetration = wpndata->flPenetration;
		float bulletdamage = wpndata->iDamage;
		float distance = wpndata->flRange;
		float traveled = 0.f;
		int wpnmask = (weapon->m_Item().m_iItemDefinitionIndex()) == WEAPON_TASER ? 0x1100 : 0x1002;
		C_BaseEntity* hit = nullptr;
		while (bulletdamage > 0.0f)
		{
			distance -= traveled;
			Vector endpos = start + dir * distance;
			UTIL_TraceLineIgnoreTwoEntities(result, endpos, 0x4600400b, player, hit, 0, &enter_tr);

			CTraceFilterSkipTwoEntities filter(player, hit, 0);
			UTIL_ClipTraceToPlayers(start, endpos + dir * 40.f, 0x4600400B, &filter, &enter_tr);

			if (enter_tr.fraction == 1.0f)
			{
				//Fix for jumping players
				break;
			}

			surfacedata_t* enter_surf = (*Interfaces::Instance()->GetInterface<IPhysicsSurfaceProps>())->GetSurfaceData(enter_tr.surface.surfaceProps);
			float penmod = *(float*)((uintptr_t)enter_surf + 88);

			traveled += distance * enter_tr.fraction;
			bulletdamage *= powf(wpndata->flRangeModifier, (traveled * 0.002f));

			if (traveled > 3000.f)
			{
				if (penetration > 0 || penmod <= 0.1f)
				{
					hitsleft = 0;
				}
			}
			else if (penmod <= 0.1f)
			{
				hitsleft = 0;
			}
			hit = (C_BaseEntity*)enter_tr.hit_entity;
			if (enter_tr.hitgroup != 0)
			{
				if (hit && hit->IsPlayer())
				{
					bulletdamage = BulletPenetration::ScaleDamage((C_BasePlayer*)hit, bulletdamage, wpndata->flArmorRatio, enter_tr.hitgroup);
					finaldmg = bulletdamage;
					return;
				}
			}
			if (!BulletPenetration::HandleBulletPenetration(enter_surf, &enter_tr, dir, &result, penetration, hitsleft, bulletdamage))
				break;
		}
	}

	void TraceSubsystem::EndCustomTrace()
	{
		assert(m_pCustomTracePlayer != nullptr);
		assert(m_pBoneCache != nullptr);
		*m_iMostRecentBoneCounterPtr = m_iMostRecentBoneCounterBackup;
		memcpy(m_pBoneCache->Base(), m_BonecacheBackup.data(), m_pBoneCache->Count() * sizeof(matrix3x4a_t));
		m_pCustomTracePlayer = nullptr;
		m_pBoneCache = nullptr;
	}
	void TraceSubsystem::BeginCustomTrace(
		C_BasePlayer* player, 
		matrix3x4_t* matrix
	)
	{
		assert(m_pCustomTracePlayer == nullptr); //Mismatched Begin/End calls
		assert(m_pBoneCache == nullptr);

		static auto g_iModelBoneCounterPtr = (uintptr_t)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern("3B 05 ? ? ? ? 0F 84 ? ? ? ? 8B 35") + 0x2);
		m_pCustomTracePlayer = player;

		int g_iModelBoneCounter = **(int**)g_iModelBoneCounterPtr;
		auto baseanimating = player->GetBaseAnimating();
		m_pBoneCache = (Bonecache*)((uintptr_t)baseanimating + 0x2900);
		Bonearray* barrayptr = (Bonearray*)(m_pBoneCache->Base());
		m_BonecacheBackup = *(Bonearray*)(m_pBoneCache->Base());
		Bonearray newarray;
		for (auto i = 0; i < m_pBoneCache->Count(); i++)
		{
			newarray[i] = matrix3x4a_t(matrix[i]);
		}
		memcpy(m_pBoneCache->Base(), newarray.data(), m_pBoneCache->Count() * sizeof(matrix3x4a_t));
		m_iMostRecentBoneCounterPtr = (int*)((uintptr_t)player + 0x2680);
		m_iMostRecentBoneCounterBackup = *m_iMostRecentBoneCounterPtr;
		*m_iMostRecentBoneCounterPtr = g_iModelBoneCounter;

	}
}
