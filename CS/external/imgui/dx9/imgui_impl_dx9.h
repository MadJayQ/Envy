#pragma once

#include <d3d9.h>
#include "imgui.h"

IMGUI_API bool  ImGui_ImplDX9_Init(void* hwnd, IDirect3DDevice9* device);
IMGUI_API void  ImGui_ImplDX9_Shutdown();
IMGUI_API void  ImGui_ImplDX9_NewFrame();

IMGUI_API void  ImGui_ImplDX9_InvalidateDeviceObjects();
IMGUI_API bool  ImGui_ImplDX9_CreateDeviceObjects();

IMGUI_API LRESULT   ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);