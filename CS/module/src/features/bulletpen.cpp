#include "tracesys.h"


#include "csgosdk.h"
#include "structs.h"

using namespace Envy::SourceEngine;

/*
	Reversing notes
	bool __thiscall sub_101A47F0(int this) //trace
	{
		return *(float *)(this + 44) < 1.0 || *(_BYTE *)(this + 54) || *(_BYTE *)(this + 55);
		fraction < 1.0 || trace->allsolid || trace->startsolid
	}

*/
namespace Envy
{
	namespace BulletPenetration
	{

		void CopyTrace(int thisptr, int a1)
		{
			*(uintptr_t *)thisptr = *(uintptr_t *)a1;
			*(uintptr_t *)(thisptr + 4) = *(uintptr_t *)(a1 + 4);
			*(uintptr_t *)(thisptr + 8) = *(uintptr_t *)(a1 + 8);
			*(uintptr_t *)(thisptr + 12) = *(uintptr_t *)(a1 + 12);
			*(uintptr_t *)(thisptr + 16) = *(uintptr_t *)(a1 + 16);
			*(uintptr_t *)(thisptr + 20) = *(uintptr_t *)(a1 + 20);
			*(uintptr_t *)(thisptr + 24) = *(uintptr_t *)(a1 + 24);
			*(uintptr_t *)(thisptr + 28) = *(uintptr_t *)(a1 + 28);
			*(uintptr_t *)(thisptr + 32) = *(uintptr_t *)(a1 + 32);
			*(uintptr_t *)(thisptr + 36) = *(uintptr_t *)(a1 + 36);
			*(BYTE *)(thisptr + 40) = *(BYTE *)(a1 + 40);
			*(BYTE *)(thisptr + 41) = *(BYTE *)(a1 + 41);
			*(BYTE *)(thisptr + 42) = *(BYTE *)(a1 + 42);
			*(BYTE *)(thisptr + 43) = *(BYTE *)(a1 + 43);
			*(uintptr_t *)(thisptr + 44) = *(uintptr_t *)(a1 + 44);
			*(uintptr_t *)(thisptr + 48) = *(uintptr_t *)(a1 + 48);
			*(WORD *)(thisptr + 52) = *(WORD *)(a1 + 52);
			*(BYTE *)(thisptr + 54) = *(BYTE *)(a1 + 54);
			*(BYTE *)(thisptr + 55) = *(BYTE *)(a1 + 55);
			*(uintptr_t *)(thisptr + 56) = *(uintptr_t *)(a1 + 56);
			*(uintptr_t *)(thisptr + 60) = *(uintptr_t *)(a1 + 60);
			*(uintptr_t *)(thisptr + 64) = *(uintptr_t *)(a1 + 64);
			*(uintptr_t *)(thisptr + 68) = *(uintptr_t *)(a1 + 68);
			*(WORD *)(thisptr + 72) = *(WORD *)(a1 + 72);
			*(WORD *)(thisptr + 74) = *(WORD *)(a1 + 74);
			*(uintptr_t *)(thisptr + 76) = *(uintptr_t *)(a1 + 76);
			*(uintptr_t *)(thisptr + 80) = *(uintptr_t *)(a1 + 80);
		}

		bool IsBreakableEntity(C_BaseEntity* entity)
		{

			static auto IsBreakableEntityCall = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("6A 18 68 ? ? ? ? E8 ? ? ? ? 8B 7D 08");
			static auto IsBreakableEntityCall1Args = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 56 E8 ? ? ? ? 83 C4 14 85 C0 74 12 8B 10");
			static auto IsBreakableEntityCall2Args = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 56 E8 ? ? ? ? 8B C8 83 C4 14 85 C9 74 17");
			if (!entity)
				return false;
			//Fix this...
			auto health = *(int*)((uintptr_t)entity + 0xFC);

			if ((health >= 0 || entity->UnknownInt_Breakable() <= 0))//(v2 = pEntity->GetUnknownInt(), result = 1, v2 <= 0)))
			{
				if (entity->m_TakeDamage() != 2)
				{
					char *NetworkName = entity->GetClientClass()->m_pNetworkName;
					//if (NetworkName[1] == 'F' || NetworkName[4] == 'c' || NetworkName[5] == 'B' || NetworkName[9] == 'h')
					if (*(unsigned*)NetworkName != 0x65724243)
						return false;
				}

				int collisiongroup = entity->m_CollisionGroup();

				if (collisiongroup != 17 && collisiongroup != 6 && collisiongroup != 0)
				{
					return false;
				}

				if (health > 200)
					return false;
				uintptr_t pPhysicsInterface = ((uintptr_t(__cdecl *)(C_BaseEntity*, uintptr_t, uintptr_t, uintptr_t, uintptr_t))IsBreakableEntityCall)(entity, 0, *(uintptr_t*)(IsBreakableEntityCall1Args + 8), *(uintptr_t*)(IsBreakableEntityCall1Args + 3), 0);
				if (pPhysicsInterface)
				{
					if (((bool(__thiscall*)(uintptr_t)) *(uintptr_t*)(*(uintptr_t*)pPhysicsInterface))(pPhysicsInterface) != 1)
						return false;
				}
				else
				{
					const char *szClassname = entity->GetClassname();
					if (szClassname[0] == 'f' && szClassname[5] == 'b' && szClassname[13] == 'e')
					{
						if (szClassname[15] == 's')
						{
							// don't try to break it if it has already been broken
							if (static_cast<C_BreakableSurface*>(entity)->m_bIsBroken())
								return false;
						}
					}
					else if (entity->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
					{
						// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
						return false;
					}
				}
				uintptr_t pBreakableInterface = ((uintptr_t(__cdecl *)(C_BaseEntity*, uintptr_t, uintptr_t, uintptr_t, uintptr_t))IsBreakableEntityCall)(entity, 0, *(uintptr_t*)(IsBreakableEntityCall2Args + 8), *(uintptr_t*)(IsBreakableEntityCall2Args + 3), 0);
				if (pBreakableInterface)
				{
					// Bullets don't damage it - ignore
					float DamageModBullet = ((float(__thiscall*)(uintptr_t)) *(uintptr_t*)(*(uintptr_t*)pPhysicsInterface + 0xC))(pBreakableInterface);
					if (DamageModBullet <= 0.0f)
						return false;
				}
			}
			return true;
		}

		bool DidHit(trace_t* trace)
		{
			return (trace->fraction < 1.0 || trace->allsolid || trace->startsolid);
		}
		bool DidHitNonWorldEntity(trace_t* trace)
		{
			return (trace->hit_entity != Interfaces::Instance()->GetInterface<IClientEntityList>()->GetEntityByIndex(0));
		}
		bool TraceToExit(
			Vector& end,
			Vector& start,
			Vector& dir,
			trace_t* enter,
			trace_t* exit
		)
		{
			int firstcontents = 0;
			int pointcontents;
			float distance = 0.f;
			auto trace = Interfaces::Instance()->GetInterface<IEngineTrace>();
			auto tracesys = g_Subsystems->Get<TraceSubsystem>();
			while (distance <= 90.f)
			{
				distance += 4.f;
				end = start + dir * distance;
				if (!firstcontents)
					firstcontents = (*trace)->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);
				int pointcontents = (*trace)->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);
				if (pointcontents & MASK_SHOT_HULL && (!(pointcontents & CONTENTS_HITBOX)))
					continue;
				/*
				if (pointcontents == firstcontents)
					continue;
				*/
				auto new_end = (end - (dir * 4));

				Ray_t ray;
				ray.Init(end, new_end);
				(*trace)->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, 0, exit);

				/*
				TODO(Jake): Reverse this later, might come in handy for autowall debugging
				if ( ((int (__thiscall *)(void ***))off_10AC5508[13])(&off_10AC5508) )
					sub_10325780(0x4600400B, 0, exitTrace_1, 0xC2700000);
				*/
				if (exit->startsolid && exit->surface.flags & SURF_HITBOX)
				{
					//sub_1013E240(0x600400B, *(_uintptr_t *)(exit + 76), 0, exit);
					Ray_t ray2;
					ray2.Init(end, start);
					CTraceFilter filter;
					filter.m_CollisionGroup = 0;
					filter.pSkip = exit->hit_entity;
					(*trace)->TraceRay(ray2, MASK_SHOT_HULL, &filter, exit);
					//if ((unsigned __int8)sub_101A47F0(v15) && !*(_BYTE *)(v15 + 55)) SomeFunc && !exit->startsolid SEE ABOVE
					if (DidHit(exit) && !exit->startsolid)
					{
						end = exit->endpos;
						return true;
					}
					continue;
				}
				if (!DidHit(exit) || exit->startsolid)
				{
					if (enter->hit_entity != NULL)
					{
						if (DidHitNonWorldEntity(enter))
						{
							if (IsBreakableEntity((C_BaseEntity*)enter->hit_entity))
							{
								//sub_10166B00(v15, v16); v15 = exit, v16 = exit -> exit = enter
								//CopyTrace((int)exit, (int)enter);
								memcpy(exit, enter, sizeof(CGameTrace));
								exit->endpos = start + dir;
								return true;
							}
						}
					}
					continue;
				}
				if ((exit->surface.flags >> 7) & SURF_LIGHT)
				{
					if (IsBreakableEntity((C_BaseEntity*)exit->hit_entity))
					{
						if (IsBreakableEntity((C_BaseEntity*)enter->hit_entity))
						{
							end = exit->endpos;
							return true;
						}
					}
					if (!((enter->surface.flags >> 7) & SURF_LIGHT))
						continue;
				}
				if (exit->plane.normal.Dot(dir) <= 1.0f)
				{
					float fraction = exit->fraction * 4.f;
					end = end - (dir * fraction);
					return true;
				}
			}
			return false;
		}
		bool HandleBulletPenetration(
			surfacedata_t* enter_surface,
			trace_t* enter_trace,
			Vector direction,
			Vector* origin,
			float penetration, 
			int& penetration_count,
			float& bulletdamage
		)
		{
			auto trace = Interfaces::Instance()->GetInterface<IEngineTrace>();
			auto physics = Interfaces::Instance()->GetInterface<IPhysicsSurfaceProps>();
			char isLightSurf = (enter_trace->surface.flags >> 7) & SURF_LIGHT;
			char isSolidSurf = (enter_trace->contents >> 3) & CONTENTS_SOLID;
			Vector end;
			trace_t exit;
			auto enter_material = enter_surface->game.material;//*(int*)((uintptr_t)enter_surface + 96);
			float enter_surf_penetration_mod = enter_surface->game.flPenetrationModifier;//*(float*)((uintptr_t)enter_surface + 88);

			if (!penetration_count && !isSolidSurf && !isLightSurf && enter_material != 89)
			{
				if (enter_material != 71)
					return false;
			}
			if (penetration_count <= 0)
				return false;

			if (!TraceToExit(end, enter_trace->endpos, direction, enter_trace, &exit))
			{
				auto pointcontents = (*trace)->GetPointContents(end, MASK_SHOT_HULL, 0);
				if (!(pointcontents & MASK_SHOT_HULL))
					return false;
			}

			auto exit_surface = (*physics)->GetSurfaceData(exit.surface.surfaceProps);
			auto damage_mod = 0.16f;
			float exit_surf_penetration_mod = exit_surface->game.flPenetrationModifier;//*(float*)((uintptr_t)exit_surface + 88);
			auto exit_material = exit_surface->game.material;//*(int*)((uintptr_t)exit_surface + 96);
			auto avg_pen_mod = (enter_surf_penetration_mod + exit_surf_penetration_mod) / 2;
			if (enter_material == 71 || enter_material == 89)
			{
				damage_mod = 0.05f;
				avg_pen_mod = 3.f;
			}
			else if (isLightSurf || isSolidSurf)
			{
				damage_mod = 0.16f;
				avg_pen_mod = 1.f;
			}
			if (enter_material == exit_material)
			{
				if (exit_material == 87 || exit_material == 85)
				{
					avg_pen_mod = 3.f;
				}
				if (exit_material == 76)
					avg_pen_mod = 2.f;
			}
			/*
			*
			auto modifier = fmaxf(0.f, 1.f / avg_pen_mod);
			auto penetration_length = (exit.endpos - enter_trace->endpos).Length();
			auto taken_damage = ((modifier * 3.f) * fmaxf(0.f, (3.f / penetration) * 1.25f) + (bulletdamage * damage_mod)) + (((penetration_length * penetration_length) * modifier) / 24.f);

			bulletdamage -= fmaxf(0.f, taken_damage);

			if (bulletdamage < 1.f)
				return false;

			*origin = exit.endpos;

			--penetration_count;

			return true;
			*/

			float thickness = (exit.endpos - enter_trace->endpos).Length();
			float invs_pen_mod = fmaxf(0.f, 1.f / avg_pen_mod);
			thickness = (std::powf(thickness, 2.f) * invs_pen_mod) / 24;
			//lost_damage = invs_pen_mod * 3.0 * fmax(0.0, (float)(3.0 / wep_pen) * 1.25) + (float)(final_damage_mod * *damage);
			float pen_factor = invs_pen_mod * 3.0 * fmax(0.f, (3.0 / penetration) * 1.25) + (damage_mod * bulletdamage);
			float lost_damage = std::fmaxf(0.f, pen_factor + thickness);
			if (lost_damage > bulletdamage)
				return false;

			if (lost_damage >= 0.f)
				bulletdamage -= lost_damage;

			if (bulletdamage < 1.f)
				return false;

			*origin = exit.endpos;
			penetration_count--;
			return true;

#if 0
			Interfaces::Instance()->GetInterface<IVDebugOverlay>()->LogTrace(&enter);
			Interfaces::Instance()->GetInterface<IVDebugOverlay>()->LogTrace(&exit);

			float damage = bulletdamage;
			surfacedata_t* exit_surface = (*physics)->GetSurfaceData(exit.surface.surfaceProps);
			auto exit_material = *(int*)((uintptr_t)exit_surface + 96);
			float penetration_mod = 0.f;
			float combined_penetration_modifier = 0.0f;
			float damage_modifier = 0.f;

			float enter_surf_penetration_mod = enter_pen_mod;
			float exit_surf_penetration_mod = *(float*)((uintptr_t)exit_surface + 88);

			/*
				lightsurf & solidsurf checks
			*/
			if (solid || lightsurf)
			{
				combined_penetration_modifier = 3.0f;
				damage_modifier = 0.05;
				if (enter_material != 89 && enter_material != 71)
				{
					damage_modifier = 0.16;
					combined_penetration_modifier = 1.0f;
				}
			}
			else
			{
				if (enter_material == 70)
				{
#pragma region FF_CHECKS
					/*
					if ((_UNKNOWN *)qword_5ABB6C8 == &ff_damage_reduction_bullets)
					{
						LOuintptr_t(v24) = (unsigned __int128)_mm_cvtsi32_si128(uintptr_t_5ABB6E4 ^ (unsigned int)&ff_damage_reduction_bullets);
					}
					else
					{
						(*(void(**)(void))(*(_QWORD *)qword_5ABB6C8 + 120LL))();
						v40 = v69;
					}
					*/
#pragma endregion
					//combined_penetration_modifier = (float)(v65 + *(float*)(v35 + 76)) * 0.5;
					combined_penetration_modifier = (exit_surf_penetration_mod + enter_surf_penetration_mod) * 0.5;
					damage_modifier = 0.16;
				}
				else
				{
					combined_penetration_modifier = 3.0;
					damage_modifier = 0.05;
					if (enter_material != 71 && enter_material != 89)
					{
						combined_penetration_modifier = (exit_surf_penetration_mod + enter_surf_penetration_mod) * 0.5;
						damage_modifier = 0.16;
					}
				}
			}

			if (enter_material == exit_material)
			{
				if ((exit_material & 0xFFFD) == 85)
				{
					combined_penetration_modifier = 3.0;
				}
				else if (exit_material == 76)
				{
					combined_penetration_modifier = 2.0;
				}
			}
			/*
				Thickness checks
			*/
			float thickness = (exit.endpos - enter.endpos).Length();
			float invs_pen_mod = fmaxf(0.f, 1.f / combined_penetration_modifier);
			thickness = (std::powf(thickness, 2.f) * invs_pen_mod) / 24;
			//lost_damage = invs_pen_mod * 3.0 * fmax(0.0, (float)(3.0 / wep_pen) * 1.25) + (float)(final_damage_mod * *damage);
			float pen_factor = invs_pen_mod * 3.0 * fmax(0.f, (3.0 / weapon_penetration) * 1.25) + (damage_modifier * bulletdamage);
			float lost_damage = std::fmaxf(0.f, pen_factor + thickness);
			if (lost_damage > bulletdamage)
				return false;

			if (lost_damage >= 0.f)
				bulletdamage -= lost_damage;

			if (bulletdamage < 1.f)
				return false;

			result = exit.endpos;
			hitsleft--;

			return true;
#endif
		}
		float ScaleDamage(SourceEngine::C_BasePlayer * player, float dmg, float wpnratio, int hitgroup)
		{
			auto heavy = player->m_bHasHeavyArmor();
			auto armor = player->m_ArmorValue();

			switch (hitgroup)
			{
			case HITGROUP_HEAD:
				dmg *= 4.f;
				dmg *= (heavy) ? 0.5f : 1.f;
				break;
			case HITGROUP_STOMACH:
				dmg *= 1.25f;
				break;
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				dmg *= 0.75f;
				break;
			}

			if (player->IsArmored(hitgroup))
			{
				float unkfloat = 1.f;
				float bonusratio = 0.5f;
				float armorratio = wpnratio * 0.5f;
				if (heavy)
				{
					bonusratio = 0.33f;
					armorratio = (wpnratio * 0.5f) * 0.5f;
					unkfloat = 0.33f;
				}

				auto newdmg = dmg * armorratio;
				if (heavy)
					newdmg *= 0.85f;

				if (((dmg - (dmg * armorratio)) * (unkfloat * bonusratio)) > armor)
					newdmg = dmg - (armor / bonusratio);

				dmg = newdmg;
			}
			return dmg;
		}
	}
}