#pragma once
#include <stdint.h>
#include <unordered_map>

#include "address.h"

class Module
{
public:
	Module(uintptr_t handle);
	Module() {}

	uintptr_t GetHandle() const { return m_handle; }
	uintptr_t Export(const std::string& name) { return exports[name]; }

	std::uint8_t* FindPattern(const char* sig);

private:
	std::unordered_map<std::string, uintptr_t> exports;
	uintptr_t m_handle;

	IMAGE_DOS_HEADER* m_pDOSHeader;
	IMAGE_NT_HEADERS* m_pNTHeaders;
};