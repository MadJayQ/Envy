#pragma once
#include "singleton.hpp"
#include "vmtfunc.hpp"
#include "vmt.h"
#include "address.h"
#include <map>

#define FROMVMT(object, index) VMTManager::Instance()->GetVMTFunction((uintptr_t)object, index)

class VMTManager : public Singleton<VMTManager>
{
public:
	VMTManager();
	~VMTManager();

	template<typename fn> fn GetVirtualFunction(void* object, int idx)
	{
		int* vmt = *(int**)object;
		int address = vmt[idx];
		return (fn)(address);
	}
	VMT* CreateVMT(uintptr_t object);
	void DeleteVMT(uintptr_t object);
	Address GetVMTFunction(uintptr_t object, int index, bool old = true);

private:
	std::map<uintptr_t, VMT*> m_vmts;
};
