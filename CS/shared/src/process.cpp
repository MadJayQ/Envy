#include "process.h"
#include "ntops.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <algorithm>


#include <memory>
#include <iostream>

Process::Process(const char* name) :
	process_name(name)
{
	process_id = FindProcessID(name);
}

Process::Process(const char* name, HANDLE handle) : 
	process_name(name), 
	process_handle(handle)
{
	process_id = FindProcessID(name);
}

Process::Process(HANDLE handle)
{
	process_id = GetProcessId(handle);
	process_handle = handle;
}

Process::~Process()
{
}

uint32_t Process::FindProcessID(const char* name)
{
	//Take a snapshot of all open processes
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (snapshot == NULL)
	{
		return -1;
	}
	if (Process32First(snapshot, &entry))
	{
		do
		{
			if (!strcmp(entry.szExeFile, name))
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		} while (Process32Next(snapshot, &entry));
	}

	return 0;
}

std::unordered_map<std::string, uintptr_t> Process::GetModules()
{
	std::unordered_map<std::string, uintptr_t> result;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	if (!snapshot) return result;
	if (Module32First(snapshot, &entry))
	{
		do
		{
			char module_name[MAX_PATH];
			GetModuleBaseName(
				process_handle,
				entry.hModule,
				module_name,
				MAX_PATH
			);
			std::string new_name = module_name;
			std::transform(
				new_name.begin(),
				new_name.end(),
				new_name.begin(),
				::tolower
			);
			result[new_name] = reinterpret_cast<uintptr_t>(entry.hModule);
		} while (Module32Next(snapshot, &entry));
	}
	return result;
}

uintptr_t Process::AllocateMemory(const size_t size, const uintptr_t address, uint32_t access)
{
	return reinterpret_cast<uintptr_t>(VirtualAllocEx(process_handle, reinterpret_cast<LPVOID>(address), size, MEM_COMMIT, access));
}

HANDLE Process::CreateThread(const uintptr_t address, const uintptr_t argument)
{
	return CreateRemoteThread(process_handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), reinterpret_cast<LPVOID>(argument), 0, nullptr);
}

bool Process::ReadMemory(void * buf, const uintptr_t addr, const SIZE_T size)
{
	return ReadProcessMemory(this->process_handle, reinterpret_cast<LPCVOID>(addr), buf, size, nullptr);
}

bool Process::WriteMemory(void * buf, const uintptr_t addr, const SIZE_T size)
{
	return WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(addr), buf, size, nullptr);
}

bool Process::WriteMemory(void* buf, uintptr_t& addr, const SIZE_T size, bool inc_addr)
{
	bool ret = WriteMemory(buf, addr, size);
	if (inc_addr)
		addr += size;
	return ret;
}

bool Process::FreeMemory(const uintptr_t address)
{
	return VirtualFreeEx(process_handle, reinterpret_cast<LPVOID>(address), NULL, MEM_RELEASE);
}
bool Process::FreeMemory(const uintptr_t address, const SIZE_T size)
{
	return VirtualFreeEx(process_handle, reinterpret_cast<LPVOID>(address), size, MEM_RELEASE);
}

uintptr_t Process::GetModuleExport(uintptr_t module_handle, const char * function_ordinal)
{
	IMAGE_DOS_HEADER dos_header;
	IMAGE_NT_HEADERS64 nt_header;
	RPM_Wrapper(&dos_header, module_handle);
	RPM_Wrapper(&nt_header, module_handle + dos_header.e_lfanew);

	auto export_base = nt_header.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	auto export_base_size = nt_header.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	if (export_base) // CONTAINS EXPORTED FUNCTIONS
	{
		std::unique_ptr<IMAGE_EXPORT_DIRECTORY> export_data_raw(reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(malloc(export_base_size)));
		auto export_data = export_data_raw.get();

		// READ EXPORTED DATA FROM TARGET PROCESS FOR LATER PROCESSING
		if (!ReadMemory(export_data, module_handle + export_base, export_base_size))
		{
			std::cout << "failed to read export data" << std::endl;
			return 0;
		}

		// BLACKBONE PASTE, NEVER EXPERIENCED THIS BUT WHO KNOWS?
		if (export_base_size <= sizeof(IMAGE_EXPORT_DIRECTORY))
		{
			export_base_size = static_cast<DWORD>(export_data->AddressOfNameOrdinals - export_base
				+ max(export_data->NumberOfFunctions, export_data->NumberOfNames) * 255);

			export_data_raw.reset(reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(malloc(export_base_size)));
			export_data = export_data_raw.get();

			if (!ReadMemory(export_data, module_handle + export_base, export_base_size))
			{
				std::cout << "failed to read export data" << std::endl; 
				return 0;
			}
		}

		// GET DATA FROM READ MEMORY
		auto delta = reinterpret_cast<uintptr_t>(export_data) - export_base;
		auto address_of_ordinals = reinterpret_cast<WORD*>(export_data->AddressOfNameOrdinals + delta);
		auto address_of_names = reinterpret_cast<DWORD*>(export_data->AddressOfNames + delta);
		auto address_of_functions = reinterpret_cast<DWORD*>(export_data->AddressOfFunctions + delta);

		// NO EXPORTED FUNCTIONS? DID WE FUCK UP?
		if (export_data->NumberOfFunctions <= 0)
		{
			std::cout << "No exports found!" << std::endl; 
			return 0;
		}

		auto ptr_function_ordinal = reinterpret_cast<uintptr_t>(function_ordinal);

		for (size_t i = 0; i < export_data->NumberOfFunctions; i++)
		{
			WORD ordinal;
			std::string function_name;
			auto is_import_by_ordinal = ptr_function_ordinal <= 0xFFFF;

			// GET EXPORT INFORMATION
			ordinal = static_cast<WORD>(is_import_by_ordinal ? i : address_of_ordinals[i]);
			function_name = reinterpret_cast<char*>(address_of_names[i] + delta);

			// IS IT THE FUNCTION WE ASKED FOR?
			auto found_via_ordinal = is_import_by_ordinal && static_cast<WORD>(ptr_function_ordinal) == (ordinal + export_data->Base);
			auto found_via_name = !is_import_by_ordinal && function_name == function_ordinal;

			if (found_via_ordinal || found_via_name)
			{
				auto function_pointer = module_handle + address_of_functions[ordinal];

				// FORWARDED EXPORT?
				// IF FUNCTION POINTER IS INSIDE THE EXPORT DIRECTORY, IT IS *NOT* A FUNCTION POINTER!
				// FUCKING SHIT MSVCP140 FUCK YOU
				if (function_pointer >= module_handle + export_base && function_pointer <= module_handle + export_base + export_base_size)
				{
					char forwarded_name[255] = { 0 };
					ReadMemory(forwarded_name, function_pointer, sizeof(forwarded_name));

					std::string forward(forwarded_name);
					std::string library_name = forward.substr(0, forward.find(".")) + ".dll";
					function_name = forward.substr(forward.find(".") + 1, function_name.npos);

					// LOWERCASE THANKS
					std::transform(library_name.begin(), library_name.end(), library_name.begin(), ::tolower);

					auto modules = GetModules();
					auto search = modules.find(library_name);
					if (search != modules.end())
						return GetModuleExport(search->second, function_name.c_str());
					else
						std::cout << "Forwarded module not loaded" << std::endl; // TODO: HANDLE THIS? WHO CARES 
				}

				return function_pointer;
			}
		}
	}
}

void Process::UnlinkModuleFromPEB(uintptr_t module_handle)
{
	PROCESS_BASIC_INFORMATION* pbi;
	PEB peb;
	PEB_LDR_DATA peb_ldr;

	auto heap = GetProcessHeap();
	auto size = sizeof(PROCESS_BASIC_INFORMATION);
	pbi = (PROCESS_BASIC_INFORMATION*)(HeapAlloc(heap, HEAP_ZERO_MEMORY, size));
	ULONG size_needed = 0;
	auto status = NtQueryInformationProcess(process_handle, ProcessBasicInformation, pbi, size, &size_needed);
	if (!NT_SUCCESS(status) && size < size_needed)
	{
		if (pbi)
		{
			HeapReAlloc(heap, HEAP_ZERO_MEMORY, pbi, size_needed);
		}

		status = NtQueryInformationProcess(process_handle, ProcessBasicInformation, pbi, size_needed, &size_needed);
	}

	if (!NT_SUCCESS(status))
	{
		if (pbi)
		{
			HeapFree(heap, 0, pbi);
			return;
		}
	}

	if (pbi->PebBaseAddress)
	{
		if (!RPM_Wrapper(&peb, (uintptr_t)pbi->PebBaseAddress) || !RPM_Wrapper(&peb_ldr, (uintptr_t)peb.Ldr))
		{
			auto err = GetLastError();
			if (pbi) HeapFree(heap, 0, pbi);
			return;
		}

	}
	LIST_ENTRY* curr = peb_ldr.InMemoryOrderModuleList.Flink;
	LDR_DATA_TABLE_ENTRY* entry = { 0 };
	while (curr != &peb_ldr.InMemoryOrderModuleList && curr != NULL)
	{
		entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		if ((uintptr_t)entry->BaseAddress == module_handle)
		{
			entry->InMemoryOrderLinks.Flink->Blink = entry->InMemoryOrderLinks.Blink;
			entry->InMemoryOrderLinks.Blink->Flink = entry->InMemoryOrderLinks.Flink;
			break;
		}
		curr = curr->Flink;
	}
}

uintptr_t Process::RemoteGetModuleHandle(const char * module)
{
	uintptr_t module_handle;
	PROCESS_BASIC_INFORMATION* pbi;
	PEB peb;
	PEB_LDR_DATA peb_ldr;

	auto heap = GetProcessHeap();
	auto size = sizeof(PROCESS_BASIC_INFORMATION);
	pbi = (PROCESS_BASIC_INFORMATION*)(HeapAlloc(heap, HEAP_ZERO_MEMORY, size));

	ULONG size_needed = 0;
	auto status = NtQueryInformationProcess(process_handle, ProcessBasicInformation, pbi, size, &size_needed);
	if (!NT_SUCCESS(status) && size < size_needed)
	{
		if (pbi)
		{
			HeapReAlloc(heap, HEAP_ZERO_MEMORY, pbi, size_needed);
		}

		status = NtQueryInformationProcess(process_handle, ProcessBasicInformation, pbi, size_needed, &size_needed);
	}

	if (!NT_SUCCESS(status))
	{
		if (pbi)
		{
			HeapFree(heap, 0, pbi);
			return 0;
		}
	}

	if (pbi->PebBaseAddress)
	{
		if (!ReadMemory((void*)&peb, (uintptr_t)pbi->PebBaseAddress, sizeof(peb)))
		{
			auto err = GetLastError();
			return NULL;

		}
		if (!ReadMemory((void*)&peb_ldr, (uintptr_t)peb.Ldr, sizeof(peb_ldr))) return NULL;
		auto ldr_list_head = (LIST_ENTRY*)peb_ldr.InMemoryOrderModuleList.Flink;
		auto ldr_current_node = ldr_list_head;
		do
		{
			LDR_DATA_TABLE_ENTRY entry = { 0 };
			if (!ReadMemory((void*)&entry, (uintptr_t)ldr_current_node, sizeof(entry)))
				break;
			ldr_current_node = entry.InMemoryOrderLinks.Flink;
			wchar_t wcs_dll_name[MAX_PATH] = { 0 };
			char mbs_dll_name[MAX_PATH] = { 0 };
			if (entry.FullDllName.Length > 0)
			{
				if (!ReadMemory((void*)wcs_dll_name, (uintptr_t)entry.FullDllName.Buffer, entry.FullDllName.Length)) break;
				wcstombs_s(NULL, mbs_dll_name, wcs_dll_name, MAX_PATH);
			}
			if (entry.BaseAddress)
			{
				if (_stricmp(mbs_dll_name, module) == 0)
				{
					module_handle = (uintptr_t)entry.BaseAddress;
					break;
				}
			}
		} while (ldr_list_head != ldr_current_node);
	}

	if (pbi) HeapFree(heap, 0, pbi);

	return module_handle;
}

std::wstring Process::Directory()
{
	std::wstring ret;
	PEB peb;
	PEB_LDR_DATA peb_ldr;

	auto peb_address = GetPEBAddress();

	if (peb_address)
	{
		if (!ReadMemory((void*)&peb, (uintptr_t)peb_address, sizeof(peb))); return ret;
		if (!ReadMemory((void*)&peb_ldr, (uintptr_t)peb.Ldr, sizeof(peb_ldr))) return ret;
		auto ldr_list_head = (LIST_ENTRY*)peb_ldr.InMemoryOrderModuleList.Flink;
		auto ldr_current_node = ldr_list_head;
		LDR_DATA_TABLE_ENTRY entry = { 0 };
		if (!ReadMemory((void*)&entry, (uintptr_t)ldr_current_node, sizeof(entry)))
			return ret;
		ldr_current_node = entry.InMemoryOrderLinks.Flink;
		wchar_t wcs_dll_name[MAX_PATH] = { 0 };
		if (entry.FullDllName.Length > 0)
		{
			if (!ReadMemory((void*)&wcs_dll_name, (uintptr_t)entry.FullDllName.Buffer, entry.FullDllName.Length))
			{
				wchar_t* end = 0;
				end = wcsrchr(wcs_dll_name, L'\\');
				if (!end) end = wcsrchr(wcs_dll_name, L'/');
				*end++ = L'\0';

				ret = std::wstring(wcs_dll_name);
			}
		}
	}
	return ret;
}

void Process::Attach(DWORD access)
{
	//Lol 
	process_handle = OpenProcess(access, FALSE, process_id);
	loaded_modules = GetModules();
}

void Process::Detach()
{
	if (process_handle != NULL)
	{
		CloseHandle(process_handle);
		process_handle = NULL;
	}
}

/*
	Function: AllocateSection
	Purpose: BattlEye fucking hooked NtAllocateVirtualMemory, we need to manually create a memory section in our target process with NtCreateSection
	If we had a driver loaded in kernel mode we could use ZwAllocateVirtualMemory.
	Returns: Handle to our allocated section (hopefully) with ALL_ACCES and EXECUTE_RW permissions
*/

uintptr_t Process::GetPEBAddress()
{
	PROCESS_BASIC_INFORMATION* pbi;
	auto heap = GetProcessHeap();
	auto size = sizeof(PROCESS_BASIC_INFORMATION);
	pbi = reinterpret_cast<PROCESS_BASIC_INFORMATION*>(HeapAlloc(heap, HEAP_ZERO_MEMORY, size));
	ULONG size_needed = 0;
	auto status = NtQueryInformationProcess(
		process_handle,
		ProcessBasicInformation,
		pbi,
		size,
		&size_needed
	);
	if (!NT_SUCCESS(status) && size < size_needed)
	{
		HeapReAlloc(heap, 0, pbi, size_needed);
		status = NtQueryInformationProcess(
			process_handle,
			ProcessBasicInformation,
			pbi,
			size_needed,
			&size_needed
		);
	}
	if (!NT_SUCCESS(status) && pbi)
	{
		HeapFree(heap, 0, pbi);
		return NULL;
	}

	return (uintptr_t)pbi->PebBaseAddress;
}

HANDLE Process::AllocateSection(unsigned long long size)
{

	HANDLE handle;
	LARGE_INTEGER section_size;
	section_size.QuadPart = size;
	NTSTATUS status = NtCreateSection(
		&handle, 
		SECTION_ALL_ACCESS, 
		0,
		&section_size,
		PAGE_EXECUTE_READWRITE, 
		SEC_COMMIT, 
		0
	);
	if (!NT_SUCCESS(status))
	{
		throw "Failed to allocate section";
		return NULL;
	}
	return handle;
}

std::ostream & operator<<(std::ostream& os, const Process& process)
{

	ACCESS_MASK mask = QueryHandleAccessRights(process.process_handle);
	/*
		ToDo(Jake):
		This is not actually correct, we only care about the permissions VM_READ and VM_WRITE
		While Full control is ideal, we only need Read & Write perms to inject our image
		We do not want to create a thread, so PROCESS_CREATE_THREAD is not needed, BattlEye hooks the Win32 API CreateRemoteThread
		We need to hijack a thread with correct Read/Write access to inject our shellcode
	*/
	const char*	ace_string = (mask == PROCESS_ALL_ACCESS) ? "(Full Control) - Successs!" : "INSUFFICIENT PERMISSIONS DETECTED!!!!!";
	os << "==============PROCESS INFORMATION==============" << std::endl;
	os << "NAME: " << process.process_name << std::endl;
	os << "PID: " << process.process_id << std::endl;
	os << "Handle Information:" << std::endl;
	os << "Handle: " << process.process_handle << std::endl;
	os << "Granted Access: " << "0x" << std::hex << mask << std::dec << " " << ace_string << std::endl;
	os << "============END PROCESS INFORMATION=============" << std::endl;

	return os;
}
