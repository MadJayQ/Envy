#include "animsys.h"
#include "csgosdk.h"
#include "structs.h"
#include "mathfuncs.h"

#include "visualsys.h"

using namespace Envy::SourceEngine;
namespace Envy
{
	namespace Rebuilt
	{
		//TODO(Jake): Rebuild this properly in ReClass
		class CStudioHdr
		{
			studiohdr_t* m_pStudioHdr;
		};
		struct BoneAccessor
		{

			const void *m_pAnimating;

			matrix3x4_t *m_pBones;

			int m_ReadableBones;		// Which bones can be read.
			int m_WritableBones;		// Which bones can be written.
		};
		BoneAccessor* GetBoneAccessor(IClientRenderable* renderable)
		{
			return reinterpret_cast<BoneAccessor*>((uintptr_t)renderable + 0x2690);
		}
		void BuildTransformations(
			C_BasePlayer* player, 
			CStudioHdr* hdr, 
			Vector* pos, 
			Quaternion* q, 
			const matrix3x4_t& transform, 
			int mask, 
			uint8_t* computed
		)
		{
			using BuildTransformationsFn = void(__thiscall*)(C_BasePlayer*, CStudioHdr*, Vector*, Quaternion*, matrix3x4_t const&, int, uint8_t*);
			ZeroMemory(computed, 0x100);
			auto fn = VMTManager::Instance()->GetVirtualFunction<BuildTransformationsFn>(player, 184);
			return fn(player, hdr, pos, q, transform, mask, computed);
		}
		void StandardBlendingRules(
			C_BasePlayer* player,
			CStudioHdr* hdr,
			Vector* pos,
			QuaternionAligned* q,
			float time,
			int mask
		)
		{
			using StandardBlendingRulesFn = void(__thiscall*)(C_BasePlayer*, CStudioHdr*, Vector*, QuaternionAligned*, float, int);
			auto fn = VMTManager::Instance()->GetVirtualFunction<StandardBlendingRulesFn>(player, 200);
			return fn(player, hdr, pos, q, time, mask);
		}
	}

	namespace AnimationState
	{
		void UpdateAnimationState(CCSGOPlayerAnimState* anim, QAngle angle)
		{
			static auto UpdateAnimState = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

			if (!UpdateAnimState)
			{
				return;
			}

			__asm
			{
				push 0
				mov ecx, anim
				movss xmm1, dword ptr[angle + 4]
				movss xmm2, dword ptr[angle]

				call UpdateAnimState
			}

		}
		void ResetAnimationState(CCSGOPlayerAnimState * anim)
		{
			using ResetAnimStateFn = void(__thiscall*)(CCSGOPlayerAnimState*);
			static auto ResetAnimState = (ResetAnimStateFn)(Peb::Instance()->GetModule("client.dll").FindPattern("56 6A 01 68 ? ? ? ? 8B F1"));

			if (!ResetAnimState)
				return;

			ResetAnimState(anim);
		}
		void CreateAnimationState(C_BasePlayer* player, CCSGOPlayerAnimState * state)
		{
			using CreateAnimStateFn = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);

			static auto CreateAnimState = (CreateAnimStateFn)(Peb::Instance()->GetModule("client.dll").FindPattern("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));

			if (!CreateAnimState)
				return;

			CreateAnimState(state, player);
		}
	}
	void AnimationSubsystem::Init()
	{
	}

	bool AnimationSubsystem::SetupBones(
		C_BasePlayer * player,
		QAngle angle, 
		Vector origin, 
		matrix3x4_t* out, 
		float time, 
		int mask
	)
	{
		auto hdr = *(Rebuilt::CStudioHdr**)((uintptr_t)player + 0x293C);
		if (hdr == NULL)
			return false;

		IClientRenderable* renderable = player->GetClientRenderable();
		auto accessor = Rebuilt::GetBoneAccessor(renderable);
		if (!accessor)
			return false;

		auto backup = (matrix3x4_t*)malloc(MAXSTUDIOBONES * sizeof(matrix3x4_t));
		memcpy(backup, accessor->m_pBones, MAXSTUDIOBONES * sizeof(matrix3x4_t));

		if (!backup)
			return false;

		auto abs_origin = *(Vector*)((uintptr_t)player + 0xA0);
		auto abs_angle = VMTManager::Instance()->GetVirtualFunction<QAngle&(__thiscall*)(void*)>(player, 11)(player);

		Vector pos[128] = { Vector(0.f, 0.f, 0.f) };
		QuaternionAligned q[128] = { QuaternionAligned{0.f, 0.f, 0.f, 0.f} };

		matrix3x4_t parentTransform;
		Math::AngleMatrix(angle, origin, parentTransform);

		Rebuilt::StandardBlendingRules(player, hdr, pos, q, time, mask);
		uint8_t computed[0x100] = { 0 };
		Rebuilt::BuildTransformations(player, hdr, pos, q, parentTransform, mask, computed);
		memcpy(out, accessor->m_pBones, MAXSTUDIOBONES * sizeof(matrix3x4_t));
		memcpy(accessor->m_pBones, backup, MAXSTUDIOBONES * sizeof(matrix3x4_t));
		*(Vector*)((uintptr_t)player + 0xA0) = abs_origin;
		*(QAngle*)((uintptr_t)player + 0xC4) = abs_angle;
		free(backup);

		return true;
	}
	static float v50 = 0.f;
	void AnimationSubsystem::SimulateServerAnimationCode(QAngle angle, float curtime)
	{

		auto clientstate = Interfaces::Instance()->GetInterface<CClientState>()->get();
		bool allocate = (m_pLocalPlayerState == nullptr);
		bool change = (!allocate) && (g_LocalPlayer->GetRefEHandle() != m_localHandle);
		bool reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);
		auto global = Interfaces::Instance()->GetInterface<CGlobalVarsBase>()->get();

		if (change)
		{
			free(m_pLocalPlayerState);
		}
		if (reset)
		{
			AnimationState::ResetAnimationState(m_pLocalPlayerState);
		}

		if (allocate || change)
		{
			auto state = (CCSGOPlayerAnimState*)malloc(sizeof(CCSGOPlayerAnimState));

			if (state != nullptr)
			{
				AnimationState::CreateAnimationState(g_LocalPlayer, state);
			}

			m_localHandle = g_LocalPlayer->GetRefEHandle();
			m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
			m_pLocalPlayerState = state;
		}

		std::array<AnimationLayer, 13> layerbackup;
		std::array<float, 24> poseparambackup;

		for (int i = 0; i < 24; i++)
		{
			if (i < 13)
			{
				layerbackup[i] = g_LocalPlayer->GetAnimOverlay(i);
			}
			poseparambackup[i] = g_LocalPlayer->GetPoseParameter(i);
		}

		auto origin = g_LocalPlayer->m_absOrigin();
		auto angles = g_LocalPlayer->m_absAngles();

		AnimationState::UpdateAnimationState(m_pLocalPlayerState, angle);

		g_LocalPlayer->m_absOrigin() = origin;
		g_LocalPlayer->m_absAngles() = angles;

		for (int i = 0; i < 24; i++) 
		{
			if (i < 13)
			{
				g_LocalPlayer->GetAnimOverlay(i) = layerbackup[i];
			}

			g_LocalPlayer->GetPoseParameter(i) = poseparambackup[i];
		}

		if (m_pLocalPlayerState->m_flSpeed > 0.1)
		{
			m_flNextTurnTime = curtime + 0.22f;
		}
		else
		{
			float v73 = g_LocalPlayer->m_flLowerBodyYawTarget();
			float delta = fabsf(Math::AngleDiff(v73, angle.yaw));
			m_flLowerBodyYawDelta = delta;
			if (delta > 35.f && curtime > m_flNextTurnTime)
			{
				m_flNextTurnTime = curtime + 1.1f;
			}
		}
	}
}

