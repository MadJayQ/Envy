#include "csgosdk.h"
#include "structs.h"
#include "ienginetrace.hpp"


using namespace Envy::SourceEngine;

static constexpr const char* StandardFilterRulesCallSig = "8B 89 ? ? ? ? 83 F9 FF 74 16 0F B7 C1 C1 E0 04 05 ? ? ? ? C1 E9 10 39 48 04 75 03 8B 00 C3 33 C0 C3";
static constexpr const char* StandardFilterRulesSig = "E8 ? ? ? ? 85 C0 74 0B 8D 48 04";
namespace Envy
{
	namespace SourceEngine
	{
		bool StandardFilterRules(IHandleEntity * pHandleEntity, int fContentsMask)
		{

			static auto StandardFilterRulesCall = (uintptr_t)Peb::Instance()->GetModule("client_panorama.dll").FindPattern(StandardFilterRulesCallSig);
			static auto StandardFilterRulesMemory = *(uintptr_t*)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern(StandardFilterRulesCallSig) + 0x15);

			C_BaseEntity* pCollide = (C_BaseEntity*)pHandleEntity; //(*(CBaseEntity*(**)(IHandleEntity*))(*(DWORD *)pHandleEntity + 0x1C))(pHandleEntity);

																 // Static prop case...
			if (!pCollide)
				return true;

			SolidType_t solid = pCollide->GetCollideable()->GetSolid();

			const model_t *pModel = pCollide->GetModel();

			auto model_info_client = Interfaces::Instance()->GetInterface<IVModelInfoClient>()->get();

			if ((model_info_client->GetModelType(pModel) != mod_brush) || (solid != SOLID_BSP && solid != SOLID_VPHYSICS))
			{
				if ((fContentsMask & CONTENTS_MONSTER) == 0)
					return false;
			}
			// This code is used to cull out tests against see-thru entities
			if (!(fContentsMask & CONTENTS_WINDOW))
			{
				//if (pCollide->IsTransparent())
				//return false;

				//easy to read format
				//register DWORD EAX = *(DWORD*)((DWORD)pCollide + 4);
				//register DWORD ECX = ((DWORD)pCollide + 4);
				//unsigned short v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(EAX + 0x1C))(ECX);
				unsigned short v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(*(DWORD*)((DWORD)pCollide + 4) + 0x1C))(((DWORD)pCollide + 4));

				if (v10 == (unsigned short)(-1))
				{
					DWORD v9 = ((int(__thiscall*)(C_BaseEntity*))StandardFilterRulesCall)(pCollide);
					if (v9)
					{
						//easy to read format
						//ECX = *(DWORD*)(v9 + 4);
						//EAX = *(DWORD*)ECX;
						//v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(EAX + 0x1C))(ECX);
						DWORD ECX = *(DWORD*)(v9 + 4);
						v10 = *((unsigned short*(__thiscall *)(DWORD)) *(DWORD*)(*(DWORD*)ECX + 0x1C))(ECX);
					}
				}
				//easy to read format
				//EAX = *(DWORD*)AdrOf_StandardFilterRulesMemoryOne;
				//BOOL transparent = ((bool(__thiscall *)(DWORD, DWORD)) *(DWORD*)(EAX + 0x80)) (AdrOf_StandardFilterRulesMemoryOne, (DWORD)v10);
				BOOLEAN transparent = ((BOOLEAN(__thiscall *)(DWORD, DWORD)) *(DWORD*)(*(DWORD*)StandardFilterRulesMemory + 0x80)) (StandardFilterRulesMemory, (DWORD)v10);
				if (transparent)
					return false;
			}

			// FIXME: this is to skip BSP models that are entities that can be 
			// potentially moved/deleted, similar to a monster but doors don't seem to 
			// be flagged as monsters
			// FIXME: the FL_WORLDBRUSH looked promising, but it needs to be set on 
			// everything that's actually a worldbrush and it currently isn't
			if (!(fContentsMask & CONTENTS_MOVEABLE) && (pCollide->m_MoveType() == MOVETYPE_PUSH))// !(touch->flags & FL_WORLDBRUSH) )
				return false;

			return true;
		}
		bool PassServerEntityFilter(const IHandleEntity * pTouch, const IHandleEntity * pPass)
		{
			if (!pPass)
				return true;

			if (pTouch == pPass)
				return false;

			C_BaseEntity *pEntTouch = (C_BaseEntity*)pTouch;//EntityFromEntityHandle(pTouch);ddddd
			C_BaseEntity *pEntPass = (C_BaseEntity*)pPass;//EntityFromEntityHandle(pPass);
			if (!pEntTouch || !pEntPass)
				return true;


			// don't clip against own missiles
			if (pEntTouch->m_hOwnerEntity().Get() == pEntPass)
				return false;

			// don't clip against owner
			if (pEntPass->m_hOwnerEntity().Get() == pEntTouch)
				return false;


			return true;
		}
		BOOLEAN GameRulesShouldCollide(int collisionGroup0, int collisionGroup1)
		{

			static uintptr_t g_pGameRules = *(uintptr_t*)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern("8B 0D ? ? ? ? FF B3 ? ? ? ? FF 77 08") + 0x2);
#if 1
			__asm {
				push collisionGroup1
				push collisionGroup0
				mov ecx, g_pGameRules
				mov ecx, [ecx]
				mov eax, [ecx]
				mov eax, [eax + 0x70]
				call eax
			}
#endif

			//return ((BOOLEAN(__stdcall*)(int, int))(ReadInt(ReadInt(ReadInt(g_pGameRules)) + GameRulesShouldCollideOffset)))(collisionGroup0, collisionGroup1);
		}
		bool BaseShouldHitEntity(IHandleEntity * pSkip, IHandleEntity * pHandleEntity, int m_collisionGroup, int contentsMask)
		{
			if (!StandardFilterRules(pHandleEntity, contentsMask))
				return false;
			if (pSkip)
			{
				if (!PassServerEntityFilter(pHandleEntity, pSkip))
				{
					return false;
				}
			}

			C_BaseEntity* ent = (C_BaseEntity*)(pHandleEntity);
			if (!ent)
				return false;
			if (!ent->ShouldCollide(m_collisionGroup, contentsMask))
				return false;
			if (!GameRulesShouldCollide(m_collisionGroup, ent->m_CollisionGroup()))
				return false;

			return true;
		}
	}
}