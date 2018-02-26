#pragma once
#include "singleton.hpp"
#include "ntops.h"
#include "module.h"

class Peb : public Singleton<Peb>
{

	using ModuleMap = std::unordered_map<std::string, Module>;
public:
	Peb();
	void UnlinkModule(uintptr_t module);
	ModuleMap Modules() const { return m_modules; }

	Module GetModule(std::string name) { return m_modules[name]; }

private:
	PPEB m_peb;
	PPEB_LDR_DATA m_pebldr;
	ModuleMap m_modules;
};