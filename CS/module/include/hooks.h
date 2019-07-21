#pragma once

#include <d3d9.h>

#include "function.hpp"
#include "envydefs.hpp"
#include "structs.h"

#ifdef PlaySound
#undef PlaySound
#endif

namespace Envy
{
	namespace Index
	{
		constexpr auto EndScene = 42;
		constexpr auto Reset = 16;
		constexpr auto PaintTraverse = 41;
		constexpr auto CreateMove = 22;
		constexpr auto PlaySound = 82;
		constexpr auto FrameStageNotify = 36;
		constexpr auto DrawModelExecute = 21;
		constexpr auto DoPostScreenSpaceEffects = 44;
		constexpr auto ClientPredictionUpdate = 3;
		constexpr auto WriteUsercmdDeltaToBuffer = 24;
		constexpr auto Transmit = 49;
		constexpr auto LockCursor = 67;
		constexpr auto UnlockCursor = 66;
		constexpr auto RenderView = 6;
	}
	namespace Hooks
	{
		HRESULT __stdcall hkEndScene(IDirect3DDevice9* device);
		HRESULT __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
		void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
		void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active);
		void __fastcall hkPaintTraverse(SourceEngine::_IPanel_*, int edx, SourceEngine::vgui::VPANEL panel, bool forceRepaint, bool allowForce);
		void __fastcall hkFrameStageNotify(SourceEngine::_IBaseClientDll_* client, int edx, SourceEngine::ClientFrameStage_t stage);
		void __fastcall hkUpdate(SourceEngine::_IPrediction_* pred, int edx, int startframe, bool validframe, int incoming_ack, int outgoing_cmd);
		bool __fastcall hkWriteUsercmdDeltaToBuffer(SourceEngine::_IBaseClientDll_*, int, int, void*, int, int, bool);
		int __fastcall hkDoPostScreenEffects(SourceEngine::_IClientMode_*, int, int);
		void __fastcall hkRenderView(SourceEngine::_IViewRender_*, int, SourceEngine::CViewSetup&, SourceEngine::CViewSetup&, int, int);
	}


	void ENVY_API InitializeHooks();
}