#pragma once

#include "subsystem.h"
#include "csgosdk.h"
#include "esp.h"

#include <memory>
#include <d3d9.h>

namespace Envy
{
	class VisualSubsystem : public ICheatSubsystem
	{
	public:
		VisualSubsystem()
		{

		}
		virtual void Init() override;
		bool OnPaintTraverse(SourceEngine::vgui::VPANEL& panel, bool& forceRepaint, bool& allowForce);
		void OnPostScreenEffects();
		void SetNextTurnTime(float nextTurnTime) { m_flNextTurnTime = nextTurnTime; }
		void SetAnimTime(float lastTurnTime) { m_flAnimNextTime = lastTurnTime; }

		void SetRealYaw(float yaw) { m_flRealYaw = yaw; }
		void SetFakeYaw(float yaw) { m_flFakeYaw = yaw; }

		void OverrideBoneMatrix(SourceEngine::C_BasePlayer* player, SourceEngine::matrix3x4_t* m) { m_ESP->OverrideBoneMatrix(player, m); }

	private:
		std::unique_ptr<ESP> m_ESP;

		float m_flNextTurnTime = 0.f;
		float m_flAnimNextTime = 0.f;
		float m_flRealYaw = 0.f;
		float m_flFakeYaw = 0.f;

	};
	extern IDirect3DDevice9* g_D3DDevice;
}