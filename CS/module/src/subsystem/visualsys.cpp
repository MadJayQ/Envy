#include "..\..\include\subsystem\visualsys.h"
#include "interfaces.h"
#include "structs.h"
#include <mathfuncs.h>

using namespace Envy::SourceEngine;
namespace Envy
{
	void VisualSubsystem::Init()
	{
		m_ESP = std::make_unique<ESP>();
	}
	/*
		Return true to call the original function
	*/
	bool VisualSubsystem::OnPaintTraverse(
		SourceEngine::vgui::VPANEL& panel, 
		bool& forceRepaint, 
		bool& allowForce
	)
	{
		auto ipanel = Interfaces::Instance()->GetInterface<IPanel>();
		auto entitylist = Interfaces::Instance()->GetInterface<IClientEntityList>();
		auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>();
		static auto panelId = vgui::VPANEL{ 0 };
		static auto topPanel = vgui::VPANEL{ 0 };
		if (!panelId || !topPanel) 
		{
			const auto panelName = (*ipanel)->GetName(panel);
			if (!strcmp(panelName, "MatSystemTopPanel")) {
				topPanel = panel;
			}
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (topPanel == panel)
		{
			if (Options::Instance()->esp_enabled() && (*engine)->IsInGame() && !(*engine)->IsTakingScreenshot())
			{
				for (auto i = 1; i <= (*entitylist)->GetHighestEntityIndex(); i++)
				{
					/*
						HACK!!! REMOVE LATER
					*/
					if (!Options::Instance()->aimbot_enable())
						m_ESP->ResetOverrideMatrix(i);

					auto ent = entitylist->GetEntityByIndex(i);

					if (!ent) continue;
					if (ent == g_LocalPlayer) continue;

					m_ESP->Setup(ent);
					m_ESP->Exec();
				}
				//if (Options::Instance()->draw_spread_circle())
					//m_ESP->DrawSpreadCircle();
			}
		}
		else if (panelId == panel)
		{
			m_ESP->Watermark();
#if 1
			if ((*engine)->IsInGame() && (*engine)->IsConnected())
			{
				auto render = Interfaces::Instance()->GetInterface<ISurface>()->get();
				render->DrawSetColor(Color::White);
				render->DrawSetTextPos(50, 350);
				wchar_t buf[512];
				swprintf_s(buf, L"Turn Time: %.3f Anim Turn Time: %.3f", m_flNextTurnTime, m_flAnimNextTime);
				render->DrawPrintText(buf, wcslen(buf));
			}
#endif
			if (Options::Instance()->misc_visualize_aa() && (Interfaces::Instance()->GetInterface<CInput>())->ThirdPerson())
			{
				m_ESP->DrawAngleLines(m_flRealYaw, m_flFakeYaw, g_LocalPlayer->m_flLowerBodyYawTarget());
			}
		}

		return true;
	}
	void VisualSubsystem::OnPostScreenEffects()
	{
		auto glowmanager = Interfaces::Instance()->GetInterface<CGlowObjectManager>();

		for (auto i = 0; i < (*glowmanager)->m_GlowObjectDefinitions.Count(); i++)
		{
			auto& glowobject = (*glowmanager)->m_GlowObjectDefinitions[i];
			auto ent = reinterpret_cast<C_BasePlayer*>(glowobject.m_pEntity);

			if (glowobject.IsUnused())
				continue;

			if (!ent || ent->IsDormant())
				continue;

			if (ent->GetClientClass()->m_ClassID != ClassId_CCSPlayer)
				continue;

			if (!ent->IsAlive())
				continue;

			if (ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				continue;

			glowobject.m_flRed = Options::Instance()->glow_r();
			glowobject.m_flGreen = Options::Instance()->glow_g();
			glowobject.m_flBlue = Options::Instance()->glow_b();
			glowobject.m_flAlpha = Options::Instance()->glow_a();
			glowobject.m_bRenderWhenOccluded = Options::Instance()->glow_rwo();
			glowobject.m_bRenderWhenUnoccluded = Options::Instance()->glow_rwu();
		}
	}
}
