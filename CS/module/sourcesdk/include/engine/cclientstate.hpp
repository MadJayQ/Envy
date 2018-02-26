#pragma once

#include <cstdint>

namespace Envy
{
	namespace SourceEngine
	{
#pragma pack(push, 1)
		class INetChannel
		{
		public:
			char pad_0000[20];           //0x0000
			bool m_bProcessingMessages;  //0x0014
			bool m_bShouldDelete;        //0x0015
			char pad_0016[2];            //0x0016
			int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
			int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
			int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
			int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
			int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
			int32_t m_nChokedPackets;    //0x002C number of choked packets
			char pad_0030[1044];         //0x0030
		}; //Size: 0x0444

		class CClockDriftMgr
		{
		public:
			float m_ClockOffsets[16];   //0x0000
			uint32_t m_iCurClockOffset; //0x0044
			uint32_t m_nServerTick;     //0x0048
			uint32_t m_nClientTick;     //0x004C
		}; //Size: 0x0050
		class _CClientState_
		{
		public:
			void ForceFullUpdate()
			{
				//m_nDeltaTick = -1;
			}

			char _0x0000[156];
			INetChannel* m_nNetChannel; //0x009C 
			char _0x00A4[19444];
			float m_flLastServerTickTime; //0x4C98 
			bool insimulation; //0x4C9C 
			unsigned char unused; //0x4C9D 
			char _0x4C9E[2];
			__int32 oldtickcount; //0x4CA0 
			float m_tickRemainder; //0x4CA4 
			float m_frameTime; //0x4CA8 
			__int32 lastoutgoingcommand; //0x4CAC 
			__int32 last_command_ack; //0x4CB0 
			__int32 chokedcommands; //0x4CB4
			__int32 command_ack; //0x4CB8 
			__int32 m_nSoundSequence; //0x4CBC 
			bool ishltv; //0x4CC0 
			__int32 servercrc; //0x4CC1 
			__int32 serverClientSideDllCrc; //0x4CC5 
			char _0x4CC9[71];
			QAngle viewangles; //0x4D10 
			char _0x4D1C[1965];
		}; //Size: 0x4D1C
#pragma pack(pop)
	}

	class CClientState : public SourceInterface<SourceEngine::_CClientState_>
	{
	public:
		CClientState() {}

		virtual Interface* Init() override
		{
			m_data = **reinterpret_cast<Interface***>(
				Peb::Instance()->GetModule("engine.dll").FindPattern("A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1
			);
			SourceInterface<SourceEngine::_CClientState_>::Init();
			return m_data;
		}
	};
}