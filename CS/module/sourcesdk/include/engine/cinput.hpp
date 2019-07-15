#pragma once

#include "cusercmd.hpp"

#include "vmtmanager.h"

#define MULTIPLAYER_BACKUP 150

namespace Envy
{
	namespace SourceEngine
	{
		class bf_write;
		class bf_read;

		class _CInput_
		{
		public:
			virtual void  Init_All(void);
			virtual void  Shutdown_All(void);
			virtual int   GetButtonBits(int);
			virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
			virtual void  ExtraMouseSample(float frametime, bool active);
			virtual bool  WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
			virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
			virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);


			inline CUserCmd* GetUserCmd(int sequence_number);
			inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);
			void* pvftable;
			char				pad_0x0001[0x8];			//0x0004	
			bool                m_fTrackIRAvailable;		//0x000C
			bool                m_fMouseInitialized;		//0x0010
			bool                m_fMouseActive;				//0x0014
			bool                m_fJoystickAdvancedInit;	
			char                pad_0x08[0x2C];
			void* m_pKeys;
			char                pad_0x38[0x64];
			int                 pad_0x41;
			int                 pad_0x42;
			bool                m_fCameraInterceptingMouse;
			bool                m_fCameraInThirdPerson;
			bool                m_fCameraMovingWithMouse;
			QAngle              m_vecCameraOffset;
			bool                m_fCameraDistanceMove;
			int                 m_nCameraOldX;
			int                 m_nCameraOldY;
			int                 m_nCameraX;
			int                 m_nCameraY;
			bool                m_CameraIsOrthographic;
			Vector              m_angPreviousViewAngles;
			Vector              m_angPreviousViewAnglesTilt;
			float               m_flLastForwardMove;
			char                pad_0xE4[0x3];
			CUserCmd* m_pCommands;
			CVerifiedUserCmd* m_pVerifiedCommands;
		};
	}


	class CInput : public SourceInterface<SourceEngine::_CInput_>
	{
	public:
		CInput() {}

		virtual Interface* Init() override
		{
			m_data = *reinterpret_cast<Interface**>(
				Peb::Instance()->GetModule("client_panorama.dll").FindPattern("C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 59 C3") + 2
			);
			SourceInterface<SourceEngine::_CInput_>::Init();
			return m_data;
		}
		SourceEngine::CUserCmd* CInput::GetUserCmd(int sequence_number)
		{
			SourceEngine::CUserCmd* commands = *(SourceEngine::CUserCmd**)((uintptr_t)m_data + 0xF4);
			return &commands[sequence_number % MULTIPLAYER_BACKUP];
		}

		SourceEngine::CUserCmd* CInput::GetUserCmd(int slot, int seq)
		{
			typedef SourceEngine::CUserCmd* (__thiscall* GetUserCmdFn)(void*, int, int);
			return VMTManager::Instance()->GetVirtualFunction<GetUserCmdFn>(m_data, 8)(m_data, slot, seq);
		}

		SourceEngine::CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
		{
			SourceEngine::CVerifiedUserCmd* commands = *(SourceEngine::CVerifiedUserCmd**)((uintptr_t)m_data + 0xF8);
			return &commands[sequence_number % MULTIPLAYER_BACKUP];
		}

		bool ThirdPerson()
		{
			return *(bool*)((uintptr_t)m_data + 0xA5);
		}
	};
}