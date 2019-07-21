#pragma once

#include "vmatrix.h"

#include "peb.h"

namespace Envy
{
	namespace SourceEngine
	{
		class IPanel;
		class C_BaseEntity;

		enum class ClearFlags_t
		{
			VIEW_CLEAR_COLOR = 0x1,
			VIEW_CLEAR_DEPTH = 0x2,
			VIEW_CLEAR_FULL_TARGET = 0x4,
			VIEW_NO_DRAW = 0x8,
			VIEW_CLEAR_OBEY_STENCIL = 0x10,
			VIEW_CLEAR_STENCIL = 0x20,
		};


		enum class MotionBlurMode_t
		{
			MOTION_BLUR_DISABLE = 1,
			MOTION_BLUR_GAME = 2,
			MOTION_BLUR_SFM = 3
		};

		class CViewSetup
		{
		public:
			int x;
			int oldX;
			int y;
			int oldY;
			int width;
			int oldWidth;
			int height;
			int oldHeight;

			bool m_bOrtho;
			float m_OrthoLeft;
			float m_OrthoTop;
			float m_OrthoRight;
			float m_OrthoBottom;

		private:
			char pad1[0x7C];

		public:
			float fov;
			float fovViewmodel;
			Vector origin;
			QAngle angles;

			float zNear;
			float zFar;
			float zNearViewmodel;
			float zFarViewmodel;

			float m_flAspectRatio;
			float m_flNearBlurDepth;
			float m_flNearFocusDepth;
			float m_flFarFocusDepth;
			float m_flFarBlurDepth;
			float m_flNearBlurRadius;
			float m_flFarBlurRadius;
			int m_nDoFQuality;
			MotionBlurMode_t m_nMotionBlurMode;

			float m_flShutterTime;
			Vector m_vShutterOpenPosition;
			QAngle m_shutterOpenAngles;
			Vector m_vShutterClosePosition;
			QAngle m_shutterCloseAngles;

			float m_flOffCenterTop;
			float m_flOffCenterBottom;
			float m_flOffCenterLeft;
			float m_flOffCenterRight;

			bool m_bOffCenter : 1;
			bool m_bRenderToSubrectOfLargerScreen : 1;
			bool m_bDoBloomAndToneMapping : 1;
			bool m_bDoDepthOfField : 1;
			bool m_bHDRTarget : 1;
			bool m_bDrawWorldNormal : 1;
			bool m_bCullFrontFaces : 1;
			bool m_bCacheFullSceneState : 1;
			bool m_bRenderFlashlightDepthTranslucents : 1;
		private:
			char pad2[0x40];

		};//Size=0x014C

		class _IClientMode_
		{
		public:
			virtual             ~_IClientMode_() {}
			virtual int         ClientModeCSNormal(void *) = 0;
			virtual void        Init() = 0;
			virtual void        InitViewport() = 0;
			virtual void        Shutdown() = 0;
			virtual void        Enable() = 0;
			virtual void        Disable() = 0;
			virtual void        Layout() = 0;
			virtual IPanel*     GetViewport() = 0;
			virtual void*       GetViewportAnimationController() = 0;
			virtual void        ProcessInput(bool bActive) = 0;
			virtual bool        ShouldDrawDetailObjects() = 0;
			virtual bool        ShouldDrawEntity(C_BaseEntity *pEnt) = 0;
			virtual bool        ShouldDrawLocalPlayer(C_BaseEntity *pPlayer) = 0;
			virtual bool        ShouldDrawParticles() = 0;
			virtual bool        ShouldDrawFog(void) = 0;
			virtual void        OverrideView(CViewSetup *pSetup) = 0;
			virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
			virtual void        StartMessageMode(int iMessageModeType) = 0;
			virtual IPanel*     GetMessagePanel() = 0;
			virtual void        OverrideMouseInput(float *x, float *y) = 0;
			virtual bool        CreateMove(float flInputSampleTime, void* usercmd) = 0;
			virtual void        LevelInit(const char *newmap) = 0;
			virtual void        LevelShutdown(void) = 0;
		};
	}

	class IClientMode : public SourceInterface<SourceEngine::_IClientMode_>
	{
	public:
		IClientMode() {}

		virtual Interface* Init() override
		{

			intptr_t clientModeAddr = reinterpret_cast<intptr_t>(Peb::Instance()->GetModule("client_panorama.dll").FindPattern("8B 0D ? ? ? ? 8B 01 5D FF 60 30") + 0x2);
			m_data = **reinterpret_cast<Interface***>(
				clientModeAddr
			);
			SourceInterface<SourceEngine::_IClientMode_>::Init();
			return m_data;
		}
	};
}