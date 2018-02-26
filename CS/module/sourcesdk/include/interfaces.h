#pragma once
#include "singleton.hpp"
#include "function.hpp"
#include "memory\address.h"

#include <map>
#include <algorithm>

struct InterfaceListEntry
{
	using func = void*(*)();
	func fn;
	const char* name;
	InterfaceListEntry* next;
};


using InterfaceMap = std::map <std::string, uintptr_t>;
using InterfaceMapValue = InterfaceMap::value_type;

struct _Source_Interface_Base 
{
public:
	_Source_Interface_Base() {}

protected:
	void* raw_ptr;
};

template<typename T>
class SourceInterface : public _Source_Interface_Base
{
public:
	SourceInterface() : _Source_Interface_Base() {}
	using Interface = T;
	virtual Interface* Init() { raw_ptr = (void*)m_data; return m_data; }
	Interface* operator ->() { return m_data; }
	Interface* get() { return m_data; }
protected:
	Interface* m_data;
};

class Interfaces : public Singleton<Interfaces>
{
public:
	Interfaces();
	uintptr_t GetInterfaceAddr(const std::string& name);

	template<typename T>
	T* CreateInterface()
	{
		auto type_id = &typeid(T);
		if (Exists<T>()) return static_cast<T*>(m_loadedInterfaces[type_id].get());
		auto ptr = std::make_unique<T>();
		ptr->Init();
		m_loadedInterfaces.insert(
			std::make_pair(
				type_id,
				std::move(ptr)
			)
		);
		return static_cast<T*>(m_loadedInterfaces[type_id].get());
	}

	template<typename T>
	bool Exists()
	{
		return m_loadedInterfaces.count(&typeid(T)) != 0;
	}

	template<typename T>
	T* GetInterface()
	{
		return static_cast<T*>((Exists<T>()) ? m_loadedInterfaces[&typeid(T)].get() : CreateInterface<T>());
	}

private:
	InterfaceMap::iterator FindInterfaceByPrefix(const std::string& search);
	InterfaceMap m_interfaces;

private:
	std::unordered_map<const std::type_info*, std::unique_ptr<_Source_Interface_Base>> m_loadedInterfaces;
};