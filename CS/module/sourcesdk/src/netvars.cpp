#include "netvars.h"
#include "interfaces.h"
#include "clientdll.hpp"
#include <functional>

#include "envydefs.hpp"
using namespace Envy::SourceEngine;

using LoadFunction =/* std::unique_ptr<Envy::NetvarTable>(*)(RecvTable*, uintptr_t); */ std::function<std::unique_ptr<Envy::NetvarTable>(RecvTable*, uintptr_t)>();
ptrdiff_t Envy::Netvars::Offset(const std::string & tableName, const std::string & prop)
{

	CACHED std::function<ptrdiff_t(NetvarTable*, const std::string&)> search = [&](NetvarTable* table, const std::string& propName) -> ptrdiff_t
	{
		for (const auto& prop : table->m_childprops)
		{
			if (!strncmp(
				prop.second->m_pVarName, 
				propName.data(), 
				propName.size())
			)
			{
				return table->m_offset + prop.second->m_Offset;
			}
		}
		for (const auto& child : table->m_childtable)
		{
			auto offset = search(child.second.get(), propName);
			if (offset != 0)
				return table->m_offset + offset;
		}
		for (const auto& child : table->m_childtable)
		{
			if (!strncmp(
				child.first.data(),
				propName.data(),
				propName.size())
			)
			{
				return table->m_offset + child.second->m_offset;
			}
		}

		return 032;
	};
	auto result = 032;
	for (const auto& table : m_database->table())
	{
		if (!table.first.compare(tableName))
		{
			result = search(table.second.get(), prop);
			if (result != 0)
				return result;
		}
	}
	return 032;
}
void Envy::Netvars::PopulateDatabase()
{
	auto client_dll = Interfaces::Instance()->CreateInterface<IBaseClientDll>();
	auto client_classes = (*client_dll)->GetAllClasses();
	//std::function<LoadFunction> load_table;
	std::function<std::unique_ptr<NetvarTable>(RecvTable*, uintptr_t)> load_table;
	uintptr_t netvarcount;
	m_database = std::make_unique<NetvarDatabase>();
	load_table = [&load_table, &netvarcount](RecvTable* table, uintptr_t offset) mutable -> std::unique_ptr<NetvarTable>
	{
		auto ptr = std::make_unique<NetvarTable>();
		ptr->m_offset = offset;
		for (auto i = 0; i < table->m_nProps; i++)
		{
			auto prop = &table->m_pProps[i];
			if (!prop) continue;
			if (isdigit(prop->m_pVarName[0])) continue;
			if (strcmp(prop->m_pVarName, "baseclass") == 0) continue;
			if (prop->m_RecvType == SourceEngine::SendPropType::DPT_DataTable &&
				prop->m_pDataTable != NULL &&
				prop->m_pDataTable->m_pNetTableName[0] == 'D')
			{
				ptr->insert(prop->m_pVarName, load_table(prop->m_pDataTable, prop->m_Offset));
			}
			else
			{
				ptr->insert(std::string(prop->m_pVarName), prop);
			}
			netvarcount++;
		}
		return ptr;
	};
	
	for (
		auto class_ptr = client_classes;
		class_ptr;
		class_ptr = class_ptr->m_pNext
	)
	{
		if (class_ptr->m_pRecvTable)
		{
			m_database->insert(class_ptr->m_pRecvTable->m_pNetTableName, load_table(class_ptr->m_pRecvTable, 0));
			m_tablecount++;
		}
	}
}

void Envy::Netvars::CleanupDatabase()
{
}
