#include "..\..\include\subsystem\visualsys.h"
#include "interfaces.h"
#include "structs.h"
#include "camerasys.h"
#include "materialsys.h"


#include <mathfuncs.h>

using namespace Envy::SourceEngine;
namespace Envy
{
	void VisualSubsystem::Init()
	{
		m_ESP = std::make_unique<ESP>();

		m_pChamsMaterial = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial("chamsMaterial", ChamsMaterial);
		m_pChamsMaterialBack = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial("chamsMaterialBack", ChamsMaterialBack);
		m_pChamsMaterialRef = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial("chamsMaterial", ChamsMaterialRef);
		m_pChamsMaterialBackRef = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial("chamsMaterialBack", ChamsMaterialBackRef);
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

			//g_Subsystems->Get<CameraSubsystem>()->DrawCameraTexture();
//#if 1
//			if ((*engine)->IsInGame() && (*engine)->IsConnected())
//			{
//				auto render = Interfaces::Instance()->GetInterface<ISurface>()->get();
//				render->DrawSetColor(Color::White);
//				render->DrawSetTextPos(50, 350);
//				wchar_t buf[512];
//				swprintf_s(buf, L"Turn Time: %.3f Anim Turn Time: %.3f", m_flNextTurnTime, m_flAnimNextTime);
//				render->DrawPrintText(buf, wcslen(buf));
//			}
//#endif
//			if (/*Options::Instance()->misc_visualize_aa() && (Interfaces::Instance()->GetInterface<CInput>())->ThirdPerson()*/ false )
//			{
//				m_ESP->DrawAngleLines(m_flRealYaw, m_flFakeYaw, g_LocalPlayer->m_flLowerBodyYawTarget());
//			}
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

			if (ent->GetClientClass()->m_ClassID != 40)
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
			glowobject.m_bFullBloomRender = Options::Instance()->glow_bloom();
			glowobject.m_nGlowStyle = Options::Instance()->glow_style();
		}
	}
	bool VisualSubsystem::OnDrawModelExecute(IMatRenderContext* context, void* state, const ModelRenderInfo_t& model, matrix3x4_t* boneToWorld, Envy::oDrawModelExecuteFn oDrawModelExecute)
	{

		const auto SetMaterialAndColor = [&](IMaterial* material, Color color = Color::White) -> void
		{
			auto renderView = Interfaces::Instance()->GetInterface<IVRenderView>();
			auto modelRender = Interfaces::Instance()->GetInterface<IVModelRender>();

			if (material == nullptr) return;

			(*renderView)->SetColorModulation(color.rNormalized(), color.gNormalized(), color.bNormalized());
			(*modelRender)->ForcedMaterialOverride(material);
		};

		const auto ResetMaterialSettings = [&]() -> void
		{
			(*Interfaces::Instance()->GetInterface<IVModelRender>())->ForcedMaterialOverride(NULL);
		};

		const model_t* modelPtr = reinterpret_cast<const model_t*>(model.pModel);

		auto modelInfo = Interfaces::Instance()->GetInterface<IVModelInfoClient>();
		auto modelRender = Interfaces::Instance()->GetInterface<IVModelRender>();
		auto entList = Interfaces::Instance()->GetInterface<IClientEntityList>();

		std::string modelName = (*modelInfo)->GetModelName(modelPtr);

		//TODO(Jake): For now we only care about player chams
		if (modelName.find("models/player/contactshadow/contactshadow") != std::string::npos)
		{
			return true;
		}
		if (Options::Instance()->chams_enabled() && modelName.find("models/player") != std::string::npos)
		{
			auto player = entList->GetPlayerByIndex(model.entity_index);
			if (g_LocalPlayer->EntIndex() == model.entity_index)
			{
				return true;
			}
			if (g_LocalPlayer->m_iTeamNum() == player->m_iTeamNum())
			{
				return true;
			}
			auto backMaterial = (Options::Instance()->chams_reflective()) ? m_pChamsMaterialBackRef : m_pChamsMaterialBack;
			auto frontMaterial = (Options::Instance()->chams_reflective()) ? m_pChamsMaterialRef : m_pChamsMaterial;
			if (player->IsAlive())
			{
				SetMaterialAndColor(backMaterial, Color::DarkRed);
				oDrawModelExecute(modelRender->get(), context, state, model, boneToWorld);
				SetMaterialAndColor(frontMaterial, Color::DarkYellow);
				oDrawModelExecute(modelRender->get(), context, state, model, boneToWorld);
				ResetMaterialSettings();
			}
			else
			{
				SetMaterialAndColor(backMaterial, Color::White);
				oDrawModelExecute(modelRender->get(), context, state, model, boneToWorld);
				SetMaterialAndColor(frontMaterial, Color::White);
				oDrawModelExecute(modelRender->get(), context, state, model, boneToWorld);
				ResetMaterialSettings();
			}

			return false;
		}

		return true;

	}
}
