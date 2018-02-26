#pragma once

#include "interfaces.h"
#include "peb.h"
#include "vmtmanager.h"
namespace Envy
{
	static constexpr const char* GameRulesSig = "8B 0D ? ? ? ? FF B3 ? ? ? ? FF 77 08";
	namespace SourceEngine
	{
		class _CGameRules_
		{
		public:
			bool ShouldCollide(int group0, int group1)
			{
				using ShouldCollideFn = bool(__thiscall*)(_CGameRules_*, int, int);
				auto fn = VMTManager::Instance()->GetVirtualFunction<ShouldCollideFn>(this, 28);
				return fn(this, group0, group1);
			}
		};
	}

	class CGameRules : public SourceInterface<SourceEngine::_CGameRules_>
	{
	public:
		CGameRules() {}

		virtual Interface* Init() override
		{
			m_data = **reinterpret_cast<Interface***>(
				Peb::Instance()->GetModule("client.dll").FindPattern(GameRulesSig) + 2
			);

			SourceInterface<SourceEngine::_CGameRules_>::Init();

			return m_data;
		}
	};
}