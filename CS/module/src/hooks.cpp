#include "..\include\hooks.h"
#include "function.hpp"
#include "vmtmanager.h"
#include "envydefs.hpp"
#include "structs.h"
#include "subsystem.h"
#include "menusys.h"
#include "visualsys.h"
#include "viewanglesys.h"
#include "camerasys.h"
#include "lagcompsys.h"


#include <intrin.h>

using namespace Envy::SourceEngine;


namespace Envy 
{

	SourceEngine::C_LocalPlayer g_LocalPlayer;

	Function<HRESULT, STDCALL, IDirect3DDevice9*> oEndScene(INVALID);
	Function<HRESULT, STDCALL, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*> oReset(INVALID);
	Function<void, THISCALL, _IBaseClientDll_*, int, float, bool> oCreateMove(INVALID);
	Function<bool, THISCALL, _IBaseClientDll_*, int, void*, int, int, bool>oWriteUsercmdDeltaToBuffer(INVALID);
	Function<void, THISCALL, _IPanel_*, SourceEngine::vgui::VPANEL, bool, bool> oPaintTraverse(INVALID);
	Function<void, THISCALL, _IBaseClientDll_*, ClientFrameStage_t> oFrameStageNotify(INVALID);
	Function<void, THISCALL, _IPrediction_*, int, bool, int, int> oUpdate(INVALID);
	Function<int, THISCALL, _IClientMode_*, int> oDoPostScreenEffects(INVALID);
	Function<void, THISCALL, _ISurface_*> oUnlockCursor(INVALID);
	Function<void, THISCALL, _ISurface_*>oLockCursor(INVALID);
	Function<void, THISCALL, _IViewRender_*, CViewSetup&, CViewSetup&, int, int> oRenderView(INVALID);
	Function<void, THISCALL, _IVModelRender_*, IMatRenderContext*, void*, const ModelRenderInfo_t&, matrix3x4_t*> oDrawModelExecute(INVALID);



	VMT* panel_vmt;
	VMT* client_vmt;
	VMT* surface_vmt;
	VMT* g_pClientPredictionVMT;
	VMT* g_pClientModeVMT;
	VMT* viewrender_vmt;
	VMT* modelrender_vmt;

	IDirect3DDevice9* g_D3DDevice;
	namespace Hooks 
	{
		HRESULT ENVY_API hkEndScene(
			IDirect3DDevice9* device
		)
		{
			if (oEndScene == INVALID)
			{
				oEndScene->assign(
					VMTManager::Instance()->GetVMTFunction(
					(uintptr_t)device,
					Envy::Index::EndScene
					)
				);
			}
			CACHED auto menu = g_Subsystems->Get<MenuSubsystem>();
			menu->OnEndScene(device);

			return oEndScene(device);
		}

		HRESULT ENVY_API hkReset(
			IDirect3DDevice9 * device,
			D3DPRESENT_PARAMETERS * pPresentationParameters
		)
		{
			if (oReset == INVALID)
			{
				oReset->assign(
					VMTManager::Instance()->GetVMTFunction(
					(uintptr_t)device,
						Envy::Index::Reset
					)
				);
			}
			CACHED auto menu = g_Subsystems->Get<MenuSubsystem>();
			menu->OnDeviceLost();

			auto hr = oReset(device, pPresentationParameters);
			if (hr >= 0)
			{
				menu->OnReset(
					device,
					pPresentationParameters
				);
				//Load fonts
			}

			return hr;
		}
		CACHED int used = 0;
		void ENVY_API hkCreateMove(
			int sequence_number, 
			float input_sample_frametime, 
			bool active, 
			bool & sendpacket
		)
		{
			CACHED auto clientptr = Interfaces::Instance()->GetInterface<IBaseClientDll>()->get();
			if (oCreateMove == INVALID)
			{
				oCreateMove->assign(
					VMTManager::Instance()->GetVMTFunction(
					(uintptr_t)clientptr,
						Envy::Index::CreateMove
					)
				);
			}
			oCreateMove(
				clientptr,
				sequence_number, 
				input_sample_frametime, 
				active
			);
			CACHED auto input = Interfaces::Instance()->GetInterface<CInput>();
			CACHED auto viewanglesys = g_Subsystems->Get<ViewangleSubssytem>();

			auto cmd = (input)->GetUserCmd(sequence_number);
			auto verified = input->GetVerifiedCmd(sequence_number);

			if (!cmd || !cmd->command_number)
				return;
		
			viewanglesys->OnCreateMove(
				cmd,
				sendpacket,
				sequence_number
			);
			if (!cmd->Entrust())
			{
				MessageBox(NULL, "CRITICAL USERCMD FAILURE PREVENTATIVE MEASURES TAKEN TO ENSURE ACCOUNT SAFETY!", "CRITICAL FAILURE", MB_ICONERROR);
				assert(false);
			}

			if (Options::Instance()->misc_lag_exploit() && (cmd->buttons & IN_USE))
			{
				if (used++ < 3)
				{
					auto time = Interfaces::Instance()->GetInterface<CGlobalVarsBase>()->TimeToTick(11.f);
					viewanglesys->m_nTickbaseShift() = time;
				}
				else
				{
					used = 0;
				}
			}

			if (!sendpacket)
			{
				g_LocalPlayer.m_nChokedTicks()++;
			}
			else
			{
				g_LocalPlayer.m_nChokedTicks() = 0;
			}
			verified->m_cmd = *cmd;
			verified->m_crc = cmd->GetChecksum();
		}
		__declspec(naked) void __stdcall hkCreateMove_Proxy(
			int sequence_number, 
			float input_sample_frametime, 
			bool active
		)
		{
			__asm
			{
				push ebp
				mov ebp, esp
				push ebx
				lea ecx, [esp]
				push ecx
				push dword ptr[active]
				push dword ptr[input_sample_frametime]
				push dword ptr[sequence_number]
				call Hooks::hkCreateMove
				pop ebx
				pop ebp
				retn 0Ch
			}
		}
		void ENVY_HOOK hkPaintTraverse(
			_IPanel_ * thisptr, 
			int edx, 
			vgui::VPANEL panel, 
			bool forceRepaint, 
			bool allowForce
		)
		{
			if (oPaintTraverse == INVALID)
			{
				oPaintTraverse->assign(
					VMTManager::Instance()->GetVMTFunction(
					(uintptr_t)thisptr,
						Envy::Index::PaintTraverse
					)
				);
			}
			oPaintTraverse(thisptr, panel, forceRepaint, allowForce);
			CACHED auto visuals = g_Subsystems->Get<VisualSubsystem>();
			visuals->OnPaintTraverse(panel, forceRepaint, allowForce);
		}
		void ENVY_HOOK hkUpdate(
			_IPrediction_* pred,
			int edx,
			int startframe,
			bool validframe,
			int incoming_ack,
			int outgoing_cmd
		)
		{
			if (oUpdate == INVALID)
			{
				oUpdate->assign(
					FROMVMT(pred, Index::ClientPredictionUpdate)
				);
			}

			g_Subsystems->Get<ViewangleSubssytem>()->OnPredictionUpdate(
				startframe,
				validframe,
				incoming_ack,
				outgoing_cmd
			);

			oUpdate(pred, startframe, validframe, incoming_ack, outgoing_cmd);
		}
		struct INetMessage {
			virtual ~INetMessage();
		};

		template<typename T>
		class CNetMessagePB : public INetMessage, public T {};

		class CCLCMsg_Move {
		private:
			char __PAD0[0x8];
		public:
			int numBackupCommands;
			int numNewCommands;
		};

		using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;
		static bool inSendMove = false;
		static bool firstSendMovePack = false;
		CACHED Function<void> cl_sendmove(INVALID);
		uintptr_t WriteUsercmdLocation;
		CACHED uintptr_t cl_sendmove_ret;

		void WriteUsercmd(void* buf, CUserCmd* to, CUserCmd* from)
		{
			_asm
			{
				mov ecx, buf
				mov edx, to
				push from
				call WriteUsercmdLocation
				add esp, 4h
			}
		}

		bool ENVY_HOOK hkWriteUsercmdDeltaToBuffer(
			_IBaseClientDll_ * client, 
			int edx, 
			int slot, 
			void *buf, 
			int from,
			int to, 
			bool newcmd)
		{
			if (oWriteUsercmdDeltaToBuffer == INVALID)
			{
				cl_sendmove->assign((uintptr_t)Peb::Instance()->GetModule("engine.dll").FindPattern("55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98"));
				cl_sendmove_ret = (uintptr_t)Peb::Instance()->GetModule("engine.dll").FindPattern("84 C0 74 04 B0 01 EB 02 32 C0 8B FE");
				WriteUsercmdLocation = (uintptr_t)Peb::Instance()->GetModule("client_panorama.dll").FindPattern("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
				oWriteUsercmdDeltaToBuffer->assign(
					FROMVMT(client, Index::WriteUsercmdDeltaToBuffer)
				);
			}
			auto ret = (uintptr_t)_ReturnAddress();
			auto viewanglesys = g_Subsystems->Get<ViewangleSubssytem>();
			if (viewanglesys->m_nTickbaseShift() <= 0 || ret != cl_sendmove_ret)
			{
				return oWriteUsercmdDeltaToBuffer(
					client,
					slot,
					buf,
					from,
					to,
					newcmd
				);
			}
			if (from != -1)
			{
				return true;
			}

			uintptr_t framepointer;
			_asm
			{
				mov framepointer, ebp
			}

			auto msg = reinterpret_cast<CCLCMsg_Move_t*>(framepointer + 0xFCC);
			auto state = Interfaces::Instance()->GetInterface<CClientState>()->get();
			auto netchan = state->m_nNetChannel;

			int newcommands = msg->numNewCommands;

			if (!inSendMove)
			{
				if (newcommands <= 0)
				{
					return false;
				}

				inSendMove = true;
				firstSendMovePack = true;
				viewanglesys->m_nTickbaseShift() += newcommands;

				while (viewanglesys->m_nTickbaseShift() > 0)
				{
					cl_sendmove();
					int* vmt = *(int**)netchan;
					int address = vmt[49];
					using TransmitFn = bool(__thiscall*)(INetChannel*, bool);
					TransmitFn transmit = (TransmitFn)(address);
					transmit(netchan, false);
					firstSendMovePack = false;
				}

				inSendMove = false;
				return false;
			}

			if (!firstSendMovePack)
			{
				int loss = min(viewanglesys->m_nTickbaseShift(), 15);
				viewanglesys->m_nTickbaseShift() -= loss;
				netchan->m_nOutSequenceNr += loss;
			}

			int lastoutgoingcommand = *(int*)((uintptr_t)state + 0x4CAC);
			int next = lastoutgoingcommand + state->chokedcommands + 1;
			int totalnew = min(viewanglesys->m_nTickbaseShift(), 15);
			viewanglesys->m_nTickbaseShift() -= totalnew;

			from = -1;
			msg->numNewCommands = totalnew;
			msg->numBackupCommands = 0;

			for (to = next - newcommands + 1; to <= next; to++)
			{
				if (!oWriteUsercmdDeltaToBuffer(client, slot, buf, from, to, true))
					return false;
				from = to;
			}

			auto input = Interfaces::Instance()->GetInterface<CInput>();
			
			CUserCmd* lastreal = nullptr;//(*input)->GetUserCmd(slot, from);
			CUserCmd fromCmd;
			if (lastreal)
			{
				fromCmd = *lastreal;
			}

			CUserCmd toCmd = fromCmd;
			toCmd.command_number++;
			toCmd.tick_count += 200;

			for (int i = newcommands; i <= totalnew; i++)
			{
				WriteUsercmd(buf, &toCmd, &fromCmd);
				fromCmd = toCmd;
				toCmd.command_number++;
				toCmd.tick_count++;
			}

			return true;
		}

		void ENVY_HOOK hkDrawModelExecute(
			_IVModelRender_* modelRenderer,
			int edx,
			IMatRenderContext* context,
			void* state,
			const ModelRenderInfo_t& info,
			matrix3x4_t* matrix
		)
		{
			if (oDrawModelExecute == INVALID)
			{
				oDrawModelExecute->assign(
					FROMVMT(modelRenderer, Index::DrawModelExecute)
				);
			}

			auto visualSys = g_Subsystems->Get<VisualSubsystem>();

			bool callOriginal = visualSys->OnDrawModelExecute(context, state, info, matrix, oDrawModelExecute);

			if (callOriginal)
			{
				oDrawModelExecute(modelRenderer, context, state, info, matrix);
			}
		}
		int ENVY_HOOK hkDoPostScreenEffects(
			_IClientMode_ * clientmode, 
			int edx, 
			int a1
		)
		{
			if (oDoPostScreenEffects == INVALID)
			{
				oDoPostScreenEffects->assign(
					FROMVMT(clientmode, Index::DoPostScreenSpaceEffects)
				);
			}

			g_Subsystems->Get<VisualSubsystem>()->OnPostScreenEffects();

			return oDoPostScreenEffects(clientmode, a1);
		}
		void ENVY_HOOK hkFrameStageNotify(
			_IBaseClientDll_ * client, 
			int edx, 
			ClientFrameStage_t stage
		)
		{
			if (oFrameStageNotify == INVALID)
			{
				oFrameStageNotify->assign(
					FROMVMT(client, Index::FrameStageNotify)
				);
			}

			CACHED auto viewanglesys = g_Subsystems->Get<ViewangleSubssytem>();
			CACHED auto lagcompsys = g_Subsystems->Get<LagCompensationSubsystem>();


			//viewanglesys->OnFrameStageNotify(stage);
			//lagcompsys->OnFrameStageNotify(stage);

			if (stage == ClientFrameStage_t::FRAME_RENDER_START)
			{

				CACHED uintptr_t OcclusionOffset = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("C7 87 ? ? ? ? ? ? ? ? 85 F6 0F 84") + 2;
				CACHED uintptr_t FrameCountOFfset = (uintptr_t)Peb::Instance()->GetModule("client.dll").FindPattern("8B B7 ? ? ? ? 89 75 F8") + 2;

				auto globalVars = Interfaces::Instance()->GetInterface<CGlobalVarsBase>();
				auto engineClient = Interfaces::Instance()->GetInterface<IVEngineClient>();
				auto entityList = Interfaces::Instance()->GetInterface<IClientEntityList>();

				for (int i = 1; i <= (*globalVars)->maxClients; i++)
				{
					if (i == (*engineClient)->GetLocalPlayer()) continue;

					C_BaseEntity* entity = entityList->GetEntityByIndex(i);

					if (!entity) continue;

					*(int*)((uintptr_t)entity + 0xA30) = (*globalVars)->framecount;
					*(int*)((uintptr_t)entity + 0xA28) = 0;
				}
			}

			return oFrameStageNotify(client, stage);

		}

		void ENVY_HOOK hkRenderView(
			_IViewRender_* render,
			int edx,
			CViewSetup& view,
			CViewSetup& hudView,
			int clearFlags,
			int whatToDraw
		)
		{
			if (oRenderView == INVALID)
			{
				oRenderView->assign(
					FROMVMT(render, Index::RenderView)
				);
			}


			auto cameraSys = g_Subsystems->Get<CameraSubsystem>();
			cameraSys->OnRenderView(view, hudView, oRenderView);

			oRenderView(render, view, hudView, clearFlags, whatToDraw);

		}
		void ENVY_HOOK hkUnlockCursor(
			_ISurface_* surface,
			int edx
		)
		{
			if (oUnlockCursor == INVALID)
			{
				oUnlockCursor->assign(
					FROMVMT(surface, Index::UnlockCursor)
				);
			}

			return oUnlockCursor(surface);
		}
		void ENVY_HOOK hkLockCursor(
			_ISurface_* surface,
			int edx
		)
		{
			if (oLockCursor == INVALID)
			{
				oLockCursor->assign(
					FROMVMT(surface, Index::LockCursor)
				);
			}

			auto menuSys = g_Subsystems->Get<MenuSubsystem>();
			if (menuSys->GetMenu()->Visible())
			{
				surface->UnlockCursor();
			}
			else {
				oLockCursor(surface);
			}
		}
	}
	void ENVY_API InitializeHooks()
	{
		auto peb = Peb::Instance();
		auto clientDLL = peb->GetModule("client_panorama.dll");
		g_LocalPlayer = *(C_LocalPlayer*)(clientDLL.FindPattern("8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
		auto module = Peb::Instance()->GetModule("shaderapidx9.dll");
		g_D3DDevice = **(IDirect3DDevice9***)(module.FindPattern("A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
		auto device_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)g_D3DDevice);
		device_vmt->HookFunction(Index::EndScene, (uintptr_t)Hooks::hkEndScene);

		auto base_client = Interfaces::Instance()->GetInterface<IBaseClientDll>();
		auto panel = Interfaces::Instance()->GetInterface<IPanel>();
		auto prediction = Interfaces::Instance()->GetInterface<IPrediction>();
		auto clientmode = Interfaces::Instance()->GetInterface<IClientMode>();
		auto surface = Interfaces::Instance()->GetInterface<ISurface>();
		auto viewrender = Interfaces::Instance()->GetInterface<IViewRender>();
		auto modelrender = Interfaces::Instance()->GetInterface<IVModelRender>();

		panel_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)panel->get());
		client_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)base_client->get());
		surface_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)surface->get());
		viewrender_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)viewrender->get());
		modelrender_vmt = VMTManager::Instance()->CreateVMT((uintptr_t)modelrender->get());


		panel_vmt->HookFunction(Index::PaintTraverse, (uintptr_t)Hooks::hkPaintTraverse);
		surface_vmt->HookFunction(Index::LockCursor, (uintptr_t)Hooks::hkLockCursor);
		viewrender_vmt->HookFunction(Index::RenderView, (uintptr_t)Hooks::hkRenderView);
		client_vmt->HookFunction(Index::CreateMove, (uintptr_t)Hooks::hkCreateMove_Proxy);
		//Interfaces::Instance()->GetInterface<CInput>();
		client_vmt->HookFunction(Index::FrameStageNotify, (uintptr_t)Hooks::hkFrameStageNotify);
		//client_vmt->HookFunction(Index::WriteUsercmdDeltaToBuffer, (uintptr_t)Hooks::hkWriteUsercmdDeltaToBuffer);

		//g_pClientPredictionVMT = VMTManager::Instance()->CreateVMT((uintptr_t)prediction->get());
		//g_pClientPredictionVMT->HookFunction(Index::ClientPredictionUpdate, (uintptr_t)Hooks::hkUpdate);

		g_pClientModeVMT = VMTManager::Instance()->CreateVMT((uintptr_t)clientmode->get());
		g_pClientModeVMT->HookFunction(Index::DoPostScreenSpaceEffects, (uintptr_t)Hooks::hkDoPostScreenEffects);

		modelrender_vmt->HookFunction(Index::DrawModelExecute, (uintptr_t)Hooks::hkDrawModelExecute);

		g_Subsystems->Get<CameraSubsystem>();
	}
}