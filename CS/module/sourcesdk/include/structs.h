#pragma once

#include "csgosdk.h"
#include "ehandle.hpp"
#include <array>

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = Envy::Netvars::Instance()->Offset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = Envy::Netvars::Instance()->Offset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define OFFSET(type, name, offset)									\
	type& name##() const											\
	{																\
		return *(type*)((uintptr_t)this + offset);					\
	}
#define POFFSET(type, name, offset)									\
	type* name##() const											\
	{																\
		return (type*)((uintptr_t)this + offset);					\
	}



#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10	

namespace Envy
{
	namespace SourceEngine
	{
		enum CSWeaponType
		{
			WEAPONTYPE_KNIFE = 0,
			WEAPONTYPE_PISTOL,
			WEAPONTYPE_SUBMACHINEGUN,
			WEAPONTYPE_RIFLE,
			WEAPONTYPE_SHOTGUN,
			WEAPONTYPE_SNIPER_RIFLE,
			WEAPONTYPE_MACHINEGUN,
			WEAPONTYPE_C4,
			WEAPONTYPE_PLACEHOLDER,
			WEAPONTYPE_GRENADE,
			WEAPONTYPE_UNKNOWN
		};

		class C_BaseEntity;

		// Created with ReClass.NET by KN4CK3R
		class CHudTexture
		{
		public:
			char szShortName[64];    //0x0000
			char szTextureFile[64];  //0x0040
			bool bRenderUsingFont;   //0x0080
			bool bPrecached;         //0x0081
			int8_t cCharacterInFont; //0x0082
			uint8_t pad_0083[1];     //0x0083
			uint32_t hFont;          //0x0084
			int32_t iTextureId;      //0x0088
			float afTexCoords[4];    //0x008C
			uint8_t pad_009C[16];    //0x009C
		};

#pragma pack(push, 1)
		class CCSWeaponInfo
		{
		public:
			void*   vftbl;
			char*   consoleName;
			char    pad_0008[12];
			int	    iMaxClip1;
			int	    iMaxClip2;
			int	    iDefaultClip1;
			int	    iDefaultClip2;
			char    pad_0024[8];
			char*   szWorldModel;
			char*   szViewModel;
			char*   szDroppedModel;
			char    pad_0038[4];
			char*   N0000023E;
			char    pad_0040[56];
			char*   szEmptySound;
			char    pad_007C[4];
			char*   szBulletType;
			char    pad_0084[4];
			char*   szHudName;
			char*   szWeaponName;
			char    pad_0090[56];
			int     WeaponType;
			int	    iWeaponPrice;
			int	    iKillAward;
			char*   szAnimationPrefix;
			float   flCycleTime;
			float   flCycleTimeAlt;
			float   flTimeToIdle;
			float   flIdleInterval;
			bool    bFullAuto;
			char    pad_0x00E5[3];
			int	    iDamage;
			float   flArmorRatio;
			int	    iBullets;
			float   flPenetration;
			float   flFlinchVelocityModifierLarge;
			float   flFlinchVelocityModifierSmall;
			float   flRange;
			float   flRangeModifier;
			float   flThrowVelocity;
			char    pad_0x010C[12];
			bool    bHasSilencer;
			char    pad_0x0119[3];
			char*   pSilencerModel;
			int	    iCrosshairMinDistance;
			int	    iCrosshairDeltaDistance;
			float   flMaxPlayerSpeed;
			float   flMaxPlayerSpeedAlt;
			float   flSpread;
			float   flSpreadAlt;
			float   flInaccuracyCrouch;
			float   flInaccuracyCrouchAlt;
			float   flInaccuracyStand;
			float   flInaccuracyStandAlt;
			float   flInaccuracyJumpInitial;
			float   flInaccuracyJump;
			float   flInaccuracyJumpAlt;
			float   flInaccuracyLand;
			float   flInaccuracyLandAlt;
			float   flInaccuracyLadder;
			float   flInaccuracyLadderAlt;
			float   flInaccuracyFire;
			float   flInaccuracyFireAlt;
			float   flInaccuracyMove;
			float   flInaccuracyMoveAlt;
			float   flInaccuracyReload;
			int	    iRecoilSeed;
			float   flRecoilAngle;
			float   flRecoilAngleAlt;
			float   flRecoilAngleVariance;
			float   flRecoilAngleVarianceAlt;
			float   flRecoilMagnitude;
			float   flRecoilMagnitudeAlt;
			float   flRecoilMagnitudeVariance;
			float   flRecoilMagnitudeVarianceAlt;
			float   flRecoveryTimeCrouch;
			float   flRecoveryTimeStand;
			float   flRecoveryTimeCrouchFinal;
			float   flRecoveryTimeStandFinal;
			int	    iRecoveryTransitionStartBullet;
			int	    iRecoveryTransitionEndBullet;
			bool    bUnzoomAfterShot;
			bool    bHideViewModelZoomed;
			char    pad_0x01B5[2];
			char    iZoomLevels[3];
			int	    iZoomFOV[2];
			float   fZoomTime[3];
			char*   szWeaponClass;
			float   flAddonScale;
			char    pad_0x01DC[4];
			char*   szEjectBrassEffect;
			char*   szTracerEffect;
			int	    iTracerFrequency;
			int	    iTracerFrequencyAlt;
			char*   szMuzzleFlashEffect_1stPerson;
			char    pad_0x01F4[4];
			char*   szMuzzleFlashEffect_3rdPerson;
			char    pad_0x01FC[4];
			char*   szMuzzleSmokeEffect;
			float   flHeatPerShot;
			char*   szZoomInSound;
			char*   szZoomOutSound;
			float   flInaccuracyPitchShift;
			float   flInaccuracySoundThreshold;
			float   flBotAudibleRange;
			char    pad_0x0218[8];
			char*   pWrongTeamMsg;
			bool    bHasBurstMode;
			char    pad_0x0225[3];
			bool    bIsRevolver;
			bool    bCannotShootUnderwater;
		};
		static constexpr auto MAX_LAYER_RECORDS = 13;
		struct LayerRecord
		{
			int m_sequence;
			float m_cycle;
			float m_weight;
			int m_order;

			LayerRecord()
			{
				m_sequence = 0;
				m_cycle = 0;
				m_weight = 0;
				m_order = 0;
			}

			LayerRecord(const LayerRecord& src)
			{
				m_sequence = src.m_sequence;
				m_cycle = src.m_cycle;
				m_weight = src.m_weight;
				m_order = src.m_order;
			}
		};
		struct LagRecord
		{
			LagRecord()
			{
				m_fFlags = 0;
				m_vecOrigin.Init();
				m_vecAngles.Init();
				m_vecMinsPreScaled.Init();
				m_vecMaxsPreScaled.Init();
				m_flSimulationTime = -1;
				m_masterSequence = 0;
				m_flLowerBodyYawTarget = 0.f;
				m_masterCycle = 0;
				m_tickcount = 0;
			}

			LagRecord(const LagRecord& src)
			{
				m_fFlags = src.m_fFlags;
				m_vecOrigin = src.m_vecOrigin;
				m_vecAngles = src.m_vecAngles;
				m_vecMinsPreScaled = src.m_vecMinsPreScaled;
				m_vecMaxsPreScaled = src.m_vecMaxsPreScaled;
				m_flSimulationTime = src.m_flSimulationTime;
				m_flLowerBodyYawTarget = src.m_flLowerBodyYawTarget;
				for (int layerIndex = 0; layerIndex < MAX_LAYER_RECORDS; ++layerIndex)
				{
					m_layerRecords[layerIndex] = src.m_layerRecords[layerIndex];
				}
				m_masterSequence = src.m_masterSequence;
				m_masterCycle = src.m_masterCycle;
				m_tickcount = src.m_tickcount;
			}

			int						m_fFlags;
			Vector					m_vecOrigin;
			QAngle					m_vecAngles;
			Vector					m_vecMinsPreScaled;
			Vector					m_vecMaxsPreScaled;
			float					m_flSimulationTime;
			float					m_flLowerBodyYawTarget;
			LayerRecord				m_layerRecords[MAX_LAYER_RECORDS];
			int						m_masterSequence;
			float					m_masterCycle;
			int						m_tickcount;

		};
#pragma pack(pop)

		class CCollisionProperty
		{
		public:
			NETVAR(Vector, m_vecMins, "DT_BaseEntity", "m_vecMins");
			NETVAR(Vector, m_vecMaxs, "DT_BaseEntity", "m_vecMaxs");
		};

		class C_EconItemView
		{
		public:
			NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
			NETVAR(int32_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
			NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
			NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
			NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
			NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
			NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
		};

		class C_BaseEntity : public IClientEntity
		{
		public:
			NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
			NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
			NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
			NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
			NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
			NETVAR(int, m_CollisionGroup, "DT_BaseEntity", "m_CollisionGroup");
			NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
			OFFSET(int, m_TakeDamage, 0x27C);
			PNETVAR(CCollisionProperty, m_Collision, "DT_BaseEntity", "m_Collision");


			const matrix3x4_t& m_rgflCoordinateFrame()
			{
				static auto _m_rgflCoordinateFrame = Netvars::Instance()->Offset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
				return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
			}

			int& m_MoveType()
			{
				static auto _m_MoveType = 0x258;
				return *(int*)((uintptr_t)this + _m_MoveType);
			}

			int UnknownInt_Breakable()
			{
				return (*(int(__thiscall **)(C_BaseEntity*))(*(DWORD *)this + 0x1E8))(this);
			}

			void* GetBaseAnimating()
			{
				using GetBaseAnimatingFn = void*(__thiscall*)(C_BaseEntity* ent);
				return VMTManager::Instance()->GetVirtualFunction<GetBaseAnimatingFn>(this, 44)(this);
			}

			const char* GetClassname()
			{
				return ((const char*(__thiscall*)(C_BaseEntity*)) *(uintptr_t*)(*(uintptr_t*)this + 0x22C))(this);
			}

			static inline bool FClassnameIs(C_BaseEntity *pEntity, const char *szClassname)
			{
				if (pEntity == NULL)
					return false;

				return !strcmp(pEntity->GetClassname(), szClassname) ? true : false;
			}

			int PhysicsSolidMaskForEntity();

			bool IsPlayer();
			bool IsWeapon();
			bool IsPlantedC4();
			bool IsDefuseKit();
			bool ShouldCollide(int collisionGroup, int contentsMask);
		};

		class C_PlantedC4
		{
		public:
			NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
			NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
			NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
			NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
			NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
			NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
			NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
		};

		class C_BaseAttributableItem : public C_BaseEntity
		{
		public:
			NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
			NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
			NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
			NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
			NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
			NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
			NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

			C_EconItemView& m_Item()
			{
				// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
				// A real fix for this requires a rewrite of the netvar manager
				return *(C_EconItemView*)this;
			}
		};


		class C_BaseCombatWeapon : public C_BaseAttributableItem
		{
		public:
			NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
			NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
			NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
			NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
			NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");

			CCSWeaponInfo* GetCSWeaponData();
			bool HasBullets();
			bool CanFire();
			bool IsGrenade();
			bool IsKnife();
			bool IsReloading();
			bool IsRifle();
			bool IsPistol();
			bool IsSniper();
			float GetInaccuracy();
			float GetSpread();
			void UpdateAccuracyPenalty();

		};

		class C_BasePlayer : public C_BaseEntity
		{
			using AnimArray = std::array<AnimationLayer, 13>;
			using PoseParamArray = std::array<float, 24>;
		public:
			NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
			NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
			NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
			NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
			NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
			NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
			NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
			NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
			NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
			NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
			NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
			NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
			NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
			NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
			NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
			NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
			NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
			NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
			NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
			NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
			NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
			NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
			PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
			PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");

			PNETVAR(float, m_flPoseParamters, "DT_BaseAnimating", "m_flPoseParameter");

			OFFSET(float, m_flSpawnTime, 0xA290);
			POFFSET(AnimationLayer, m_pAnimLayers, 0x2970);
			OFFSET(AnimArray, m_AnimLayerArray, 0x2970);

			OFFSET(Vector, m_absOrigin, 0xA0);
			OFFSET(QAngle, m_absAngles, 0xC4);
			OFFSET(QAngle, m_angThirdpersonAngles, 0x31C8);

			CUserCmd*& m_pCurrentCommand();

			float GetPredictionTime() const;

			Vector        GetEyePos();
			player_info_t GetPlayerInfo();
			bool          IsAlive();
			bool          CarryingC4();

			bool IsArmored(int hitgroup);
			Vector        GetBonePos(int bone);
			Vector        GetHitboxPos(int hitbox_id);
			Vector		  TranslateHitboxPos(int hitbox_id, matrix3x4_t* m);

			AnimationLayer& GetAnimOverlay(int idx);

			float& GetPoseParameter(int idx);
			std::array<float, 24>& GetPoseParameters();


			void SetAbsAngles(const QAngle& angle);
			/*

			bool          GetHitboxPos(int hitbox, Vector &output);
			
			bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
			bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
			*/
		};

		class C_Breakable : public C_BaseEntity {
		public:
		};

		class C_BreakableSurface : public C_Breakable
		{
		public:
			NETVAR(bool, m_bIsBroken, "DT_BreakableSurface", "m_bIsBroken");
		};

		class C_BaseViewModel : public C_BaseEntity
		{
		public:
			NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
			NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
			NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
			NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
		};

		class C_LocalPlayer
		{
			friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
		public:
			C_LocalPlayer() : m_local(nullptr) {}

			operator bool() const { return *m_local != nullptr; }
			operator C_BasePlayer*() const { return *m_local; }

			C_BasePlayer* operator->() { return *m_local; }

			int& m_nChokedTicks() { return _m_nChokedTicks; }
		private:
			C_BasePlayer** m_local;
			int _m_nChokedTicks = 0;
		};
	}
	extern SourceEngine::C_LocalPlayer g_LocalPlayer;
}