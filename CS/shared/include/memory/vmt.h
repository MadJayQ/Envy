#pragma once
#include "rtti.hpp"

#include <unordered_map>
#include <array>

using VMTHookEntry = std::array<uintptr_t, 2>; //(old, new)
using VMTHookMap = std::unordered_map<uintptr_t, VMTHookEntry>;

class VMT
{
public:
	VMT(void* object);
	VMT() {}
	~VMT();

	uintptr_t HookFunction(uintptr_t idx, uintptr_t addr);
	void UnhookFunction(uintptr_t idx);

	VMTHookEntry GetEntry(uintptr_t idx) { return m_functionmap[idx]; }

private:
	void* m_object;
	size_t m_length;
	uintptr_t*  m_vmtbackup;
	uintptr_t*  m_vmtnew;

	RTTICompleteObjectLocator* m_rtti;

	VMTHookMap m_functionmap;
};
