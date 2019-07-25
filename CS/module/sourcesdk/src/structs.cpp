#include "..\include\structs.h"
#include "peb.h"
#include "vmtmanager.h"
#include "mathfuncs.h"

namespace Envy
{
	namespace SourceEngine
	{
		CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
		{
			static auto client = Peb::Instance()->GetModule("client_panorama.dll");
			static auto wpnDataFn = reinterpret_cast<CCSWeaponInfo*(__thiscall*)(void*)>(
				client.FindPattern("55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B")
			);
			return wpnDataFn(this);
		}
		bool C_BaseCombatWeapon::HasBullets()
		{
			return (m_iClip1() > 0) && !IsReloading();
		}
		bool C_BaseCombatWeapon::CanFire()
		{
			return (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>())->curtime > m_flNextPrimaryAttack();
		}
		bool C_BaseCombatWeapon::IsGrenade()
		{
			return (GetCSWeaponData()->WeaponType == WEAPONTYPE_GRENADE);
		}
		bool C_BaseCombatWeapon::IsKnife()
		{
			return (GetCSWeaponData()->WeaponType == WEAPONTYPE_KNIFE);
		}
		bool C_BaseCombatWeapon::IsRifle()
		{
			switch (GetCSWeaponData()->WeaponType)
			{
			case WEAPONTYPE_RIFLE:
			case WEAPONTYPE_SHOTGUN:
			case WEAPONTYPE_MACHINEGUN:
			case WEAPONTYPE_SUBMACHINEGUN:
				return true;
			default:
				return false;
			}
		}
		bool C_BaseCombatWeapon::IsPistol()
		{
			return (GetCSWeaponData()->WeaponType == WEAPONTYPE_PISTOL);
		}
		bool C_BaseCombatWeapon::IsSniper()
		{
			return (GetCSWeaponData()->WeaponType == WEAPONTYPE_SNIPER_RIFLE);
		}
		bool C_BaseCombatWeapon::IsReloading()
		{
			static auto client = Peb::Instance()->GetModule("client_panorama.dll");
			static auto inReload = *(uint32_t*)(client.FindPattern("C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
			return *(bool*)((uintptr_t)this + inReload);
		}
		float C_BaseCombatWeapon::GetInaccuracy()
		{
			//483
			return VMTManager::Instance()->GetVirtualFunction<float(__thiscall*)(void*)>(this, 483)(this);
		}
		float C_BaseCombatWeapon::GetSpread()
		{
			//484
			return VMTManager::Instance()->GetVirtualFunction<float(__thiscall*)(void*)>(this, 484)(this);

		}
		void C_BaseCombatWeapon::UpdateAccuracyPenalty()
		{
			//485
			VMTManager::Instance()->GetVirtualFunction<void(__thiscall*)(void*)>(this, 485)(this);
		}
		CUserCmd *& C_BasePlayer::m_pCurrentCommand()
		{
			// TODO: insert return statement here
			static auto command = *(uint32_t*)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern("89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
			return *(CUserCmd**)((uintptr_t)this + command);
		}
		float C_BasePlayer::GetPredictionTime() const
		{
			return (float)(m_nTickBase() * (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>())->interval_per_tick);
		}
		Vector C_BasePlayer::GetEyePos()
		{
			return m_vecOrigin() + m_vecViewOffset();
		}
		player_info_t C_BasePlayer::GetPlayerInfo()
		{
			player_info_t ret;
			(*Interfaces::Instance()->GetInterface<IVEngineClient>())->GetPlayerInfo(EntIndex(), &ret);
			return ret;
		}

		bool C_BasePlayer::IsArmored(int hitgroup)
		{
			bool result = false;
			auto armor = m_ArmorValue();
			if (armor > 0)
			{
				switch (hitgroup)
				{
				case HITGROUP_GENERIC:
				case HITGROUP_CHEST:
				case HITGROUP_STOMACH:
				case HITGROUP_LEFTARM:
				case HITGROUP_RIGHTARM:
					result = true;
					break;
				case HITGROUP_HEAD:
					result = m_bHasHelmet(); // DT_CSPlayer -> m_bHasHelmet
					break;
				}
			}
			return result;
		}

		Vector C_BasePlayer::GetBonePos(int bone)
		{
			matrix3x4_t boneMatrix[MAXSTUDIOBONES];

			if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) 
			{
				return boneMatrix[bone].at(3);
			}
			return Vector{};
		}

		Vector C_BasePlayer::TranslateHitboxPos(int hitbox_id, matrix3x4_t* m)
		{
			auto studio_model = Interfaces::Instance()->GetInterface<IVModelInfoClient>()->GetStudiomodel(GetModel());
			if (studio_model) 
			{
				auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
				if (hitbox) 
				{
					auto
						min = Vector{},
						max = Vector{};

					Math::VectorTransform(hitbox->bbmin, m[hitbox->bone], min);
					Math::VectorTransform(hitbox->bbmax, m[hitbox->bone], max);

					return (min + max) / 2.0f;
				}
			}
			return Vector{};
		}

		Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
		{
			matrix3x4_t boneMatrix[MAXSTUDIOBONES];

			if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
				auto modelInfoClient = Interfaces::Instance()->GetInterface<IVModelInfoClient>();
				auto model = GetModel();
				auto studio_model = modelInfoClient->GetStudiomodel(model);
				if (studio_model) {
					auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
					if (hitbox) {
						auto
							min = Vector{},
							max = Vector{};

						Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

						return (min + max) / 2.0f;
					}
				}
			}
			return Vector{};
		}

		AnimationLayer & C_BasePlayer::GetAnimOverlay(int idx)
		{
			auto layers = m_pAnimLayers();
			return layers[idx];
		}

		float& C_BasePlayer::GetPoseParameter(int idx)
		{
			assert(idx >= 0 && idx < 24);
			return GetPoseParameters()[idx];
		}

		std::array<float, 24>& C_BasePlayer::GetPoseParameters()
		{
			return *(std::array<float, 24>*)((uintptr_t)m_flPoseParamters());
		}

		void C_BasePlayer::SetAbsAngles(const QAngle & angle)
		{
			using SetAbsAngleFn = void(__thiscall*)(void*, const QAngle& angle);
			static SetAbsAngleFn _SetAbsAngle;
			if (!_SetAbsAngle)
				_SetAbsAngle = (SetAbsAngleFn)Peb::Instance()->GetModule("client_panorama.dll").FindPattern("83 EC 64 53 56 57 8B F1 E8");

			_SetAbsAngle(this, angle);
		}

		bool C_BasePlayer::IsAlive()
		{
			return (m_lifeState() == LIFE_ALIVE);
		}
		bool C_BasePlayer::CarryingC4()
		{
			static auto hasC4 = reinterpret_cast<bool(__thiscall*)(void*)>(
				Peb::Instance()->GetModule("client_panorama.dll").FindPattern("56 8B F1 85 F6 74 31")
			);
			return hasC4(this);
		}

		int C_BaseEntity::PhysicsSolidMaskForEntity()
		{
			using PhysicsSolidMaskForEntityFn = unsigned int(__thiscall*)(C_BaseEntity*);
			return VMTManager::Instance()->GetVirtualFunction<PhysicsSolidMaskForEntityFn>(this, 148)(this);
		}

		bool C_BaseEntity::IsPlayer()
		{
			//152
			return VMTManager::Instance()->GetVirtualFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 152)(this);
		}

		bool C_BaseEntity::IsWeapon()
		{
			//160
			return VMTManager::Instance()->GetVirtualFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 160)(this);
		}

		bool C_BaseEntity::IsPlantedC4()
		{
			return (GetClientClass()->m_ClassID == ClassId_CPlantedC4);
		}

		bool C_BaseEntity::IsDefuseKit()
		{
			return (GetClientClass()->m_ClassID == ClassId_CBaseAnimating);
		}

		bool C_BaseEntity::ShouldCollide(int collisionGroup, int contentsMask)
		{
			return VMTManager::Instance()->GetVirtualFunction<bool(__thiscall*)(C_BaseEntity*, int, int)>(this, 170)(this, collisionGroup, contentsMask);
		}

	}
}