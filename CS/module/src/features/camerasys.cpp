#include "..\..\include\subsystem\camerasys.h"

#include "imaterialsystem.hpp"
#include "csgosdk.h"
#include "structs.h"

#include "inputsys.h"
#include "materialsys.h"

#include <fstream>

#include "math/mathfuncs.h"

#include "menusys.h"

using namespace Envy::SourceEngine;

static constexpr const char* CameraMaterial = 
R"#("screenspace_general"
{
	"$basetexture" "camerart"
})#";

static constexpr const char* CameraShader =
R"#("screenspace_general"
{
	"$pixshader"      "dronecam_ps30"
	"$basetexture"    "camerart"
	"$texture1"       "dev/replay_noise"
	"$c0_x"           "0.0"
	"$c0_y"           "0.0"
	"$c0_z"           "0.0"
	"proxies"
	{
		"uniformnoise"
		{
			"resultvar" "$c0_y"
		}
		"Sine"
		{
			"sineperiod" 8
			"resultVar" "$c0_z"
		}
	}
}
)#";

enum ClearFlags_t
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};


void Envy::CameraSubsystem::OnRenderView(const SourceEngine::CViewSetup& view, const SourceEngine::CViewSetup& hudView, Envy::oFunc oFunc)
{
	SourceEngine::CViewSetup newView = view;

	if (!g_Subsystems->Get<MenuSubsystem>()->GetMenu()->ExternalVisible())
		return;

	switch (m_CameraState)
	{
		case Camera_MIRROR:
		{
			newView.angles.yaw += 180.f;
			break;
		}
		case Camera_SPECTATE:
		{
			auto entList = Interfaces::Instance()->GetInterface<IClientEntityList>();
			auto players = entList->GetAllPlayers();
			m_CurrentSpectate = players[m_CurrentSpectateIdx];
			bool specFound = m_CurrentSpectate->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			int initalIdx = m_CurrentSpectateIdx;
			while (!specFound)
			{
				m_CurrentSpectateIdx = (m_CurrentSpectateIdx + 1) % players.size();
				m_CurrentSpectate = players[m_CurrentSpectateIdx];
				specFound = m_CurrentSpectate->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
				if (m_CurrentSpectateIdx == initalIdx)
				{
					//None found, full loop
					break;
				}
			}
			if (specFound)
			{
				QAngle eyeAngles = m_CurrentSpectate->m_angEyeAngles();
				Vector viewPosition = m_CurrentSpectate->m_vecOrigin() + m_CurrentSpectate->m_vecViewOffset();
				newView.origin = viewPosition;
				newView.angles = eyeAngles;
			}
			else
			{
				newView.angles.yaw += 180.f;
			}
			break;
		}
	}

	newView.x = newView.oldX = 0;
	newView.y = newView.oldY = 0;
	newView.width = newView.oldWidth = m_CameraWidth;
	newView.height = newView.oldHeight = m_CameraHeight;
	newView.m_flAspectRatio = float(newView.width) / (float)newView.height;

	newView.m_flNearBlurDepth = 20.f;
	newView.m_flNearFocusDepth = 50.f;
	newView.m_flFarBlurDepth = 0.f;
	newView.m_flFarFocusDepth = 50.f;
	newView.m_flNearBlurRadius = 0.f;
	newView.m_flFarBlurRadius = 0.f;
	newView.m_nMotionBlurMode = MotionBlurMode_t::MOTION_BLUR_DISABLE; /*MOTION_BLUR_DISABLE*/;

	newView.zNear = newView.zNearViewmodel = 7.f;
	newView.zFar = newView.zFarViewmodel = 28377.9199f;

	auto ctx = Interfaces::Instance()->GetInterface<IMaterialSystem>()->GetRenderContext();

	ctx->PushRenderTargetAndViewport();
	ctx->SetRenderTarget(m_pCameraTexture);
	
	auto viewRender = Interfaces::Instance()->GetInterface<IViewRender>();
	oFunc(
		viewRender->get(), newView, newView, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, 0
	);

	//(*viewRender)->SetScreenOverlayMaterial(NULL);

	ctx->PopRenderTargetAndViewport();
	ctx->Release();
}

void Envy::CameraSubsystem::DrawCameraTexture()
{
	auto ctx = Interfaces::Instance()->GetInterface<IMaterialSystem>()->GetRenderContext();

	ctx->DrawScreenSpaceRectangle(m_pCameraMaterial, 0, 0, 500, 500, 0, 0, 500.f, 500.f, m_pCameraTexture->GetActualWidth(), m_pCameraTexture->GetActualHeight(), 0, 1, 1);

	ctx->Release();
}

void Envy::CameraSubsystem::NextSpectate()
{
	m_CurrentSpectateIdx++;
}

void Envy::CameraSubsystem::OnToggleCameraState()
{
	switch (m_CameraState)
	{
	case Camera_MIRROR: SetCameraState(Camera_SPECTATE); break;
	case Camera_SPECTATE: SetCameraState(Camera_MIRROR); break;
	}
}
#pragma optimize( "", off )
void Envy::CameraSubsystem::Init()
{
	auto materialSystem = Interfaces::Instance()->GetInterface<IMaterialSystem>();
//	materialSystem->ForceBeginRenderTargetAllocation();
	m_pCameraTexture = materialSystem->CreateFullFrameRenderTarget("spycam_texture");
	//materialSystem->ForceEndRenderTargetAllocation();

	std::string materialName = "nadecam_material";
	std::string materialData = CameraMaterial;

	std::string shaderName = "nadecam_shader_mat";
	std::string shaderData = CameraShader;

	m_pCameraMaterial = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial(materialName, materialData);
	m_pCameraShaderMaterial = g_Subsystems->Get<MaterialSubsystem>()->CreateMaterial(shaderName, shaderData);
	auto inputSys = g_Subsystems->Get<InputSubsystem>();

	inputSys->BindHotkey(VK_RMENU, this, &CameraSubsystem::NextSpectate);
	inputSys->BindHotkey(VK_HOME, this, &CameraSubsystem::OnToggleCameraState);

}
#pragma optimize( "", on ) 

IDirect3DTexture9* Envy::CameraSubsystem::GetRawCameraTexture()
{
	if (m_pCameraMaterial == NULL || m_pCameraTexture == NULL) return NULL;

	auto handle = m_pCameraTexture->TextureHandles[0];

	return handle->d3dTexture;
}
