#pragma once
#include "singleton.hpp"
#include "engine\recvtable.h"

#include <unordered_map>
#include <iostream>

namespace Envy
{
	struct NetvarTable;

	using NetvarTableMap = std::unordered_map<std::string, std::unique_ptr<NetvarTable>>;
	using NetvarPropMap = std::unordered_map<std::string, Envy::SourceEngine::RecvProp*>;
	using TableMapIterator = NetvarTableMap::iterator;

	struct NetvarTable
	{
		NetvarTableMap m_childtable;
		NetvarPropMap m_childprops;
		uintptr_t m_offset;

		bool empty() { return m_childtable.size() == 0 && m_childprops.size() == 0; }

		void insert(const std::string& name, std::unique_ptr<NetvarTable>&& table)
		{
			m_childtable.emplace(
				name,
				std::move(table)
			);
		}
		void insert(const std::string& name, Envy::SourceEngine::RecvProp* prop)
		{
			m_childprops.emplace(
				name,
				prop
			);
		}
	};

	struct NetvarDatabase
	{
		NetvarTableMap m_tables;
		void insert(const std::string&& name, std::unique_ptr<NetvarTable>&& table)
		{
			std::cout << name.c_str() << std::endl;
			auto table_ptr = table.get();
			m_tables.emplace(
				name,
				std::move(table)
			);
		}
		TableMapIterator find(const std::string& key) { return m_tables.find(key); }
		TableMapIterator begin() { return m_tables.begin(); }
		TableMapIterator end() { return m_tables.end(); }
		NetvarTableMap& table() { return m_tables; }
	};

	class Netvars : public Singleton<Netvars>
	{
	public:
		Netvars() :m_tablecount(0), m_netvarcount(0)
		{
			PopulateDatabase();
		}

		ptrdiff_t Offset(const std::string& table, const std::string& prop);
	private:
		void PopulateDatabase();
		void CleanupDatabase();

		std::unique_ptr<NetvarDatabase> m_database;
		uint32_t m_tablecount;
		uint32_t m_netvarcount;

	};
}