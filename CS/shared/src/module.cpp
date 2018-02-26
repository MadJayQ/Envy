#include "..\include\module.h"

#include "ntops.h"

#include <memory>
#include <iostream>

#include "memory\address.h"
#include "..\..\module\include\envydefs.hpp"

Module::Module(uintptr_t handle) : m_handle(handle)
{
	MemoryAddress module(handle);
	m_pDOSHeader = module->As<IMAGE_DOS_HEADER>();
	m_pNTHeaders = (module + m_pDOSHeader->e_lfanew)->As<IMAGE_NT_HEADERS32>();
	auto opt_header = m_pNTHeaders->OptionalHeader;
	auto export_entry = opt_header.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	if (export_entry.Size > 0)
	{
		auto export_directory = (module + export_entry.VirtualAddress)->As<IMAGE_EXPORT_DIRECTORY>();
		auto function_table = (module + export_directory->AddressOfFunctions)->As<Address>();
		auto ord_table = (module + export_directory->AddressOfNameOrdinals)->As<Ordinal>();
		auto name_table = (module + export_directory->AddressOfNames)->As<Address>();

		for (auto i = 0; i < export_directory->NumberOfNames; i++)
		{
			auto name = (module + name_table[i])->As<const char>();
			Address addr = (module + function_table[i]).ptr();
			exports.insert(
				std::make_pair(std::string(name), addr)
			);
		}
	}
}

std::uint8_t* Module::FindPattern(const char* sig)
{
	/*
	CACHED auto to_bytes = [](const char* pattern) mutable -> std::vector<int> { //MarkHC
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) 
		{
			if (*current == '?') 
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else 
			{
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto size = m_pNTHeaders->OptionalHeader.SizeOfImage;
	auto pattern_bytes = to_bytes(sig);
	auto data = reinterpret_cast<byte*>(m_handle);

	auto s = pattern_bytes.size();
	auto d = pattern_bytes.data();

	for (auto i = 0ul; i < size - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			auto src = data[i + j];
			auto dst = d[j];
			if (src != dst && dst != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return MemoryAddress(((uintptr_t)&data[i]) + offset);
		}
	}

	return MemoryAddress(-1);
	*/
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dosHeader = (PIMAGE_DOS_HEADER)m_handle;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)m_handle + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(sig);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(m_handle);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}
	return nullptr;
}
