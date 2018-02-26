#pragma once

#include "interfaces.h"
#include "vmtmanager.h"
namespace Envy
{
	namespace SourceEngine
	{
		class _IPanel_
		{
		public:
			const char *GetName(unsigned int vguiPanel)
			{
				typedef const char *(__thiscall* tGetName)(void*, unsigned int);
				return VMTManager::Instance()->GetVirtualFunction<tGetName>(this, 36)(this, vguiPanel);
			}
#ifdef GetClassName
#undef GetClassName
#endif
			const char *GetClassName(unsigned int vguiPanel)
			{
				typedef const char *(__thiscall* tGetClassName)(void*, unsigned int);
				return VMTManager::Instance()->GetVirtualFunction<tGetClassName>(this, 37)(this, vguiPanel);
			}
		};
	}

	class IPanel : public SourceInterface<SourceEngine::_IPanel_>
	{
	public:
		IPanel() {}

		virtual Interface* Init() override
		{
			m_data = reinterpret_cast<Interface*>(
				Interfaces::Instance()->GetInterfaceAddr("VGUI_Panel")
			);
			SourceInterface<SourceEngine::_IPanel_>::Init();

			return m_data;
		}
	};
}