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

			bool                m_fTrackIRAvailable;            //0x04
			bool                m_fMouseInitialized;            //0x05
			bool                m_fMouseActive;                 //0x06
			bool                m_fJoystickAdvancedInit;        //0x07
			char                pad_0x08[0x2C];                 //0x08
			void*               m_pKeys;                        //0x34
			char                pad_0x38[0x64];                 //0x38
			bool                m_fCameraInterceptingMouse;     //0x9C
			bool                m_fCameraInThirdPerson;         //0x9D
			bool                m_fCameraMovingWithMouse;       //0x9E
			Vector              m_vecCameraOffset;              //0xA0
			bool                m_fCameraDistanceMove;          //0xAC
			int                 m_nCameraOldX;                  //0xB0
			int                 m_nCameraOldY;                  //0xB4
			int                 m_nCameraX;                     //0xB8
			int                 m_nCameraY;                     //0xBC
			bool                m_CameraIsOrthographic;         //0xC0
			QAngle              m_angPreviousViewAngles;        //0xC4
			QAngle              m_angPreviousViewAnglesTilt;    //0xD0
			float               m_flLastForwardMove;            //0xDC
			int                 m_nClearInputState;             //0xE0
			char                pad_0xE4[0x8];                  //0xE4
			CUserCmd*           m_pCommands;                    //0xEC
			CVerifiedUserCmd*   m_pVerifiedCommands;            //0xF0
		};
	}


	class CInput : public SourceInterface<SourceEngine::_CInput_>
	{
	public:
		CInput() {}

		virtual Interface* Init() override
		{
			m_data = *reinterpret_cast<Interface**>(
				Peb::Instance()->GetModule("client.dll").FindPattern("B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1
			);
			SourceInterface<SourceEngine::_CInput_>::Init();
			return m_data;
		}
		SourceEngine::CUserCmd* CInput::GetUserCmd(int sequence_number)
		{
			return &m_data->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
		}

		SourceEngine::CUserCmd* CInput::GetUserCmd(int slot, int seq)
		{
			typedef SourceEngine::CUserCmd* (__thiscall* GetUserCmdFn)(void*, int, int);
			return VMTManager::Instance()->GetVirtualFunction<GetUserCmdFn>(m_data, 8)(m_data, slot, seq);
		}

		SourceEngine::CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
		{
			return &m_data->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
		}

		bool ThirdPerson()
		{
			return *(bool*)((uintptr_t)m_data + 0xA5);
		}
	};
}