#include "manualmap.h"
#include "helpers.hpp"

#include <codecvt>
#include <iostream>
#include <sddl.h>
#include <DbgHelp.h>
#include <ShlObj.h>
#include <rpc.h>

#include "asmfactory.h"
#include "redirection-helpers.h"


#pragma comment(lib, "DbgHelp.lib")



std::unordered_map<std::string, module_ctx> mapped_modules;
static bool base_dir_set;
static wchar_t base_dir[1024] = { 0 };

inline uintptr_t ResolveRVA(uintptr_t va, byte* image)
{
	auto image_header = Helpers::Instance()->GetImageHeader(image);
	auto nt_header = Helpers::Instance()->GetImageNTHeaders(image, image_header);
	return (uintptr_t)::ImageRvaToVa(nt_header, image, va, 0);
}

ManualMap::ManualMap(Process p)
{
	ctx.local_image = NULL;
	ctx.remote_image = NULL;
	target_process = p;
	base_dir_set = false;
}

ManualMap::~ManualMap()
{
}

void ManualMap::ImageRelocations(IMAGE_NT_HEADERS* nt_headers, module_ctx & mod_ctx)
{
	if (nt_headers->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
	{
		return;
	}
	auto image_base_delta = (size_t)((uintptr_t)mod_ctx.remote_image - nt_headers->OptionalHeader.ImageBase);
	auto reloc_dir = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	auto reloc_size = reloc_dir.Size;

	if (!reloc_size) return;
	PIMAGE_BASE_RELOCATION relocation_directory = (PIMAGE_BASE_RELOCATION)ResolveRVA(reloc_dir.VirtualAddress, mod_ctx.image);
	if (!relocation_directory) return;
	void* relocation_end = (byte*)(relocation_directory)+reloc_size;
	while (relocation_directory < relocation_end)
	{
		byte* reloc_base = (byte*)ResolveRVA(relocation_directory->VirtualAddress, mod_ctx.image);
		uintptr_t num_relocs = (relocation_directory->SizeOfBlock - 8) >> 1;
		WORD* reloc_data = (WORD*)(relocation_directory + 1);
		for (auto i = 0; i < num_relocs; ++i, ++reloc_data)
		{
			auto type = ((*reloc_data >> 12) & 0xF);
			switch (type)
			{
			case IMAGE_REL_BASED_HIGH:
			{
				short* src = (short*)(reloc_base + (*reloc_data & 0xFFF));
				short ori = *src;
				*src += (ULONG)(HIWORD(image_base_delta));
				break;
			}
			case IMAGE_REL_BASED_LOW:
			{
				short* src = (short*)(reloc_base + (*reloc_data & 0xFFF));
				short ori = *src;
				*src += (ULONG)(LOWORD(image_base_delta));
				break;
			}
			case IMAGE_REL_BASED_HIGHLOW:
			{
				size_t* src = (size_t*)(reloc_base + (*reloc_data & 0xFFF));
				size_t ori = *src;
				*src += (size_t)image_base_delta;
				break;
			}
			case IMAGE_REL_BASED_DIR64: 
			{
				uintptr_t __unaligned* src = (uintptr_t __unaligned*)(reloc_base + (*reloc_data & 0xFFF));
				uintptr_t __unaligned ori = *src;
				*src += image_base_delta;
				break;
			}
			default: break;
			}
		}
		relocation_directory = (PIMAGE_BASE_RELOCATION)(reloc_data);
	}
}

void ManualMap::FixImportTable(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers, ULONG import_directory)
{
	if (nt_headers->OptionalHeader.DataDirectory[import_directory].Size)
	{
		auto import_desc = (PIMAGE_IMPORT_DESCRIPTOR)ResolveRVA(nt_headers->OptionalHeader.DataDirectory[import_directory].VirtualAddress, mod_ctx.image);
		if (import_desc)
		{
			for (; import_desc->Name; import_desc++)
			{
				auto module = (char*)ResolveRVA(import_desc->Name, mod_ctx.image);
				if (module == NULL)
					continue;
				auto module_handle = target_process.GetModule(module);
				if (!strstr(module, "msvc"))
				{
					int x = 0;
					x++;
				}
				if (module_handle == NULL)
				{
					std::wstring base_dll = RedirectionHelper::Instance()->ConvertFromMBS(std::string(module));
					RedirectionHelper::Instance()->ResolvePath(base_dll, EnsureFullPath, target_process);
					module_handle = LoadDependency(base_dll);
				}
				if (module_handle == NULL)
				{
					continue;
				}
				IMAGE_THUNK_DATA* thunk = nullptr;
				IMAGE_THUNK_DATA* funcdata = nullptr;

				if (import_desc->OriginalFirstThunk)
				{
					thunk = (IMAGE_THUNK_DATA*)ResolveRVA(import_desc->OriginalFirstThunk, mod_ctx.image);
					funcdata = (IMAGE_THUNK_DATA*)ResolveRVA(import_desc->FirstThunk, mod_ctx.image);
				}
				else
				{
					thunk = (IMAGE_THUNK_DATA*)ResolveRVA(import_desc->FirstThunk, mod_ctx.image);
					funcdata = (IMAGE_THUNK_DATA*)ResolveRVA(import_desc->FirstThunk, mod_ctx.image);
				}
				for(; thunk->u1.AddressOfData; thunk++, funcdata++)
				{
					FARPROC addr = 0;
					bool ordinal = ((thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) != 0);
					if (ordinal)
					{
						short funcord = (short)(thunk->u1.Ordinal & 0xFFFF);
						addr = (FARPROC)target_process.GetModuleExport(module_handle, (const char*)funcord);
						if (!addr) return;
					}
					else
					{
						auto import = (PIMAGE_IMPORT_BY_NAME)ResolveRVA(*(uintptr_t*)thunk, mod_ctx.image);
						char* name = (char*)import->Name;
						std::cout << name << std::endl;
						addr = (FARPROC)target_process.GetModuleExport(module_handle, name);

					}
					funcdata->u1.Function = (size_t)addr;
				}
			}
		}
	}
}

void ManualMap::ProcessSections(module_ctx & mod_ctx, IMAGE_NT_HEADERS * nt_headers)
{
	memcpy(
		mod_ctx.local_image,
		mod_ctx.image,
		nt_headers->OptionalHeader.SizeOfHeaders
	);
	IMAGE_SECTION_HEADER* image_sections = (IMAGE_SECTION_HEADER*)(nt_headers + 1);
	for (int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER header = image_sections[i];
		if (_stricmp(".reloc", (char*)header.Name) == 0)
		{
			continue;
		}
		if (header.Characteristics & (IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE))
		{
			if (header.SizeOfRawData > 0)
			{
				uintptr_t section_adr = (uintptr_t)(mod_ctx.local_image) + header.VirtualAddress; //Target local VA that our data will be copied to
				uintptr_t image_section = (uintptr_t)(mod_ctx.image) + header.PointerToRawData; //Source data from .dll image loaded from disk
				size_t section_size = image_sections[i].SizeOfRawData;
				memcpy(
					reinterpret_cast<void*>(section_adr),
					reinterpret_cast<void*>(image_section),
					section_size
				);
			}
		}
	}
}

void ManualMap::ProcessTlsEntries(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers)
{
	auto tls_dir = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
	if (tls_dir.Size == 0) return;

	auto tls_directory = (PIMAGE_TLS_DIRECTORY)ResolveRVA(tls_dir.VirtualAddress, mod_ctx.image);
	if (!tls_directory) return;

	PIMAGE_TLS_CALLBACK tls_callbacks[0xFF];
	if (!target_process.ReadMemory(tls_callbacks, tls_directory->AddressOfCallBacks, sizeof(tls_callbacks))) return;
	
	for (int i = 0; tls_callbacks[i]; i++)
	{
		CallEntrypoint(mod_ctx, (FARPROC)tls_callbacks[i]);
	}
		

}

void ManualMap::CallEntrypoint(module_ctx & mod_ctx, FARPROC entry)
{
	auto a = ASMFactory::GetAssembler();
	if (sxs_acontext && false)
	{
		/*
			sub rsp, (rsp_diff + 8)
			mov rax, sxs_acontext
			mov rax, [rax]
			mov rcx, rax

		*/
		size_t rsp_diff = 0x28;
		rsp_diff = a->Align(rsp_diff, 0x10);
		a->AddBytes({ 0x48, 0x83, 0xEC });
		a->AddByte((byte)(rsp_diff + 8));
		a->AddBytes({ 0x48, 0xB8 });
		a->AddLong64((size_t)sxs_acontext);
		a->AddBytes({ 0x48, 0x8B, 0x00 });
		a->AddBytes({ 0x48, 0x89, 0xC1 });
		a->LoadParameter((uintptr_t)sxs_acontext + sizeof(HANDLE), RDX);
		a->AddBytes({ 0x49, 0xBD });
		a->AddLong64((size_t)ActivateActCtx);
		a->AddBytes({ 0x41, 0xFF, 0xD5 });
		a->AddBytes({ 0x48, 0x83, 0xC4 });
		a->AddByte((byte)(rsp_diff + 8));
	}

	a->PushInt64((uintptr_t)mod_ctx.remote_image);
	a->PushInt64(DLL_PROCESS_ATTACH);
	a->PushInt64(0x00);
	a->PushCall(CWIN64CALL, entry);
	
	if (sxs_acontext && false)
	{
		size_t rsp_dif = a->Align(0x28, 0x10);
		// sub  rsp, (rsp_dif + 8)
		a->AddBytes({ 0x48, 0x83, 0xEC });
		a->AddByte((byte)(rsp_dif + 8));
		// >>>
		// >>>
		// mov  rax, m_pAContext + sizeof(HANDLE)
		a->AddBytes({ 0x48, 0xB8 });
		a->AddLong64(sxs_acontext + sizeof(HANDLE));
		// mov  rax, [rax]
		a->AddBytes({ 0x48, 0x8B, 0x00 });
		// mov  rcx, 0 -> first parameter
		a->LoadParameter(0, RCX);
		// mov  rdx, rax
		a->AddBytes({ 0x48, 0x89, 0xC2 });
		// mov  r13, calladdress
		a->AddBytes({ 0x49, 0xBD });
		a->AddLong64((size_t)DeactivateActCtx);
		// call r13
		a->AddBytes({ 0x41, 0xFF, 0xD5 });
		// >>>
		// >>>
		// add rsp, (rsp_dif + 8)
		a->AddBytes({ 0x48, 0x83, 0xC4 });
		a->AddByte((byte)(rsp_dif + 8));
	}
	a->SaveReturn();
	a->SignalOSEvent();
	a->GenerateEpilogue();

	size_t result;
	a->ExecuteASMInWorker(result);
	a->TerminateWorkerThread();
	a->FlushAssembler();
}

void ManualMap::InitializeCookie(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers)
{
	auto cookie_dir = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
	auto load_config = (PIMAGE_LOAD_CONFIG_DIRECTORY)ResolveRVA(cookie_dir.VirtualAddress, mod_ctx.image);
	if (load_config && load_config->SecurityCookie)
	{
		FILETIME systime = { 0 };
		LARGE_INTEGER performance = { 0 };
		uintptr_t cookie = 0;

		GetSystemTimeAsFileTime(&systime);
		QueryPerformanceCounter(&performance);

		cookie = target_process.GetProcessID() ^ ASMFactory::GetAssembler()->GetWorkerThreadID() ^ (uintptr_t)(&cookie);
#ifdef _M_AMD64
		cookie ^= *(uintptr_t*)(&systime);
		cookie ^= (performance.QuadPart << 32) ^ performance.QuadPart;
		cookie &= 0xFFFFFFFFFFFF;

		if (cookie == 0x2B992DDFA232)
			cookie++;
#else

		cookie ^= systime.dwHighDateTime ^ systime.dwLowDateTime;
		cookie ^= performance.LowPart;
		cookie ^= performance.HighPart;

		if (cookie == 0xBB40E64E)
			cookie++;
		else if (!(cookie & 0xFFFF0000))
			cookie |= (cookie | 0x4711) << 16;
#endif
		size_t remote_cookie_addr = (size_t)load_config->SecurityCookie - (size_t)mod_ctx.image + (size_t)mod_ctx.remote_image;
		if (!target_process.WriteMemory((void*)cookie, remote_cookie_addr, sizeof(uintptr_t))) return;
	}
}

NTSTATUS ManualMap::InjectImage(const std::string& path)
{
	auto file_bytes = Helpers::Instance()->ReadAllBytes(path.c_str());
	byte* image = file_bytes.data();

	auto a = ASMFactory::GetAssembler();
	a->SetAssemblerTargetProcess(target_process);

	PerformImageMap(image, StripFilePath(path));

	return ERROR_SUCCESS;
}

/*
uint32_t ManualMap::CreateWorkerThread()
{
worker_code_thread = target_process.AllocateMemory(0x1000);
user_code = target_process.AllocateMemory(0x1000);
user_data = target_process.AllocateMemory(0x4000, PAGE_READWRITE);

JitRuntime rt;
CodeHolder code;
code.init(rt.getCodeInfo());

X86Assembler a(&code);
Label loop = a.newLabel();

if (worker_thread == INVALID_HANDLE_VALUE || worker_thread == NULL)
{
auto ntdll = loaded_modules["ntdll"];
auto procedure = GetModuleExport(ntdll, "NtDelayExecution");
auto terminate = GetModuleExport(ntdll, "NtTerminateThread");
if (!procedure || !terminate)
{
return 0xFFFFFFFF;
}
{
a.bind(loop);
a.push(TRUE);
a.push(worker_code_thread);
a.mov(x86::eax, procedure);
a.call(x86::eax);
a.jmp(loop);

a.mov(x86::edx, user_data);
a.mov(x86::dword_ptr(x86::edx), x86::eax);
a.push(x86::eax);
a.push(NULL);
a.mov(x86::eax, terminate);
a.call(x86::eax);
a.ret();
}

LARGE_INTEGER delay = { {0} };
delay.QuadPart = -10 * 1000 * 5;
target_process.WriteMemory((void*)&delay, worker_code_thread, sizeof(LARGE_INTEGER));
size_t code_size = code.getCodeSize();
void* code_ptr;
rt.add(&code_ptr, &code);
target_process.WriteMemory(code_ptr, worker_code_thread + sizeof(LARGE_INTEGER), code_size);
worker_thread = target_process.CreateThread(worker_code_thread + sizeof(LARGE_INTEGER), user_data);
return GetThreadId(worker_thread);
}

}
*/

uintptr_t ManualMap::PerformImageMap(byte* image, const std::string& image_name, module_ctx* mod_ctx)
{
	bool depend = false;
	if (mod_ctx == NULL)
	{
		depend = true;
		mod_ctx = &ctx;
	}
	mod_ctx->name = image_name;
	auto a = ASMFactory::GetAssembler();
	auto image_header = Helpers::Instance()->GetImageHeader(image);
	auto nt_header = Helpers::Instance()->GetImageNTHeaders(image, image_header);
	mod_ctx->image = image;
	a->CreateRPCEnvironment();
	CreateActx(image);
	size_t rva_low = (!depend) ? ((uintptr_t)-1) : 0;
	size_t rva_high = 0;

	auto section_header = (PIMAGE_SECTION_HEADER)((uintptr_t)&nt_header->OptionalHeader + nt_header->FileHeader.SizeOfOptionalHeader);
	for (auto i = 0; i < nt_header->FileHeader.NumberOfSections; ++i)
	{
		if (!section_header[i].Misc.VirtualSize)
			continue;
		if (section_header[i].VirtualAddress < rva_low)
			rva_low = section_header[i].VirtualAddress;
		if ((section_header[i].VirtualAddress + section_header[i].Misc.VirtualSize) > rva_high)
			rva_high = section_header[i].VirtualAddress + section_header[i].Misc.VirtualSize;
	}

	size_t image_size = rva_high - rva_low;

	if ((nt_header->OptionalHeader.ImageBase % 4096) != 0)
	{
		return 0;
	}


	auto handle = target_process.AllocateSection(image_size);
	Helpers::Instance()->MirrorRemoteSection(
		target_process.GetProcessHandle(),
		handle,
		nt_header->OptionalHeader.SizeOfImage,
		mod_ctx->remote_image,
		mod_ctx->local_image
		);

	FixImportTable(*mod_ctx, nt_header, IMAGE_DIRECTORY_ENTRY_IMPORT);
	FixImportTable(*mod_ctx, nt_header, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	ImageRelocations(nt_header, *mod_ctx);
	ProcessSections(*mod_ctx, nt_header);
	ProcessTlsEntries(*mod_ctx, nt_header);
	InitializeCookie(*mod_ctx, nt_header);
	if (nt_header->OptionalHeader.AddressOfEntryPoint)
	{
		FARPROC dll_entry = (FARPROC)((uintptr_t)mod_ctx->remote_image + nt_header->OptionalHeader.AddressOfEntryPoint);
		CallEntrypoint(*mod_ctx, dll_entry);
	}

	//InitializeCookie(*mod_ctx, nt_header);

	return 0;
}

/*
uintptr_t ManualMap::PerformImageMap(byte* image, const std::string& image_name, module_ctx* mod_ctx)
{
std::cout << "Mapping image: " << image_name << std::endl;
if (mod_ctx == NULL)
{
mod_ctx = &ctx;
}
auto image_header = Helpers::Instance()->GetImageHeader(image);
auto nt_header = Helpers::Instance()->GetImageNTHeaders(image, image_header);
mod_ctx->image = image;
try
{
//Allocate our memory section
auto handle = target_process.AllocateSection(nt_header->OptionalHeader.SizeOfImage);
//See helpers.hpp for more information on this function
Helpers::Instance()->MirrorRemoteSection(target_process.GetProcessHandle(), handle, nt_header->OptionalHeader.SizeOfImage, mod_ctx->remote_image, mod_ctx->local_image);
mapped_modules[image_name] = *mod_ctx;
memcpy(
mod_ctx->local_image,
image,
nt_header->OptionalHeader.SizeOfHeaders
);
WriteImageSections(image_header, nt_header, nt_header->FileHeader.NumberOfSections, *mod_ctx);
AdjustImageReloc(&nt_header->OptionalHeader, *mod_ctx);
FixImportTable(&nt_header->OptionalHeader, *mod_ctx);
CloseHandle(handle);
if (!NT_SUCCESS(NtUnmapViewOfSection(GetCurrentProcess(), mod_ctx->local_image)))
{
throw "Failed to unmap section!";
}
if (!image_name.compare("Cheat Module"))
{
std::cout << "Entry point for module: " << image_name << " is: 0x" << std::hex << ((uintptr_t)mod_ctx->remote_image + nt_header->OptionalHeader.AddressOfEntryPoint) << std::dec << std::endl;
CallEntrypoint(&nt_header->OptionalHeader);
}
return (uintptr_t)mod_ctx->remote_image;
}
catch (const char* e)
{
//Error
printf(e);
return 0;
}


return 0;
}
*/

uintptr_t ManualMap::GetModuleExport(uintptr_t module_handle, const char * function_ordinal)
{
	return 0;
}


void ManualMap::CreateActx(byte* image)
{

	auto a = ASMFactory::GetAssembler();
	const char* manifest = nullptr;

	auto create_actx = target_process.GetModuleExport("kernel32.dll", "CreateActCtxA");

	void* manifest_data = nullptr;
	size_t manifest_size = 0;
	GetEmbeddedManifestResource(image, manifest_size, 2, &manifest_data);
	if (!manifest_data) return;
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
				if (!b) { return; }
			}

			base_dir_set = true;
		}
	}
	if (!base_dir_set) return;
	char base_dir_mbs[MAX_PATH] = { 0 };
	wcstombs(base_dir_mbs, base_dir, MAX_PATH);
	manifest = (const char*)(malloc(manifest_size));
	strcpy_s((char*)manifest, MAX_PATH, base_dir_mbs);
	char random_filename[MAX_PATH];
	sprintf_s(random_filename, "\\%IX\0", GetTickCount64());
	strcat_s((char*)manifest, sizeof(random_filename), random_filename);

	FILE* f;
	auto err = fopen_s(&f, manifest, "w");
	if (f && err == 0)
	{
		fwrite(manifest_data, sizeof(char), manifest_size, f);
	}
	fclose(f);
	sxs_acontext = target_process.AllocateMemory(512, 0, PAGE_READWRITE);

	size_t result;
	ACTCTX alloc = { 0 };
	alloc.cbSize = sizeof(alloc);
	alloc.lpSource = (LPCSTR)((SIZE_T)sxs_acontext + sizeof(HANDLE) + sizeof(alloc));

	a->GeneratePrologue();
	a->PushInt64((uintptr_t)(size_t)sxs_acontext + sizeof(HANDLE));
	a->PushCall(CallingConvention64::CWIN64CALL, (FARPROC)create_actx);
	a->AddBytes({ 0x48, 0xBA });
	a->AddLong64((uintptr_t)sxs_acontext);
	a->AddBytes({ 0x48, 0x89, 0x02 });
	a->SaveReturn();
	a->SignalOSEvent();
	a->GenerateEpilogue();

	target_process.WriteMemory((void*)&alloc, sxs_acontext + sizeof(HANDLE), sizeof(alloc));
	target_process.WriteMemory((void*)manifest, sxs_acontext + sizeof(HANDLE) + sizeof(alloc), strlen(manifest) + 1);
	a->ExecuteASMInWorker(result);
	free((void*)manifest);
}

void ManualMap::GetEmbeddedManifestResource(byte* image, size_t& res_size, uint32_t id, void ** res)
{
	auto nt_headers = Helpers::Instance()->GetImageNTHeaders(
		image,
		Helpers::Instance()->GetImageHeader(image)
		);

	*res = NULL;
	res_size = 0;

	size_t size = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	if (size == 0) return;

	auto resource_va = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	auto root = (PIMAGE_RESOURCE_DIRECTORY)ResolveRVA(resource_va, image);
	if (root)
	{
		const IMAGE_RESOURCE_DIR_STRING_U* dir_str = 0;
		for (WORD i = 0; i < root->NumberOfIdEntries + root->NumberOfNamedEntries; i++)
		{
			auto entry_type = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(root + 1 + i);
			if ((entry_type->OffsetToDirectory) >= size)
				return;
			auto res_type = (PIMAGE_RESOURCE_DIRECTORY)((byte*)root + (entry_type->OffsetToDirectory));

			if (entry_type->NameIsString) dir_str = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((byte*)root + entry_type->NameOffset);

			for (WORD j = 0; j < res_type->NumberOfIdEntries + res_type->NumberOfNamedEntries; j++)
			{
				auto entry_id = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(res_type + 1 + j);
				if ((entry_id->OffsetToDirectory) >= size)
					return;
				if (entry_id->Id != id)
					continue;

				auto res_id = (PIMAGE_RESOURCE_DIRECTORY)((byte*)root + (entry_id->OffsetToDirectory));
				if (entry_id->NameIsString) dir_str = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((byte*)root + (entry_id->OffsetToDirectory));

				for (WORD k = 0; k < res_id->NumberOfIdEntries + res_id->NumberOfNamedEntries; k++)
				{
					auto data_lang = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(res_id + 1 + k);
					if ((data_lang->OffsetToDirectory) >= size)
						return;

					auto data = (PIMAGE_RESOURCE_DATA_ENTRY)((byte*)root + (data_lang->OffsetToDirectory));
					if (data_lang->NameIsString) dir_str = reinterpret_cast<const IMAGE_RESOURCE_DIR_STRING_U*>((byte*)root + data_lang->NameOffset);

					if (data->Size == 0)
						continue;
					void* res_data = (void*)ResolveRVA(data->OffsetToData, image);
					if (res_data && (uintptr_t)res_data == (uintptr_t)-1)
						continue;
					*res = res_data;
					res_size = data->Size;
					return;

				}
			}
		}
	}
}

uintptr_t ManualMap::LoadDependency(const std::wstring & path)
{
	if (path.empty()) return NULL;

	auto a = ASMFactory::GetAssembler();
	auto remote_ldr = target_process.GetModuleExport("ntdll.dll", "LdrLoadDll");
	UNICODE_STRING path_ustr;
	RtlInitUnicodeString(&path_ustr, path.c_str());
	
	ULONG flags = 0;
	auto flags_ptr = a->CommitBlock((void*)&flags, sizeof(ULONG));
	auto return_pointer = target_process.AllocateMemory(sizeof(size_t));
	{
		a->GeneratePrologue();
		a->PushInt64(0);
		a->PushInt64((uintptr_t)flags_ptr);
		a->PushUNICODEStringStructure(&path_ustr);
		a->PushInt64(return_pointer);
		a->PushCall(CWIN64CALL, (FARPROC)remote_ldr);
		a->AddBytes({ 0x48, 0x89, 0xD0 });
		a->AddBytes({ 0x48, 0xA3 });
		a->AddLong64((uintptr_t)return_pointer);
		a->GenerateEpilogue();
	}

	a->ExecuteASM();

	uintptr_t ret;
	target_process.ReadMemory((void*)&ret, return_pointer, sizeof(uintptr_t));

	return ret;

}
