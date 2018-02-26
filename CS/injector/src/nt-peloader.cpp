#include "nt-peloader.h"

#include "asmfactory.h"
#include "redirection-helpers.h"
#include "ntops.h"

#include <codecvt>
#include <iostream>
#include <sddl.h>
#include <ShlObj.h>
#include <rpc.h>
#include <fstream>

static bool base_dir_set;
static wchar_t base_dir[1024] = { 0 };

PELoader::PELoader(Process p) :
	target_process(p)
{
	base_dir_set = false;
}

PELoader::~PELoader()
{
}
uintptr_t GetRemoteProcAddr(const std::string& mod, const std::string& func, Process target_process)
{
	auto a = ASMFactory::GetAssembler();
	auto return_pointer = target_process.AllocateMemory(sizeof(uintptr_t));
	{
		a->PushInt(target_process.GetModule(mod));
		a->PushANSIString(func.c_str());
		a->PushCall(CSTDCALL, (FARPROC)GetProcAddress);
		//a->AddBytes({ 0x48, 0x89, 0xD0 });
		//a->AddBytes({ 0x48, 0xA3 });
		//a->AddLong((uintptr_t)return_pointer);
		a->AddByte(0xA3);
		a->AddLong(return_pointer);
		a->AddBytes({ 0xC2, 0x04, 0x00 });
	}
	a->ExecuteASM();
	uintptr_t ret;
	target_process.ReadMemory((void*)&ret, return_pointer, sizeof(uintptr_t));
	if (!target_process.FreeMemory(return_pointer))
	{
		std::cout << "ERROR: " << GetLastError();
	}
	a->FreeAllocatedBlocks();

	return ret;
}
HMODULE PELoader::InjectImage(const std::string & path)
{
	auto a = ASMFactory::GetAssembler();
	a->SetAssemblerTargetProcess(target_process);

	auto remote_ldr = GetRemoteProcAddr("ntdll.dll", "LdrLoadDll", target_process);
	std::wstring wpath = RedirectionHelper::Instance()->ConvertFromMBS(path);

	UNICODE_STRING path_ustr;
	RtlInitUnicodeString(&path_ustr, wpath.c_str());

	ULONG flags = 0;
	auto flags_ptr = a->CommitBlock((void*)&flags, sizeof(ULONG));
	auto return_pointer = target_process.AllocateMemory(sizeof(size_t));
	{
		a->PushInt(0);
		a->PushInt((uintptr_t)flags_ptr);
		a->PushUNICODEStringStructure(&path_ustr);
		a->PushInt(return_pointer);
		a->PushCall(CSTDCALL, (FARPROC)remote_ldr);
		a->AddBytes({ 0x8B, 0x02 });
		a->AddBytes({0xA3 });
		a->AddLong((uintptr_t)return_pointer);
		a->AddBytes({ 0x33, 0xc0 });
		a->AddBytes({ 0xC2, 0x04, 0x00 });
	}

	a->ExecuteASM();
	uintptr_t ret;
	target_process.ReadMemory((void*)&ret, return_pointer, sizeof(uintptr_t));
	if (!target_process.FreeMemory(return_pointer))
	{
		std::cout << "ERROR: " << GetLastError();
	}
	a->FreeAllocatedBlocks();
	//RtlFreeUnicodeString(&path_ustr);
	//target_process.UnlinkModuleFromPEB(ret);
	return (HMODULE)ret;
}

HMODULE PELoader::InjectImage(byte* buffer, const size_t size)
{
	if (!base_dir_set)
	{
		wchar_t* app_data_path = new wchar_t[1024];
		wchar_t envy_path[MAX_PATH] = { 0 };
		UUID folder_id;
		CoCreateGuid(&folder_id);
		swprintf_s(
			envy_path,
			ARRAYSIZE(envy_path),
			L"Local\\{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
			folder_id.Data1,
			folder_id.Data2,
			folder_id.Data3,
			folder_id.Data4[0],
			folder_id.Data4[1],
			folder_id.Data4[2],
			folder_id.Data4[3],
			folder_id.Data4[4],
			folder_id.Data4[5],
			folder_id.Data4[6],
			folder_id.Data4[7]
			);
		auto res = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &app_data_path); //This function fucking blows
		if (SUCCEEDED(res))
		{
			wchar_t* last_dir_slash = wcsrchr(app_data_path, L'\\');
			*(++last_dir_slash) = '\0';
			wcscpy_s(base_dir, app_data_path);
			wcscat_s(base_dir, envy_path);

			DWORD dir_attr = GetFileAttributesW(base_dir);
			if (dir_attr == INVALID_FILE_ATTRIBUTES)
			{
				bool b = CreateDirectoryW(base_dir, NULL);
				if (!b) { return 0; }
			}

			base_dir_set = true;
		}
	}
	if (!base_dir_set) return 0;
	char base_dir_mbs[MAX_PATH] = { 0 };
	wcstombs(base_dir_mbs, base_dir, MAX_PATH);
	auto tickcount = GetTickCount64();
	char random_filename[MAX_PATH];
	sprintf_s(random_filename, "\\%IX.dll\0", GetTickCount64());
	std::string filename(base_dir_mbs);
	filename += random_filename;
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::binary | std::ios::out);
	fout.write((const char*)buffer, size);
	fout.close();
	auto mod = InjectImage(filename);
	return mod;
}
