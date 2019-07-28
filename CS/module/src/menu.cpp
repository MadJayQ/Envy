#include "..\include\menu.h"
#include "imgui_internal.h"
#include "menutabs.h"

#include "camerasys.h"


namespace Envy
{


	void MenuNavBar::DrawNavBar()
	{
		auto& style = ImGui::GetStyle();
		int counter = 0;
		for (auto item : m_navItems)
		{
			bool highlight = (GetSelectedItem() == counter);
			if (highlight) ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
			if (ImGui::Button(item.GetName(), ImVec2{ MenuNav::SizeW, MenuNav::SizeH }))
			{
				SetSelectedItem(counter);
			}
			if (highlight) ImGui::PopStyleColor();
			counter++;
		}
	}
	void MenuNavBar::DrawSelectedNavItem()
	{
		auto selectedNav = &m_navItems[m_selectedNavItem];
		int numTabs = selectedNav->NumTabs();
		if (numTabs == 0)
		{
			ImGui::Text("There are no tabs here?");
		}
		else if (numTabs == 1)
		{
			selectedNav->SetSelectedTab(0);
			selectedNav->Draw();
		}
		else
		{
			auto& style = ImGui::GetStyle();
			float w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;
			for (int tabIdx = 0; tabIdx < numTabs; tabIdx++)
			{
				auto tab = selectedNav->GetTab(tabIdx);
				bool highlight = tabIdx == selectedNav->GetSelectedTab();
				if (highlight) ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
				if (ImGui::Button(tab->GetName(), ImVec2{ w / numTabs, 25.f }))
				{
					selectedNav->SetSelectedTab(tabIdx);
				}
				if (tabIdx < numTabs - 1) ImGui::SameLine();
				if (highlight) ImGui::PopStyleColor();
			}
			selectedNav->Draw();
		}
	}

	void Menu::Initialize()
	{
		// Aimbot tab. 
		auto aimnav = m_pNavBar->CreateNavItem("Aimbot");
		aimnav->CreateTab("Rage", DrawRageTab);
		aimnav->CreateTab("Legit", DrawLegitTab);
		aimnav->CreateTab("Triggerbot", DrawTriggerbotTab);

		// Visuals tab.
		auto visualnav = m_pNavBar->CreateNavItem("Visuals");
		visualnav->CreateTab("ESP", DrawESPTab);
		visualnav->CreateTab("Glow", DrawGlowTab);
		visualnav->CreateTab("Chams", nullptr);
		visualnav->CreateTab("Misc", DrawMiscTab);
		m_pNavBar->SetSelectedItem(0);

		// HvH Tab
		auto hvhnav = m_pNavBar->CreateNavItem("HvH");
		hvhnav->CreateTab("Anti-Aim", DrawAntiAimTab);
		hvhnav->CreateTab("Hitscan Editor", DrawHitscanTab);

	}


	void Menu::Render()
	{
		ImGui_ImplDX9_NewFrame();
		ImGui::GetIO().MouseDrawCursor = m_visible;
		auto ctx = ImGui::GetCurrentContext();
		auto saved_style = ctx->Style;
		ImGui::PushStyle(m_style);
		ImGui::SetNextWindowPos(ImVec2{ 0,0 }, ImGuiSetCond_Once);
		ImGui::SetNextWindowSize(ImVec2{ 750, 600 }, ImGuiSetCond_Once);
		if (m_visible)
		{

			if (ImGui::Begin("envycheat.cc - By MadJayQ",
				&m_visible,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_ShowBorders |
				ImGuiWindowFlags_NoResize))
			{
				ImVec2 navBarSize = m_pNavBar->Size();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2::Zero);
				ImGui::BeginGroupBox("##navbar", navBarSize);
				{
					m_pNavBar->DrawNavBar();
				}
				ImGui::EndGroupBox();
				ImGui::PopStyleVar();
				ImGui::SameLine();
				ImGui::BeginGroupBox("##body", ImVec2{ 0.f, navBarSize.y });
				{
					m_pNavBar->DrawSelectedNavItem();
				}
				ImGui::EndGroupBox();
				if (ImGui::Button("YEET", ImVec2{ 150.f, 25.f }))
				{

				}
				ImGui::SameLine();
				if (ImGui::Button("External Camera Window", ImVec2{ 150.f, 25.f }))
				{
					ToggleExternal(true);
				}
				ImGui::End();
			}
		}
		if (m_externalWindowVisible)
		{
			if (ImGui::Begin("envycheat.cc - External Camera", &m_externalWindowVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders))
			{
				auto cameraSys = g_Subsystems->Get<CameraSubsystem>();
				auto size = ImGui::GetWindowSize();
				cameraSys->SetCameraExtents(size.x, size.y);
				auto engineClient = Interfaces::Instance()->GetInterface<IVEngineClient>();
				if ((*engineClient)->IsConnected() && (*engineClient)->IsInGame())
				{
					IDirect3DTexture9* texture = cameraSys->GetRawCameraTexture();
					ImVec2 uv_min(0.f, 0.f);
					ImVec2 uv_max(1.f, 1.f);

					D3DSURFACE_DESC textureDesc;
					texture->GetLevelDesc(0, &textureDesc);

					ImGui::Image(texture, ImVec2((float)textureDesc.Width, (float)textureDesc.Height), uv_min, uv_max);
				}
				else
				{
					ImGui::Text("MISSION FAILED");
				}
				ImGui::End();
			}
		}
		ImGui::PopStyle();
		ImGui::Render();
	}


	void Menu::CreateStyle()
	{
		m_style.Alpha = 1.0f;											// Global alpha applies to everything in ImGui
		m_style.WindowPadding = ImVec2(10, 10);                         // Padding within a window
		m_style.WindowMinSize = ImVec2(100, 100);						// Minimum window size
		m_style.WindowRounding = 0.0f;									// Radius of window corners rounding. Set to 0.0f to have rectangular windows
		m_style.WindowTitleAlign = ImVec2(0.0f, 0.5f);                  // Alignment for title bar text
		//m_style.FrameBorderSize = 1.0f;								// Size of our frame border
		m_style.ChildWindowRounding = 0.0f;								// Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
		m_style.FramePadding = ImVec2(5, 5);							// Padding within a framed rectangle (used by most widgets)
		m_style.FrameRounding = 0.0f;									// Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
		m_style.ItemSpacing = ImVec2(5, 5);								// Horizontal and vertical spacing between widgets/lines
		m_style.ItemInnerSpacing = ImVec2(4, 4);                        // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
		m_style.TouchExtraPadding = ImVec2(0, 0);                       // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
		m_style.IndentSpacing = 21.0f;									// Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
		m_style.ColumnsMinSpacing = 6.0f;                               // Minimum horizontal spacing between two columns
		m_style.ScrollbarSize = 16.0f;									// Width of the vertical scrollbar, Height of the horizontal scrollbar
		m_style.ScrollbarRounding = 9.0f;                               // Radius of grab corners rounding for scrollbar
		m_style.GrabMinSize = 10.0f;									// Minimum width/height of a grab box for slider/scrollbar
		m_style.GrabRounding = 0.0f;									// Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
		m_style.ButtonTextAlign = ImVec2(0.5f, 0.5f);					// Alignment of button text when button is larger than text.
		m_style.DisplayWindowPadding = ImVec2(22, 22);                  // Window positions are clamped to be IsVisible within the display area by at least this amount. Only covers regular windows.
		m_style.DisplaySafeAreaPadding = ImVec2(4, 4);                  // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
		m_style.AntiAliasedLines = true;                                // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
		m_style.AntiAliasedShapes = true;                               // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
		m_style.CurveTessellationTol = 1.25f;                           // Tessellation tolerance. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.

		m_style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.00f);
		m_style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		m_style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		m_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		m_style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.22f, 0.64f, 0.21f, 0.00f);
		m_style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
		m_style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
		m_style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		m_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		m_style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
		m_style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
		m_style.Colors[ImGuiCol_TitleBg] = ImVec4(0.22f, 0.64f, 0.21f, 0.70f);
		m_style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 0.70f);
		m_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		m_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
		m_style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
		m_style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
		m_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
		m_style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
		m_style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.60f, 0.90f, 0.50f);
		m_style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		m_style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
		m_style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.f);
		m_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.64f, 0.21f, 0.70f);
		m_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.93f, 0.26f, 0.83f);
		m_style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
		m_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		m_style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
		m_style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		m_style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
		m_style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
		m_style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		m_style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		m_style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		m_style.Colors[ImGuiCol_CloseButton] = ImVec4(0.16f, 0.16f, 0.16f, 1.f);
		m_style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.26f, 0.93f, 0.26f, 0.83f);
		m_style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.20f, 0.00f, 0.83f);
		m_style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		m_style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		m_style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		m_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		m_style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	}
}