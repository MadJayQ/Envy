#include "redirection-helpers.h"

#include <iostream>
#include <algorithm>

#include "helpers.hpp"

RedirectionHelper::RedirectionHelper()
{
	Internal_Initialize<PAPI_SET_NAMESPACE_ARRAY_10, PAPI_SET_NAMESPACE_ENTRY_10, PAPI_SET_VALUE_ARRAY_10, PAPI_SET_VALUE_ENTRY_10>();
}

static bool FileExists(const std::wstring& path)
{
	return (GetFileAttributesW(path.c_str()) != 0xFFFFFFFF);
}

template<typename PApiSet, typename PApiEntry, typename PHost, typename PHostEntry>
bool RedirectionHelper::Internal_Initialize()
{
	if (!api_schema.empty())
		return true;
	PROCESS_BASIC_INFORMATION pbi;
	ULONG ret_length;
	PApiSet set_map = NULL;
	if (!NT_SUCCESS(NtQueryInformationProcess(
		GetCurrentProcess(),
		ProcessBasicInformation,
		&pbi,
		sizeof(PROCESS_BASIC_INFORMATION),
		&ret_length
		)))
	{
		return false;
	}

	PPEB peb = (PPEB)pbi.PebBaseAddress;
	set_map = reinterpret_cast<PApiSet>(peb->ApiSetMap);
	for (auto i = 0; i < set_map->Count; i++)
	{
		PApiEntry desc = set_map->entry(i);
		std::vector<std::string> hosts;
		wchar_t dll_name[MAX_PATH] = { 0 };
		auto name_size = set_map->apiName(desc, dll_name);
		std::transform(dll_name, dll_name + name_size / sizeof(wchar_t), dll_name, ::tolower);

		PHost host_data = set_map->valArray(desc);
		for (auto j = 0; j < host_data->Count; j++)
		{
			PHostEntry host = host_data->entry(set_map, j);
			std::wstring host_name(
				reinterpret_cast<wchar_t*>(reinterpret_cast<uint8_t*>(set_map) + host->ValueOffset),
				host->ValueLength / sizeof(wchar_t)
			);

			if (!host_name.empty())
			{
				hosts.push_back(converter.to_bytes(host_name));
			}
		}
		std::string dll_name_converted = converter.to_bytes(std::wstring(dll_name));
		api_schema.emplace(dll_name_converted, std::move(hosts));
	}
	return true;
}


NTSTATUS RedirectionHelper::Resolvepath(std::string & path, const std::string & name, const std::string & search_directory, Process & proc, HANDLE actx)
{
	NTSTATUS status = 0;
	return 0;
}

uintptr_t RedirectionHelper::ResolvePath(std::wstring& path, ResolveFlags flags, Process& proc, const std::wstring& baseName)
{
	wchar_t tmp_path[4096] = { 0 };
	std::wstring complete_path;

	std::transform(path.begin(), path.end(), path.begin(), ::tolower);
	std::wstring filename = StripFilePath(path);

	if (filename.find(L"ext-ms") == 0)
		filename.erase(0, 4);

	auto itr = api_schema.find(converter.to_bytes(filename));
	if (itr != api_schema.end())
	{
		if (itr->second.front() != converter.to_bytes(baseName))
		{
			path = converter.from_bytes(itr->second.front());
		}
		else
		{
			path = converter.from_bytes(itr->second.back());
		}

		if (NT_SUCCESS(ProbeSxSRedirect(path)))
		{
			return 0;
		}
		else if (flags & EnsureFullPath)
		{
			wchar_t sys_path[255] = { 0 };
			GetSystemDirectoryW(sys_path, 255);
			path = sys_path + path;
		}

		return 0;
	}

	if (flags & ApiSchemaOnly)
	{
		return ERROR_NOT_FOUND;
	}

	auto status = ProbeSxSRedirect(path);
	if (status == ERROR_SUCCESS)
		return ERROR_SUCCESS;
	
	HKEY key = NULL;
	LRESULT res = 0;
	res = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\KnownDLLs", &key);
	if (!SUCCEEDED(res))
		return -1;
	for (int i = 0; i < 0x100 & res == ERROR_SUCCESS; i++)
	{
		wchar_t value_name[255] = { 0 };
		wchar_t value_data[255] = { 0 };

		DWORD size = 25;
		DWORD type = 0;

		res = RegEnumValueW(key, i, value_name, &size, NULL, &type, (LPBYTE)value_data, &size);

		if (_wcsicmp(value_data, filename.c_str()) == 0)
		{
			wchar_t sys_path[255] = { 0 };
			size = 255;

			GetSystemDirectoryW(sys_path, size);
			if (res == ERROR_SUCCESS)
			{
				path = std::wstring(sys_path) + L"\\" + value_data;

				RegCloseKey(key);
				return ERROR_SUCCESS;
			}
		}
	}

	RegCloseKey(key);

	complete_path = proc.Directory();
	if (FileExists(complete_path))
	{
		path = complete_path;
		return ERROR_SUCCESS;
	}

	GetSystemDirectoryW(tmp_path, ARRAYSIZE(tmp_path));
	complete_path = std::wstring(tmp_path) + L"\\" + filename;
	if (FileExists(complete_path))
	{
		path = complete_path;
		return ERROR_SUCCESS;
	}

	GetWindowsDirectoryW(tmp_path, ARRAYSIZE(tmp_path));
	complete_path = std::wstring(tmp_path) + L"\\" + filename;
	if (FileExists(complete_path))
	{
		path = complete_path;
		return ERROR_SUCCESS;
	}

	GetCurrentDirectoryW(ARRAYSIZE(tmp_path), tmp_path);
	complete_path = std::wstring(tmp_path) + L"\\" + filename;
	if (FileExists(complete_path))
	{
		path = complete_path;
		return ERROR_SUCCESS;
	}

	wchar_t* context;
	GetEnvironmentVariableW(L"PATH", tmp_path, ARRAYSIZE(tmp_path));
	{
		for (wchar_t* dir = wcstok_s(tmp_path, L";", &context); dir; dir = wcstok_s(context, L";", &context))
		{
			complete_path = std::wstring(dir) + L"\\" + filename;
			if (FileExists(path))
			{
				path = complete_path;
				return ERROR_SUCCESS;
			}
		}
	}

	return ERROR_NOT_FOUND;
}

uintptr_t RedirectionHelper::ProbeSxSRedirect(std::wstring & path)
{
	UNICODE_STRING original = { 0 };
	UNICODE_STRING dll_name1 = { 0 };
	UNICODE_STRING dll_name2 = { 0 };
	PUNICODE_STRING path_ptr = nullptr;
	ULONG_PTR cookie = 0;
	wchar_t buf[255] = { 0 };

	RtlInitUnicodeString(&original, path.c_str());

	dll_name1.Buffer = buf;
	dll_name1.Length = NULL;
	dll_name1.MaximumLength = ARRAYSIZE(buf);

	auto status = RtlDosApplyFileIsolationRedirection_Ustr(
		TRUE,
		&original,
		NULL,
		&dll_name1,
		&dll_name2,
		&path_ptr,
		NULL,
		NULL,
		NULL
	);

	if (NT_SUCCESS(status))
	{
		path = std::wstring(path_ptr->Buffer);
	}
	else
	{
		if (dll_name2.Buffer)
			RtlFreeUnicodeString(&dll_name2);
		return RtlNtStatusToDosError(status);
	}

	if (dll_name2.Buffer)
		RtlFreeUnicodeString(&dll_name2);
	return ERROR_SUCCESS;
}

