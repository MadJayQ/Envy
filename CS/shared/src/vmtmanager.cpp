#include "vmtmanager.h"

VMTManager::VMTManager()
{

}

VMTManager::~VMTManager()
{
	for (auto itr = m_vmts.begin(); itr != m_vmts.end(); itr++)
	{
		delete itr->second;
	}
	m_vmts.clear();
}


VMT* VMTManager::CreateVMT(uintptr_t object)
{
	if (m_vmts.find(object) == m_vmts.end())
	{
		m_vmts.insert(
			std::make_pair(
				object,
				new VMT((void*)object)
			)
		);
	}
	
	return m_vmts[object];
}

void VMTManager::DeleteVMT(uintptr_t object)
{
	auto itr = m_vmts.find(object);
	if (itr == m_vmts.end()) return;
	delete itr->second;
	m_vmts.erase(object);
}

Address VMTManager::GetVMTFunction(uintptr_t object, int index, bool old)
{
	return m_vmts[object]->GetEntry(index)[(int)(!old)];
}
