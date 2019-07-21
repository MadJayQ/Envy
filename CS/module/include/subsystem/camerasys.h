#pragma once

#include "subsystem.h"
#include "basehandle.hpp"

#include "csgosdk.h"

#include <d3d9.h>

namespace Envy
{
	//Forward 
	using oFunc = Function<void, THISCALL, _IViewRender_*, CViewSetup&, CViewSetup&, int, int>;

	enum CameraState
	{
		Camera_MIRROR = 0,
		Camera_SPECTATE
	};
	class CameraSubsystem : public ICheatSubsystem
	{
	public:
		CameraSubsystem()
		{
			m_pCameraTexture = NULL;
			m_pCameraMaterial = NULL;
			m_CurrentSpectate = NULL;
			m_CurrentSpectateIdx = 0;
			m_CameraState = CameraState::Camera_MIRROR;
			
		}

		void OnRenderView(const SourceEngine::CViewSetup& view, const SourceEngine::CViewSetup& hudView, oFunc func);
		void DrawCameraTexture();

		void SetCameraExtents(int width, int height) { m_CameraWidth = width; m_CameraHeight = height; }
		void NextSpectate();

		void SetCameraState(CameraState state) { m_CameraState = state; }

		void OnToggleCameraState();

		virtual void Init() override;

		IDirect3DTexture9* GetRawCameraTexture();
	private:

		int m_CameraWidth;
		int m_CameraHeight;
		int m_CurrentSpectateIdx;

		CameraState m_CameraState;

		SourceEngine::ITexture* m_pCameraTexture;
		SourceEngine::IMaterial* m_pCameraMaterial;
		SourceEngine::C_BasePlayer* m_CurrentSpectate;
	};
}