#include <stdint.h>

#include "process.h"

struct module_ctx
{
	void* remote_image;
	void* local_image;
	byte* image;
	std::string name;
	std::string path;
	uintptr_t ldr_ptr;
};
struct reloc_item
{
	WORD offset : 12;
	WORD type : 4;

	WORD get_offset()
	{
		return offset % 4096;
	}
};
struct reloc_data
{
	ULONG page_rva;
	ULONG block_size;
	reloc_item item[1];
};

class ManualMap
{
public:
	ManualMap(Process p);
	~ManualMap();

	uintptr_t PerformImageMap(byte* image, const std::string& image_name, module_ctx* mod_ctx = NULL);

	NTSTATUS CreateActivationContext(module_ctx& mod_ctx);
	void WriteImageSections(IMAGE_DOS_HEADER* dos_header, IMAGE_NT_HEADERS* nt_headers, int num_sections, module_ctx& mod_ctx);
	void ImageRelocations(IMAGE_NT_HEADERS* nt_headers, module_ctx& mod_ctx);
	void FixImportTable(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers, ULONG import_table_directory);
	void ProcessSections(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers);
	void ProcessTlsEntries(module_ctx& mod_ctx, IMAGE_NT_HEADERS* nt_headers);

	void CallEntrypoint(module_ctx& mod_ctx, FARPROC entry);
	void InitializeCookie(module_ctx& mod_ctx, IMAGE_NT_HEADERS*);

	NTSTATUS InjectImage(const std::string& path);

private:
	uintptr_t GetModuleExport(uintptr_t mod, const char* ord);
	void CreateActx(byte* image);
	void GetEmbeddedManifestResource(byte* image, size_t&  res_size, uint32_t id, void** res);
	uintptr_t LoadDependency(const std::wstring& path);
public:
	module_ctx ctx; //Main module ctx
	Process target_process;


private:
	uintptr_t sxs_acontext;
};