#pragma once


#include <stdint.h>

namespace Envy
{
	namespace SourceEngine
	{
		class AnimationLayer
		{
		public:
			char  pad_0000[20];
			uint32_t m_nOrder; //0x0014
			uint32_t m_nSequence; //0x0018
			float_t m_flPrevCycle; //0x001C
			float_t m_flWeight; //0x0020
			float_t m_flWeightDeltaRate; //0x0024
			float_t m_flPlaybackRate; //0x0028
			float_t m_flCycle; //0x002C
			void *m_pOwner; //0x0030 // player's thisptr
			char  pad_0038[4]; //0x0034
		}; //Size: 0x0038

		class CCSGOPlayerAnimState
		{
		public:
			char _0x0000[232];
			__int32 unused; //0x00E8 
			float m_flSpeed; //0x00EC 
			char _0x00F0[604];

		};//Size=0x034
	}
}