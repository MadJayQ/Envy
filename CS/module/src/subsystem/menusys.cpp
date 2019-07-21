#include "menusys.h"
#include "inputsys.h"
#include "interfaces.h"

#include "icvar.hpp"
#include "convar.h"

#include "imgui_impl_dx9.h"

#include "isurface.hpp"

static constexpr const char* g_sValveWindowClass = "Valve001";
namespace Envy
{
	Envy::MenuSubsystem* g_pThis;
	LRESULT WINAPI Hooked_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		g_Subsystems->Get<InputSubsystem>()->InputMessagePump(hwnd, msg, wParam, lParam);
		return g_pThis->MenuMessagePump(hwnd, msg, wParam, lParam);
	}
	void MenuSubsystem::Init()
	{
		g_pThis = this;
		while (m_hCSGOWindow == NULL)
		{
			m_hCSGOWindow = FindWindowA(g_sValveWindowClass, NULL);
			Sleep(100);
		}
		m_oWndProc = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(
				m_hCSGOWindow,
				GWL_WNDPROC,
				reinterpret_cast<LONG_PTR>(Hooked_WndProc)
			)
		);

		g_Subsystems->Get<InputSubsystem>()->BindHotkey(VK_INSERT, this, &MenuSubsystem::OnToggleMenu);
		/*
			Create menu style & tabs here
		*/
		m_pMenu->Initialize();
		m_pMenu->CreateStyle();
	}
	void MenuSubsystem::OnDeviceLost()
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
	void MenuSubsystem::OnReset(IDirect3DDevice9* ptr, D3DPRESENT_PARAMETERS * pPresentationParameters)
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}
	void MenuSubsystem::OnEndScene(IDirect3DDevice9* ptr)
	{
		if (!m_bGUIInit)
		{
			ImGui_ImplDX9_Init(m_hCSGOWindow, ptr);
			ImGui_ImplDX9_CreateDeviceObjects();
			m_bGUIInit = true;
		}

		if (m_bWindowOpen)
		{
			if (!m_pMenu->Visible())
			{
				auto surface = Interfaces::Instance()->GetInterface<ISurface>()->get();
				m_bWindowOpen = false;
				auto icvar = Interfaces::Instance()->GetInterface<ICvar>();
				auto cl_mouseenable = (*icvar)->FindVar("cl_mouseenable");
				cl_mouseenable->SetValue(!m_bWindowOpen);
			}
		}
		m_pMenu->Render();

	}
	LRESULT ENVY_API MenuSubsystem::MenuMessagePump(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		if (m_bWindowOpen && ImGui_ImplDX9_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;
		return CallWindowProc(m_oWndProc, m_hCSGOWindow, msg, wParam, lParam);
	}

	void MenuSubsystem::OnToggleMenu()
	{
		auto surface = Interfaces::Instance()->GetInterface<ISurface>()->get();
		m_bWindowOpen = !m_bWindowOpen;
		m_pMenu->Toggle();
		if (m_bWindowOpen) {
			surface->UnlockCursor();
		}
		else {
			surface->LockCursor();
		}
		auto icvar = Interfaces::Instance()->GetInterface<ICvar>();
	}
}
