#pragma once
#include "interfaces.h"
#include "peb.h"
namespace Envy
{
	namespace SourceEngine
	{
		class IClientEntity;

		class _IMoveHelper_
		{
		public:
			virtual	void _vpad() = 0;
			virtual void SetHost(IClientEntity* host) = 0;
		};
	}

	class IMoveHelper : public SourceInterface<SourceEngine::_IMoveHelper_>
	{
	public:
		IMoveHelper() {}

		virtual Interface* Init() override
		{
			m_data = **reinterpret_cast<Interface***>(
				Peb::Instance()->GetModule("client.dll").FindPattern("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01") + 2
			);
			SourceInterface<SourceEngine::_IMoveHelper_>::Init();
			return m_data;
		}
	};
}