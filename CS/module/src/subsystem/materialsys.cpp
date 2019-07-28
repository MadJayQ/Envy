#include "..\..\include\subsystem\materialsys.h"

using namespace Envy::SourceEngine;

namespace Envy
{
	static constexpr const char* KeyValuesCtorSig = "55 8B EC 51 33 C0 C7 45 ? ? ? ? ? 56 8B F1 81 26 ? ? ? ? C6 46 03 00 89 46 10 89 46 18 89 46 14 89 46 1C 89 46 04 89 46 08 89 46 0C FF 15 ? ? ? ? 6A 01 FF 75 08 8D 4D FC 8B 10 51 8B C8 FF 52 24 8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03";
	static constexpr const char* LoadFromBufferSig = "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04";

	typedef void(__thiscall* InitKeyValuesFn)(void*, const char*);
	typedef void(__thiscall* LoadFromBufferFn)(void*, const char*, const char*, void*, const char*, void*, void*);

	static InitKeyValuesFn KeyValuesCtor = (InitKeyValuesFn)INVALID;
	static LoadFromBufferFn LoadFromBuffer = (LoadFromBufferFn)INVALID;

	void MaterialSubsystem::Init()
	{
		KeyValuesCtor = (InitKeyValuesFn)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern(KeyValuesCtorSig));
		LoadFromBuffer = (LoadFromBufferFn)(Peb::Instance()->GetModule("client_panorama.dll").FindPattern(LoadFromBufferSig));




	}

	IMaterial* MaterialSubsystem::CreateMaterial(const std::string& materialName, const std::string& materialData)
	{

		auto materialSystem = Interfaces::Instance()->GetInterface<IMaterialSystem>();
		KeyValues* values = new KeyValues(materialName.c_str());

		KeyValuesCtor(values, materialName.c_str());
		LoadFromBuffer(values, materialName.c_str(), materialData.c_str(), NULL, NULL, NULL, NULL);

		typedef void* (__thiscall * OriginalFn)(void*, int, int);
		auto func = VMTManager::Instance()->GetVirtualFunction<OriginalFn>(materialSystem->get(), 83);
		return (IMaterial*)func((void*)materialSystem->get(), (int)materialName.c_str(), (int)values);
	}
}

