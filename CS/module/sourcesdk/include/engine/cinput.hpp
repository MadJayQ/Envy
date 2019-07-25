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
			virtual CUserCmd* GetUserCmd(int sequence_number);


			inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);
			void* pvftable;											//0x0

			char				pad_0x04[0x8];						//0x04
			bool                m_fTrackIRAvailable;				//0x0C
			bool                m_fMouseInitialized;				//0x0D
			bool                m_fMouseActive;						//0x0E
			bool                m_fJoystickAdvancedInit;			//0x0F
			char                pad_0x08[0x2C];						//0x10
			void* m_pKeys;											//0x3C
			char                pad_0x38[0x64];						//0x40
			int                 pad_0x41;							//0xA4
			int                 pad_0x42;							//0xA8
			bool                m_fCameraInterceptingMouse;			//0xAC
			bool                m_fCameraInThirdPerson;				//0xAD
			bool                m_fCameraMovingWithMouse;			//0xAE
			Vector              m_vecCameraOffset;					//0xAF	
			bool                m_fCameraDistanceMove;				//0xBB
			int                 m_nCameraOldX;						//0xBC
			int                 m_nCameraOldY;						//0xC0
			int                 m_nCameraX;							//0xC4
			int                 m_nCameraY;							//0xC8
			bool                m_CameraIsOrthographic;				//0xCC
			Vector              m_angPreviousViewAngles;			//0xCD
			Vector              m_angPreviousViewAnglesTilt;		//0xD9
			float               m_flLastForwardMove;				//0xE5
			int                 m_nClearInputState;					//0xE9
			char                pad_0xE4[0x8];						//0xED
			CUserCmd* m_pCommands;									//0xF5
			CVerifiedUserCmd* m_pVerifiedCommands;					//0xF9
		};
	}


	class CInput : public SourceInterface<SourceEngine::_CInput_>
	{
	public:
		CInput() {}

		virtual Interface* Init() override
		{
			m_data = *reinterpret_cast<Interface**>(
				Peb::Instance()->GetModule("client_panorama.dll").FindPattern("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1
			);
			SourceInterface<SourceEngine::_CInput_>::Init();
			return m_data;
		}
		SourceEngine::CUserCmd* CInput::GetUserCmd(int sequence_number)
		{
			auto commands = *(SourceEngine::CUserCmd**)((uintptr_t)m_data + 0xF4);
			return &commands[sequence_number % MULTIPLAYER_BACKUP];
		}

		SourceEngine::CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
		{
			auto commands = *(SourceEngine::CVerifiedUserCmd * *)((uintptr_t)m_data + 0xF8);
			return &commands[sequence_number % MULTIPLAYER_BACKUP];

		}

		bool ThirdPerson()
		{
			return *(bool*)((uintptr_t)m_data + 0xA5);
		}
	};
}