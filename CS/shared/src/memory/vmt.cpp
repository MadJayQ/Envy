#include "..\..\include\memory\vmt.h"

static constexpr size_t ENTRY_SIZE = sizeof(uintptr_t);


void Patch(void* &object, uintptr_t* &vmt)
{
	auto protect = 0UL;
	VirtualProtect(object, ENTRY_SIZE, PAGE_EXECUTE_READWRITE, &protect);
	*(uintptr_t**)object = vmt;
	VirtualProtect(object, ENTRY_SIZE, protect, &protect);
}
VMT::VMT(void* object) : m_object(object)
{
	m_vmtbackup = *(uintptr_t**)object;
	m_rtti = *(RTTICompleteObjectLocator**)((int*)m_vmtbackup - 1);
	size_t vmt_size = 0;
	uintptr_t current = m_vmtbackup[vmt_size];
	while (current >= 0x00010000 && current < 0xFFF00000 && vmt_size < 512)
	{
		vmt_size++;
		current = m_vmtbackup[vmt_size];
	}
	m_vmtnew = new uintptr_t[vmt_size]();
	std::memcpy(m_vmtnew, m_vmtbackup, vmt_size * ENTRY_SIZE);
	Patch(m_object, m_vmtnew);

}

VMT::~VMT()
{
	if (m_vmtnew)
	{
		delete[] m_vmtnew;
		*(void**)m_vmtnew = NULL;
		Patch(m_object, m_vmtbackup);
	}
}

uintptr_t VMT::HookFunction(uintptr_t idx, uintptr_t addr)
{
	uintptr_t old = m_vmtbackup[idx];
	uintptr_t hook = addr;
	m_vmtnew[idx] = addr;
	std::array<uintptr_t, 2> entry = { old, hook };
	m_functionmap.insert(
		std::make_pair(
			idx,
			entry
		)
	);
	return old;
}

void VMT::UnhookFunction(uintptr_t idx)
{
	auto table_itr = m_functionmap.find(idx);
	auto original = table_itr->second[0];
	m_vmtnew[idx] = original;

}
