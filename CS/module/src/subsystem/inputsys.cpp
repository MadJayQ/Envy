#include "..\..\include\subsystem\inputsys.h"

namespace Envy
{
	void InputSubsystem::Init()
	{
		for (int i = 0; i < m_keyMap.size(); i++)
		{
			m_keyMap[i] = KeyState::None;
		}
	}
	bool InputSubsystem::InputMessagePump(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		case WM_XBUTTONUP:
			return ProcessMouseMessage(msg, wparam, lparam);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return ProcessKeyboardMessage(msg, wparam, lparam);
		default:
			return false;
		}
	}
	void InputSubsystem::ConsumeNextKeyInput(int* store)
	{
		if (!m_bWaitingForKey) m_bWaitingForKey = true;
		m_pNextKey = store;
	}
	bool InputSubsystem::ProcessMouseMessage(UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto button = VK_LBUTTON;
		auto state = KeyState::None;
		switch (msg)
		{
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			state = msg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
			button = VK_MBUTTON;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			state = msg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
			button = VK_RBUTTON;
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			state = msg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
			button = VK_LBUTTON;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			state = msg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
			button = (HIWORD(wparam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
			break;
		default:
			return false;

		}

		if (state == KeyState::Up && m_keyMap[button] == KeyState::Down)
			m_keyMap[button] = KeyState::Pressed;
		else
			m_keyMap[button] = state;
		return true;
	}
	bool InputSubsystem::ProcessKeyboardMessage(UINT msg, WPARAM wparam, LPARAM lparam)
	{

		auto key = wparam;
		auto state = KeyState::None;

		switch (msg) 
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			state = KeyState::Down;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			state = KeyState::Up;
			break;
		default:
			return false;
		}

		if (state == KeyState::Up && m_keyMap[key] == KeyState::Down)
		{
			if (m_bWaitingForKey)
			{
				if (m_pNextKey != NULL)
				{
					*m_pNextKey = key;
				}

				m_pNextKey = NULL;
				m_bWaitingForKey = false;
			}
			m_keyMap[key] = KeyState::Pressed;
			auto callback = m_hotkeys[key];
			if (callback) callback();
		} 
		else
		{
			m_keyMap[key] = state;
		}

		return true;
	}
}
