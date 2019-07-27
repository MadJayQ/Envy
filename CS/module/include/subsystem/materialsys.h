#pragma once

#pragma once

#include "subsystem.h"
#include "basehandle.hpp"

#include "csgosdk.h"

#include <d3d9.h>

namespace Envy
{
	//Forward a
	class MaterialSubsystem : public ICheatSubsystem
	{
	public:
		MaterialSubsystem()
		{

		}


		virtual void Init() override;

		SourceEngine::IMaterial* CreateMaterial(const std::string& materialName, const std::string& materialData);
	};
}