#include "..\..\include\features\aimbot.h"

#include "mathfuncs.h"
#include "tracesys.h"
#include "animsys.h"
#include "lagcompsys.h"
#include "visualsys.h"
#include "inputsys.h"
#include "options.hpp"
#include "thread.h"

#include "subsystem/subsystem.h"

#include <mutex>

using namespace Envy::SourceEngine;
namespace Envy
{

	namespace Concurrency
	{
		std::mutex g_Mutex;
		std::mutex g_AutowallMutex;
	}

	QAngle CalculateAngle(Vector src, Vector dst)
	{
		QAngle angles;
		Vector delta = src - dst;

		Math::VectorAngles(delta, angles);

		Math::NormalizeAngles(angles);

		return angles;
	}
	Vector Aimbot::GetHitboxPosition(C_BasePlayer* player, int hitbox_num, matrix3x4_t* bones) 
	{
		auto studio_model = Interfaces::Instance()->GetInterface<IVModelInfoClient>()->GetStudiomodel(player->GetModel());
		if (studio_model)
		{
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_num);
			if (hitbox)
			{
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, bones[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, bones[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}

	void RenderMultiPoint(C_BasePlayer* player, int hitbox_num, matrix3x4_t* bones, std::vector<Vector>* points)
	{
		auto studio_model = Interfaces::Instance()->GetInterface<IVModelInfoClient>()->GetStudiomodel(player->GetModel());
		auto debug = Interfaces::Instance()->GetInterface<IVDebugOverlay>();
		if (studio_model)
		{
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_num);
			if (hitbox)
			{
				auto min = Vector{}, max = Vector{};
				if (hitbox->m_flRadius == -1.f) return;
				float radius_modifier = (hitbox->m_flRadius != -1.f) ? hitbox->m_flRadius : 0.f;

				auto size = (hitbox->bbmax - hitbox->bbmin).Normalized();
				auto radius = 0.95f * hitbox->m_flRadius;

				auto center = (hitbox->bbmax + hitbox->bbmin) * 0.5f;
				Math::VectorTransform(center, bones[hitbox->bone], points->at(0));
				if (!Options::Instance()->aimbot_multipoint()[hitbox_num])
				{
					return;
				}
				Vector min_vector = hitbox->bbmin - size * radius;
				Vector max_vector = hitbox->bbmax + size * radius;
				Vector dirx, diry, dirz;
				Vector a, b, c, d;
				Vector ta, tb, tc, td;
				Vector ba, bb, bc, bd;
				Vector ua, ub, uc, ud;
				Vector la, lb, lc, ld;
				QAngle angle;
				Math::VectorAngles(size, angle);

				Math::AngleVectors(QAngle(angle.pitch, angle.yaw + 90.f, angle.roll), dirx);
				Math::AngleVectors(QAngle(angle.pitch + 90.f, angle.yaw, angle.roll), diry);
				Math::AngleVectors(QAngle(angle.pitch, angle.yaw, angle.roll + 90.f), dirz);

				Math::VectorTransform(min_vector, bones[hitbox->bone], min);
				Math::VectorTransform(max_vector, bones[hitbox->bone], max);
				Math::VectorTransform(center + (dirx.Normalized() * radius), bones[hitbox->bone], points->at(1));
				Math::VectorTransform(center - (dirx.Normalized() * radius), bones[hitbox->bone], points->at(2));
				Math::VectorTransform(center + (diry.Normalized() * radius), bones[hitbox->bone], points->at(3));
				Math::VectorTransform(center - (diry.Normalized() * radius), bones[hitbox->bone], points->at(4));

				Math::VectorTransform(hitbox->bbmax + (dirx.Normalized() * radius), bones[hitbox->bone], points->at(5));
				Math::VectorTransform(hitbox->bbmax - (dirx.Normalized() * radius), bones[hitbox->bone], points->at(6));
				Math::VectorTransform(hitbox->bbmax + (diry.Normalized() * radius), bones[hitbox->bone], points->at(7));
				Math::VectorTransform(hitbox->bbmax - (diry.Normalized() * radius), bones[hitbox->bone], points->at(8));

				Math::VectorTransform(hitbox->bbmin + (dirx.Normalized() * radius), bones[hitbox->bone], points->at(9));
				Math::VectorTransform(hitbox->bbmin - (dirx.Normalized() * radius), bones[hitbox->bone], points->at(10));
				Math::VectorTransform(hitbox->bbmin + (diry.Normalized() * radius), bones[hitbox->bone], points->at(11));
				Math::VectorTransform(hitbox->bbmin - (diry.Normalized() * radius), bones[hitbox->bone], points->at(12));

				auto deltax = radius * sin(DEG2RAD(45));
				auto deltaz = radius * cos(DEG2RAD(45));

				Math::VectorTransform((hitbox->bbmax + dirz.Normalized() * deltax) + (dirx.Normalized() * deltaz), bones[hitbox->bone], points->at(13));
				Math::VectorTransform((hitbox->bbmax + dirz.Normalized() * deltax) - (dirx.Normalized() * deltaz), bones[hitbox->bone], points->at(14));
				Math::VectorTransform((hitbox->bbmax + dirz.Normalized() * deltax) + (diry.Normalized() * deltaz), bones[hitbox->bone], points->at(15));
				Math::VectorTransform((hitbox->bbmax + dirz.Normalized() * deltax) - (diry.Normalized() * deltaz), bones[hitbox->bone], points->at(16));

				Math::VectorTransform((hitbox->bbmin - dirz.Normalized() * deltax) + (dirx.Normalized() * deltaz), bones[hitbox->bone], points->at(17));
				Math::VectorTransform((hitbox->bbmin - dirz.Normalized() * deltax) - (dirx.Normalized() * deltaz), bones[hitbox->bone], points->at(18));
				Math::VectorTransform((hitbox->bbmin - dirz.Normalized() * deltax) + (diry.Normalized() * deltaz), bones[hitbox->bone], points->at(19));
				Math::VectorTransform((hitbox->bbmin - dirz.Normalized() * deltax) - (diry.Normalized() * deltaz), bones[hitbox->bone], points->at(20));
				
			}
		}
	}

	void Aimbot::CompensateRecoil(QAngle& angle)
	{
		auto punch = *(Vector*)((uintptr_t)(C_BaseEntity*)g_LocalPlayer + 0x301C);

		auto punchLen = punch.Length2D();

		if (punchLen < 0 || punchLen == 0 || punchLen > 120)
			return;

		Vector compensation = Vector(Options::Instance()->rcs_pitch(), Options::Instance()->rcs_yaw(), 0.f);

		if (compensation.x == 0.f)
			return;

		angle.pitch -= punch.x * compensation.x;
		angle.yaw -= punch.y * compensation.y;
	}


	void Aimbot::ApplySmooth(QAngle& viewangles, QAngle& aimangles, int divisor)
	{
		// TODO make legit, also add config for all values
		QAngle delta = aimangles - viewangles;

		Math::NormalizeAngles(delta);

		// basically an aimstep
		if (delta.Length() > .05f)
		{
			delta /= divisor;
		}

		aimangles = viewangles + delta;
	}
	CACHED HitscanArgs s_Hitscan;
	CACHED HitscanItr s_BestHitscan;
	HitscanArgs* Aimbot::Hitscan(SourceEngine::C_BasePlayer* player, const std::vector<int>& hitboxes, matrix3x4_t* bones)
	{
		auto tracesys = g_Subsystems->Get<TraceSubsystem>();
		auto start = g_LocalPlayer->GetEyePos();
		std::vector<HitscanItr> hitscan_sorted;
		int counter = 0;
		std::array<HitscanArgs, 4> delegatedWork;
		std::array<Thread<Aimbot, void, HitscanArgs*>*, 4> threads;
		for (int i = 0; i < delegatedWork.size(); i++)
		{
			delegatedWork[i] = 
			{
				std::vector<HitscanItr>(),
				nullptr,
				0.f,
				player
			};
		}
		for (auto hb : hitboxes)
		{
			std::vector<Vector> points(21);
			RenderMultiPoint(player, hb, bones, &points);
			HitscanItr itr;
			auto pos = points.at(0);
			itr.fov = fabsf(Math::GetFOV(start, pos, m_angViewAngles));
			itr.pos = pos;
			itr.hitbox = hb;
			itr.points = points;
			hitscan_sorted.push_back(itr);
			delegatedWork[counter].hitscans.push_back(itr);
			counter = (counter + 1) % 4;
		}

		bool sort = true;
		int idx = -1;
		float hp = (float)player->m_iHealth();

		for (int i = 0; i < 4; i++)
		{
			if (delegatedWork[i].hitscans.size() > 0)
			{
				threads[i] = new Thread<Aimbot, void, HitscanArgs*>(this, &Aimbot::PerformHitscan, &delegatedWork[i]);
				threads[i]->Start();
			} 
			else
			{
				threads[i] = nullptr;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (threads[i] != nullptr)
			{
				threads[i]->Join();
				if (idx == -1 && delegatedWork[i].bestDamage >= hp)
				{
					idx = i;
					sort = false;
					for (int j = i; j < 4; j++)
					{
						if (threads[j] == nullptr) continue;
						threads[j]->Interrupt();
					}
				}
			}
		}
		for (int i = 0; i < 4; i++)
		{
			if (threads[i] != nullptr) continue;
			delete threads[i];
		}
		if (sort)
		{
			idx = 0;
			std::sort(delegatedWork.begin(), delegatedWork.end(), [&](const HitscanArgs& a, const HitscanArgs& b)
			{
				return a.bestDamage > b.bestDamage;
			});
		}

		if (delegatedWork[idx].bestHitscan)
		{
			memcpy((void*)&s_Hitscan, (void*)&delegatedWork[idx], sizeof(HitscanArgs));
			s_Hitscan.bestHitscan = (HitscanItr*)malloc(sizeof(HitscanItr));
			memcpy((void*)s_Hitscan.bestHitscan, (void*)delegatedWork[idx].bestHitscan, sizeof(HitscanItr));
			return &s_Hitscan;
		}

		return nullptr;
	}
	void Aimbot::PerformHitscan(HitscanArgs* args)
	{
		auto player = args->player;
		auto tracesys = g_Subsystems->Get<TraceSubsystem>();
		auto wep = g_LocalPlayer->m_hActiveWeapon().Get();
		if (!wep)
		{
			return;
		}
		auto itr = args->hitscans.begin();
		bool exit = false;
		float dmg = 0.f;
		float hp = (float)player->m_iHealth();
		auto start = g_LocalPlayer->GetEyePos();
		do
		{
			for (auto& point : itr->points)
			{
				if (isinf(point.x) || isinf(point.y) || isinf(point.z)) continue;
				std::lock_guard<std::mutex> guard(Concurrency::g_Mutex);
				
				CGameTrace trace;
				Ray_t ray;
				CTraceFilter filter;
				ray.Init(start, point);
				filter.pSkip = g_LocalPlayer;
				filter.m_CollisionGroup = 0;
				(*Interfaces::Instance()->GetInterface<IEngineTrace>())->TraceRay(
					ray,
					MASK_SHOT,
					&filter,
					&trace
				);
				args->bestHitscan = &(*itr);
				args->bestDamage = 9999;
				args->bestHitscan->pos = point;
				exit = true;
				break;
				if (trace.hit_entity == player || trace.fraction >= 0.97f)
				{
					auto wpndata = wep->GetCSWeaponData();
					if (!wpndata)
						return;
					dmg = wpndata->iDamage;
					auto distance = (trace.endpos - trace.startpos).Length();
					auto traveled = distance * trace.fraction;
					dmg *= powf(wpndata->flRangeModifier, (traveled * 0.002f));
					dmg = BulletPenetration::ScaleDamage(player, dmg, wep->GetCSWeaponData()->flArmorRatio, trace.hitgroup);
					if (dmg >= (float)player->m_iHealth())
					{
						args->bestHitscan = &(*itr);
						args->bestDamage = dmg;
						args->bestHitscan->pos = point;
						exit = true;
						break;
					}
					if (dmg > args->bestDamage)
					{
						args->bestHitscan = &(*itr);
						args->bestDamage = dmg;
						args->bestHitscan->pos = point;
						break;
					}
				}
				else if (Options::Instance()->aimbot_allow_penetration())
				{
					std::lock_guard<std::mutex> guard(Concurrency::g_AutowallMutex);
					tracesys->GetPointDamage(g_LocalPlayer, start, point, dmg);
					if (dmg < 1.f)
					{
						continue;
					}
					if (dmg >= hp)
					{
						args->bestHitscan = &(*itr);
						args->bestDamage = dmg;
						args->bestHitscan->pos = point;
						exit = true;
						break;
					}
					if (dmg > args->bestDamage)
					{
						args->bestHitscan = &(*itr);
						args->bestDamage = dmg;
						args->bestHitscan->pos = point;
						break;
					}
				}
			}
			if (!exit) ++itr;
		} while (!exit && itr != args->hitscans.end());
	}
	bool Aimbot::Valid()
	{
		bool target = m_pTarget != nullptr;
		bool canfire = false;
		auto wpn = g_LocalPlayer->m_hActiveWeapon().Get();

		if (wpn != nullptr)
		{
			canfire = g_LocalPlayer->m_hActiveWeapon().Get()->CanFire();
		} 
		return (target && canfire);
	}
	void Aimbot::ProcessAutoPistol(SourceEngine::CUserCmd * cmd)
	{
		auto localwpn = g_LocalPlayer->m_hActiveWeapon().Get();
		if (!localwpn) return;
		auto wpndata = localwpn->GetCSWeaponData();
		if (!wpndata) return;
		if (!wpndata->bFullAuto)
		{
			if ((cmd->buttons & IN_ATTACK) != 0 && !localwpn->CanFire())
			{
				cmd->buttons &= ~IN_ATTACK;
			}
		}
	}
	bool Aimbot::ProcessAimbot(QAngle& aimangle, bool& sendpacket)
	{

		if (g_Subsystems->Get<InputSubsystem>()->GetKeyState(Options::Instance()->toggle_aimbot()) != KeyState::Down && Options::Instance()->aimbot_force_toggle())
		{
			m_pTarget = nullptr;
			m_angViewAngles = aimangle;
			m_angAimAngles = aimangle;
			return false;
		}

		m_pTarget = nullptr;
		m_angViewAngles = aimangle;
		m_angAimAngles = aimangle;
		QAngle view;
		(*Interfaces::Instance()->GetInterface<IVEngineClient>())->GetViewAngles(view);
		auto playerlist = Interfaces::Instance()->GetInterface<IClientEntityList>()->GetAllPlayers();
		auto globals = Interfaces::Instance()->GetInterface<CGlobalVarsBase>();
		auto tracesys = g_Subsystems->Get<TraceSubsystem>();
		std::vector<AimbotItr> targets;
		std::array<matrix3x4_t, 128> bonematrix;
		auto start = g_LocalPlayer->GetEyePos();
		for (auto& player : playerlist)
		{
			if (player->IsDormant() || !player->IsAlive()) continue;
			if (player == g_LocalPlayer) continue;
			if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;
			auto dst = player->GetHitboxPos(HITBOX_HEAD);
			float dmg;
			auto start = g_LocalPlayer->GetEyePos();
			AimbotItr itr;
			itr.distance = (dst - start).Length();
			itr.fov = fabsf(Math::GetFOV(start, dst, view));
			if (itr.fov > Options::Instance()->aimbot_max_fov()) continue;
			itr.threat = 0.f;
			itr.hp = player->m_iHealth();
			itr.player = player;
			targets.push_back(itr);
		}
		std::sort(targets.begin(), targets.end(), [&](const AimbotItr& a, const AimbotItr& b) 
		{
			return a.fov < b.fov;
		});

		for (auto& itr : targets)
		{

			auto player = itr.player;
#if 0
			Vector origin = Vector(0.f, 0.f, 0.f);
			QAngle angles = QAngle(0.f, 0.f, 0.f);
			Vector min, max;
			int correct_tickcount = 0;
			float simtime = 0.f;
			g_Subsystems->Get<LagCompensationSubsystem>()->RewindPlayer(player, origin, angles, min, max, simtime, correct_tickcount);
			std::array<matrix3x4_t, 128> newmatrix;
			if (!g_Subsystems->Get<AnimationSubsystem>()->SetupBones(player, angles, origin, newmatrix.data(), simtime, BONE_USED_BY_ANYTHING))
				continue;
			auto tracesys = g_Subsystems->Get<TraceSubsystem>();
			float dmg = 0.f;
			tracesys->BeginCustomTrace(player, newmatrix.data());
			auto head = GetHitboxPosition(player, HITBOX_HEAD, newmatrix.data());
			auto abs_origin_ptr = (Vector*)((uintptr_t)player + 0xA0);
			auto abs_angle_ptr = (QAngle*)((uintptr_t)player + 0xC4);
			auto abs_origin = *abs_origin_ptr;
			auto abs_angle = *abs_angle_ptr;
			*abs_origin_ptr = origin;
			*abs_angle_ptr = angles;
			*(int*)((uintptr_t)player + 0x0E4) |= EFL_DIRTY_SPATIAL_PARTITION;
			player->GetCollideable()->UpdatePartition();
			tracesys->GetPointDamage(g_LocalPlayer, start, head, dmg);
			tracesys->EndCustomTrace();
			*abs_origin_ptr = abs_origin;
			*abs_angle_ptr = abs_angle;
			*(int*)((uintptr_t)player + 0x0E4) |= EFL_DIRTY_SPATIAL_PARTITION;
			player->GetCollideable()->UpdatePartition();
			if (dmg < 1.f)
			{
				continue;
			}
			auto aimdir = (head - start).Normalized();
			QAngle aimangle;
			Math::VectorAngles(aimdir, aimangle);
			Math::NormalizeAngles(aimangle);
#endif
			Vector origin = Vector(0.f, 0.f, 0.f);
			QAngle angles = QAngle(0.f, 0.f, 0.f);
			Vector min, max;
			int correct_tickcount = 0;
			float simtime = 0.f;
			
			g_Subsystems->Get<LagCompensationSubsystem>()->RewindPlayer(player, origin, angles, min, max, simtime, correct_tickcount);
			if (!g_Subsystems->Get<AnimationSubsystem>()->SetupBones(player, angles, origin, bonematrix.data(), simtime, BONE_USED_BY_ANYTHING))
				continue;
			g_Subsystems->Get<VisualSubsystem>()->OverrideBoneMatrix(player, bonematrix.data());
			tracesys->BeginCustomTrace(player, bonematrix.data());
			auto abs_origin_ptr = (Vector*)((uintptr_t)player + 0xA0);
			auto abs_angle_ptr = (QAngle*)((uintptr_t)player + 0xC4);
			auto abs_origin = *abs_origin_ptr;
			auto abs_angle = VMTManager::Instance()->GetVirtualFunction<QAngle&(__thiscall*)(void*)>(player, 11)(player);
			*abs_origin_ptr = origin;
			*abs_angle_ptr = angles;
			*(int*)((uintptr_t)player + 0x0E4) |= EFL_DIRTY_SPATIAL_PARTITION;
			player->GetCollideable()->UpdatePartition();
			auto hitscan = Options::Instance()->hitscan_map[Options::Instance()->selected_hitscan_preset()].second;
			auto result = Hitscan(player, hitscan, bonematrix.data());
			auto ret = false;
			if (result && result->bestHitscan)
			{
				m_pTarget = player;
				Vector bestpos = result->bestHitscan->pos;
				Vector dir = (bestpos - start).Normalized();
				Math::VectorAngles(dir, m_angAimAngles);
				if (Options::Instance()->aimbot_enable_rcs())
				{
					CompensateRecoil(m_angAimAngles);
				}
				if (Options::Instance()->aimbot_enable_smooth())
				{
					ApplySmooth(m_angViewAngles, m_angAimAngles, Options::Instance()->smooth_factor());
				}
				ret = true;
			}
			tracesys->EndCustomTrace();
			*abs_origin_ptr = abs_origin;
			*abs_angle_ptr = abs_angle;
			*(int*)((uintptr_t)player + 0x0E4) |= EFL_DIRTY_SPATIAL_PARTITION;
			player->GetCollideable()->UpdatePartition();
			if (ret) return true;
		}
		return false;
	}
}
