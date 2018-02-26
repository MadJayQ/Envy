#pragma once
#include "interfaces.h"
#include "clientclass.hpp"

constexpr const char* interface_name = "VClient";


namespace Envy
{
	namespace SourceEngine
	{
		class _IBaseClientDll_
		{
			using CreateInterfaceFn = void*(*)(const char*, int);
		public:
			virtual int              Connect(CreateInterfaceFn appSystemFactory, void *pGlobals) = 0;
			virtual int              Disconnect(void) = 0;
			virtual int              Init(CreateInterfaceFn appSystemFactory, void *pGlobals) = 0;
			virtual void             PostInit() = 0;
			virtual void             Shutdown(void) = 0;
			virtual void             LevelInitPreEntity(char const* pMapName) = 0;
			virtual void             LevelInitPostEntity() = 0;
			virtual void             LevelShutdown(void) = 0;
			virtual ClientClass*	 GetAllClasses(void) = 0;
		};
	}

	class IBaseClientDll : public SourceInterface<SourceEngine::_IBaseClientDll_>
	{
	public:
		IBaseClientDll() {}
		virtual Interface* Init() override
		{
			m_data = reinterpret_cast<Interface*>(
				Interfaces::Instance()->GetInterfaceAddr("VClient")
			);
			SourceInterface<SourceEngine::_IBaseClientDll_>::Init();
			return m_data;
		}
	};
}