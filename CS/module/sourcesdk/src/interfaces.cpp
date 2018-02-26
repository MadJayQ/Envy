#include "..\include\interfaces.h"

#include "peb.h"

Interfaces::Interfaces()
{
	auto peb = Peb::Instance();
	for (auto&& module : peb->Modules())
	{
		auto create_interface_addr = module.second.Export("CreateInterface");
		if (create_interface_addr != NULL)
		{
			MemoryAddress location(create_interface_addr + 0x4);
			if (location->Read<byte>() == 0xE9)
			{
				auto relative_jump = (location + 0x1)->Read<int32_t>();
				auto list = (location + 0x5 + relative_jump);
				InterfaceListEntry* interface_list = **(InterfaceListEntry***)(list.ptr() + 0x6); //TODO(Jake): Add support for multiple levels of indirection to MemoryAddress class
				for (; interface_list; interface_list = interface_list->next)
				{
					m_interfaces.insert(
						std::make_pair(
							interface_list->name,
							Function<uintptr_t>(reinterpret_cast<uintptr_t>(interface_list->fn))()
						)
					);
				}
			}
		}
	}
}

uintptr_t Interfaces::GetInterfaceAddr(const std::string& name)
{
	auto itr = FindInterfaceByPrefix(name);
	if (itr != m_interfaces.end())
	{
		return itr->second;
	}
	return 0;
}

InterfaceMap::iterator Interfaces::FindInterfaceByPrefix(const std::string & search)
{
	auto itr = m_interfaces.lower_bound(search);
	if (itr != m_interfaces.end())
	{
		std::string key = std::string(itr->first);
		if (key.compare(0, search.size(), search) == 0)
			return itr;
	}
	return m_interfaces.end();
}
