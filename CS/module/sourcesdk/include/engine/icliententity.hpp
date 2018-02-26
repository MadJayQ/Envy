#pragma once

#include "clientnetworkable.hpp"
#include "iclientrenderable.hpp"
#include "iclientunknown.hpp"
#include "iclientthinkable.hpp"

namespace Envy
{
	namespace SourceEngine
	{
		struct SpatializationInfo_t;

		class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
		{
		public:
			virtual void Release(void) = 0;
		};
	}
}