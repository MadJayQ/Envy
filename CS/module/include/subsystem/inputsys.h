#pragma once

#include "subsystem.h"
#include <functional>
#include <array>

namespace Envy
{
	enum class KeyState
	{
		None = 1,
		Down,
		Up,
		Pressed
	};

	class InputSubsystem : public ICheatSubsystem
	{
	public:
		InputSubsystem() {}
		virtual void Init() override;

		bool InputMessagePump(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);

		template<typename F, typename O>
		void BindHotkey(uint32_t vk, O object, F func)
		{
			m_hotkeys[vk] = std::bind(func, object);
		}

		KeyState GetKeyState(int vk) { return (vk >= 0 && vk <= 255) ? m_keyMap[vk] : KeyState::None; }

		void ConsumeNextKeyInput(int* store);

	private:
		bool ProcessMouseMessage(UINT msg, WPARAM wparam, LPARAM lparam);
		bool ProcessKeyboardMessage(UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		std::array<std::function<void(void)>, 256> m_hotkeys;
		std::array<KeyState, 256> m_keyMap;
		bool m_bWaitingForKey = false;
		int* m_pNextKey = NULL;

	};
}
