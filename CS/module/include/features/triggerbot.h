#pragma once

#include "csgosdk.h"
#include "structs.h"

namespace Envy
{
	class Triggerbot
	{
	public:
		Triggerbot() {}

		bool ProcessTriggerBot(SourceEngine::QAngle safeAngle, bool& sendpacket);
	};
}