#pragma once
#include "envydefs.hpp"
#include "function.hpp"
#include "options.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>

namespace Envy
{
	class ICheatSubsystem
	{
	public:
		virtual ~ICheatSubsystem() {}
		virtual void Init() = 0;
	};

	class CheatSubsystems
	{
	public:
		CheatSubsystems() {}

		template<typename T>
		T* Create()
		{
			auto type_id = &typeid(T);
			if (Exists<T>()) return static_cast<T*>(m_Subsystems[type_id].get());
			auto ptr = std::make_unique<T>();
			ptr->Init();
			m_Subsystems.insert(
				std::make_pair(
					type_id,
					std::move(ptr)
				)
			);
			return static_cast<T*>(m_Subsystems[type_id].get());
		}

		template<typename T> bool Exists() { return m_Subsystems.count(&typeid(T)) != 0; }
		template<typename T> T* Get() { return static_cast<T*>((Exists<T>()) ? m_Subsystems[&typeid(T)].get() : Create<T>()); }
	private:
		std::unordered_map<const std::type_info*, std::unique_ptr<ICheatSubsystem>> m_Subsystems;
	};

	extern std::unique_ptr<CheatSubsystems> g_Subsystems;
}