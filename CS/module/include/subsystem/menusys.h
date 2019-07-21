#pragma once

#include "subsystem.h"
#include "envydefs.hpp"
#include "menu.h"

#include <d3d9.h>

namespace Envy
{

	class MenuSubsystem : public ICheatSubsystem
	{
	public:
		MenuSubsystem() : 
			m_hCSGOWindow(NULL),
			m_bWindowOpen(false),
			m_bGUIInit(false)
		{
			m_pMenu = std::make_unique<Menu>();
		}

		virtual void Init() override;

		void OnDeviceLost();
		void OnReset(IDirect3DDevice9* ptr, D3DPRESENT_PARAMETERS * pPresentationParameters);
		void OnEndScene(IDirect3DDevice9* ptr);

		bool WindowOpen() const { return m_bWindowOpen; }

		Menu* GetMenu() const { return m_pMenu.get(); }

		LRESULT ENVY_API MenuMessagePump(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void OnToggleMenu();
		void ToggleExternalWindow() { m_pMenu->ToggleExternal(true); }

	private:
		HWND m_hCSGOWindow;
		WNDPROC m_oWndProc;
		bool m_bWindowOpen;
		bool m_bGUIInit;

		std::unique_ptr<Menu> m_pMenu;
	};
}