#pragma once
#include <Windows.h>
#include <stdint.h>
#include <ostream>

#include <unordered_map>
class Process
{
public:
	Process() {}
	Process(const char* name);
	Process(const char* name, HANDLE handle);
	Process(HANDLE handle);
	~Process();
	friend std::ostream& operator << (std::ostream& os, const Process& process);

	uint32_t FindProcessID(const char* name);
	void Attach(DWORD access = PROCESS_ALL_ACCESS);
	void Detach();

	static Process CurrentProcess() { return Process(GetCurrentProcess()); }

	uintptr_t GetPEBAddress();

	HANDLE AllocateSection(unsigned long long);
	HANDLE GetProcessHandle() const { return process_handle; }

	std::unordered_map<std::string, uintptr_t> GetModules();

	uintptr_t AllocateMemory(const size_t size, const uintptr_t address = 0, uint32_t access = PAGE_EXECUTE_READWRITE);

	HANDLE CreateThread(const uintptr_t address, const uintptr_t argument);

	bool ReadMemory(void* buf, const uintptr_t addr, const SIZE_T size);
	bool WriteMemory(void* buf, const uintptr_t addr, const SIZE_T size);
	bool WriteMemory(void* buf, uintptr_t& addr, const SIZE_T size, bool inc_addr);
	bool FreeMemory(const uintptr_t address);
	bool FreeMemory(const uintptr_t address, const SIZE_T size);

	uintptr_t GetProcessID() { return ::GetProcessId(process_handle); }

	uintptr_t GetModule(const std::string& name) 
	{ 
		if (loaded_modules.find(name) != loaded_modules.end())
			return loaded_modules[name];
		return NULL;
	}
	uintptr_t GetModuleExport(const std::string& module_name, const char* ord) { return GetModuleExport(GetModule(module_name), ord); }
	uintptr_t GetModuleExport(uintptr_t module, const char* ord);
	void UnlinkModuleFromPEB(uintptr_t module_handle);
	uintptr_t RemoteGetModuleHandle(const char * module);
	std::wstring Directory();

	template<typename T>
	bool RPM_Wrapper(T* buf, const uintptr_t addr)
	{
		return ReadMemory(buf, addr, sizeof(T));
	}

private:
	HANDLE process_handle;
	const char* process_name;
	uint32_t process_id;

	std::unordered_map<std::string, uintptr_t> loaded_modules;
};