#include <Windows.h>
#include <memory>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <ntops.h>
#include <locale>
#include <codecvt>
#include <Psapi.h>
#include <winnt.h>
#include <cctype>

//CREDITS: learn_more
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

struct ImportData_t
{
	std::string name;
	uintptr_t function_remote_va;
	uintptr_t ord = 0;
};

struct ModuleFileInfo
{
	byte* buffer;
	size_t size;
};

static bool icompare_pred(unsigned char a, unsigned char b)
{
	return std::tolower(a) == std::tolower(b);
}
static bool icompare(const std::string& a, const std::string& b)
{
	if (a.length() == b.length())
	{
		return std::equal(b.begin(), b.end(), a.begin(), icompare_pred);
	}
	else
	{
		return false;
	}
}
static inline std::string SearchDirectory(std::vector<std::string> dir_files, const std::string& image_name)
{
	for (auto&& file : dir_files)
	{
		std::string file_without_path = file;
		auto slash_idx = file_without_path.find_last_of("\\/");
		if (slash_idx != std::string::npos)
		{
			file_without_path.erase(0, slash_idx + 1);
		}
		if (icompare(file_without_path, image_name))
		{
			return file;
		}
	}
	return "";
}
static inline UNICODE_STRING WStringToUnicode(const std::wstring& str)
{
	UNICODE_STRING ret;
	const SIZE_T len = str.length();
	LPWSTR cstr = new WCHAR[len + 1];
	memcpy(cstr, str.c_str(), (len + 1) * sizeof(WCHAR));
	ret.Buffer = cstr;
	ret.Length = (USHORT)((len)* sizeof(WCHAR));
	ret.MaximumLength = (USHORT)((len + 1) * sizeof(WCHAR));
	return ret;
}

static inline void FreeUnicodeString(UNICODE_STRING& str)
{
	delete[] str.Buffer;
	str.Buffer = 0;
	str.Length = 0;
	str.MaximumLength = 0;
}

static inline std::string StripFilePath(const std::string& path)
{
	std::string file_without_path = path;
	const size_t last_slash_idx = file_without_path.find_last_of("\\/");
	file_without_path.erase(0, last_slash_idx + 1);
	return file_without_path;
}

static inline std::wstring StripFilePath(const std::wstring& path)
{
	std::wstring file_without_path = path;
	const size_t last_slash_idx = file_without_path.find_last_of(L"\\/");
	file_without_path.erase(0, last_slash_idx + 1);
	return file_without_path;
}

static inline std::vector<std::string> GetWindowsDirectories()
{
	std::vector<std::string> res;
	char buffer[MAX_PATH];
	GetWindowsDirectory(buffer, MAX_PATH);
	res.push_back(std::string(buffer));
	GetSystemDirectory(buffer, MAX_PATH);
	res.push_back(std::string(buffer));
	return res;
}

class Helpers
{
public:
	Helpers(Helpers const&) = delete;
	Helpers& operator=(Helpers const&) = delete;

	static Helpers* Instance()
	{
		static std::shared_ptr<Helpers> s_pInstance{ new Helpers };
		return s_pInstance.get();
	}

public:
	IMAGE_DOS_HEADER* GetImageHeader(byte* image)
	{
		return (IMAGE_DOS_HEADER*)(image);
	}
	IMAGE_NT_HEADERS* GetImageNTHeaders(byte* image, IMAGE_DOS_HEADER* dos_header)
	{
		return (IMAGE_NT_HEADERS*)(image + dos_header->e_lfanew);
	}
	std::vector<byte> ReadAllBytes(const char* filename)
	{
		std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
		std::ifstream::pos_type pos = ifs.tellg();

		std::vector<byte> res(pos);
		ifs.seekg(0, std::ios::beg);
		ifs.read(
			(char*)&res[0],
			pos
		);
		return res;
	}

	ModuleFileInfo ReadModuleFile(const std::string& path)
	{
		ModuleFileInfo ret = { 0 };

		auto file_handle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return ret;
		}
		if (GetFileAttributesA(path.c_str()) & FILE_ATTRIBUTE_COMPRESSED)
		{
			ret.size = GetCompressedFileSize(path.c_str(), NULL);
		}
		else
		{
			ret.size = GetFileSize(file_handle, NULL);
		}
		if (ret.size == 0)
		{
			CloseHandle(file_handle);
			return ret;
		}
		byte* alloc = (byte*)VirtualAlloc(NULL, ret.size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (alloc == NULL)
		{
			ret.size = 0;
			CloseHandle(file_handle);
			return ret;
		}
		DWORD bytes_read = 0;
		if (!ReadFile(file_handle, alloc, ret.size, &bytes_read, FALSE))
		{
			ret = { 0 };
		}
		else
		{
			ret.buffer = alloc;
		}

		CloseHandle(file_handle);
		return ret;

	}
	void BuildAPISchema()
	{

	}
	void BuildSystemFilepaths(std::vector<std::string> directories)
	{
		for (auto& dir : directories)
		{
			auto dlls = GetFiles(dir);
			for (auto& dll : dlls)
			{
				std::string dll_without_path = StripFilePath(dll);
				if (dll_file_paths.find(dll_without_path) == dll_file_paths.end())
				{
					dll_file_paths[dll_without_path] = dll;
				}
			}
		}
	}

	std::unordered_map<std::string, std::vector<ImportData_t>> PE_GetImportList(uintptr_t image_base, IMAGE_OPTIONAL_HEADER64* optional_header)
	{
		std::unordered_map<std::string, std::vector<ImportData_t>> result;
		auto section = optional_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (section.Size == 0) return result;
		auto import_table = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(image_base + section.VirtualAddress);

		for (; import_table->Name; ++import_table)
		{
			auto module_name = std::string(reinterpret_cast<char*>(image_base + (uintptr_t)import_table->Name));
			std::transform(module_name.begin(), module_name.end(), module_name.begin(), ::tolower);

			auto entry = reinterpret_cast<IMAGE_THUNK_DATA64*>(image_base + import_table->OriginalFirstThunk);
			for (uintptr_t index = 0; entry->u1.AddressOfData; index += sizeof(uintptr_t), ++entry)
			{
				auto import_by_name = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(image_base + entry->u1.AddressOfData);

				ImportData_t data;
				data.function_remote_va = import_table->FirstThunk + index;

				if (entry->u1.Ordinal < IMAGE_ORDINAL_FLAG64 && import_by_name->Name[0])
					data.name = reinterpret_cast<const char*>(import_by_name->Name); // IMPORT BY NAME
				else
					data.ord = IMAGE_ORDINAL64(entry->u1.AddressOfData); // IMPORT BY ORDINAL

				result[module_name].emplace_back(data);
			}
		}

		return result;
	}
	uintptr_t FindSignature(const std::string& module, const std::string& pattern)
	{
		MODULEINFO module_info;
		GetModuleInformation(
			GetCurrentProcess(),
			GetModuleHandle(module.c_str()),
			&module_info,
			sizeof(MODULEINFO)
		);
		uintptr_t start_addr = reinterpret_cast<uintptr_t>(module_info.lpBaseOfDll);
		uintptr_t end_addr = start_addr + module_info.SizeOfImage;
		const char* pattern_ptr = pattern.c_str();
		uintptr_t first_match_addr = 0;
		for (
			auto current_addr = start_addr;
			current_addr < end_addr;
			current_addr++
			)
		{
			if (!*pattern_ptr) return first_match_addr;
			if (*(PBYTE)(pattern_ptr) == '\?' || *(BYTE*)current_addr == getByte(pattern_ptr))
			{
				if (!first_match_addr) first_match_addr = current_addr;
				if (!pattern_ptr[2]) return first_match_addr;
				if (*(PWORD)pattern_ptr == '\?\?' || *(PBYTE)pattern_ptr != '\?') pattern_ptr += 3;
				else pattern_ptr += 2;
			}
			else
			{
				pattern_ptr = pattern.c_str();
				first_match_addr = 0;
			}
		}
		return 0;
	}

	std::string FindDll(const std::string& image_name)
	{
		if (dll_file_paths.empty())
			BuildSystemFilepaths(GetWindowsDirectories());
		return dll_file_paths[image_name];
	}
	std::vector<std::string> GetFiles(const std::string& directory)
	{
		std::vector<std::string> files;
		std::string search_string = directory + "/*.dll";
		WIN32_FIND_DATA find_data;
		HANDLE find_handle = FindFirstFile(search_string.c_str(), &find_data);
		if (find_handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					files.push_back(directory + "\\" + find_data.cFileName);
			} while (FindNextFile(find_handle, &find_data));
			FindClose(find_handle);
		}
		return files;
	}
	/*
		Function: MirrorRemoteSection
		Purpose: This function is pretty cool, basically we created a remote memory section in our target process
		and now we're going to create two mappings to the same  
	*/
	void MirrorRemoteSection(
		HANDLE target_process,
		HANDLE remote_section, 
		SIZE_T size,
		void* &remote_image, 
		void* &local_image
	)
	{
		void* test_adr = nullptr;
		NTSTATUS status = NtMapViewOfSection(
			remote_section,
			GetCurrentProcess(),
			&local_image,
			0,
			0,
			0,
			&size,
			SECTION_INHERIT::ViewUnmap,
			0,
			PAGE_EXECUTE_READWRITE
		);
		if (!NT_SUCCESS(status))
		{
			throw "Failed to map local image";
			return;
		}
		status = NtMapViewOfSection(
			remote_section,
			target_process,
			&remote_image,
			0,
			0,
			0,
			&size,
			SECTION_INHERIT::ViewUnmap,
			0,
			PAGE_EXECUTE_READWRITE
			);
		if (!NT_SUCCESS(status))
		{
			throw "Failed to map remote image";
			return;
		}
 	}

private:
	Helpers() {}

private:
	std::unordered_map<std::wstring, std::vector<std::wstring>> api_schema_map;
	std::unordered_map<std::string, std::string> dll_file_paths;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

};