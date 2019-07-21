#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"

#include <vector>
#include <memory>

namespace Envy
{

	using MenuTabFunc = void(*)();
	struct MenuTab
	{
		MenuTab(int tabID, const char* tabName, MenuTabFunc draw) :
			m_tabID(tabID), m_szTabName(tabName)
		{
			m_emptyDraw = []() -> void 
			{
				ImGui::Text("How tf did this happen???");
			};
			m_draw = (draw == nullptr) ? m_emptyDraw : draw;
		}

		void operator ()() { m_draw(); }

		void SetDraw(MenuTabFunc& fn)
		{
			m_draw = fn;
		}

		const char* GetName() const { return m_szTabName; }
	private:
		MenuTabFunc m_draw;
		MenuTabFunc m_emptyDraw;

		int m_tabID;
		const char* m_szTabName;
	};
	struct MenuNav
	{
		MenuNav(int navID, const char* navName) : 
			m_navID(navID), m_szNavName(navName), m_selectedTab(0)
		{

		}
		MenuTab* CreateTab(const char* name, MenuTabFunc draw)
		{
			MenuTab tab(m_tabs.size(), name, draw);
			m_tabs.push_back(tab);
			return &m_tabs.back();
		}

		void SetSelectedTab(int id) { m_selectedTab = id; }
		int GetSelectedTab() const { return m_selectedTab; }
		void Draw() 
		{
			ImGui::BeginGroupBox("##content");
			{
				m_tabs[m_selectedTab]();
			}
			ImGui::EndGroupBox();
		}

		const char* GetName() const { return m_szNavName; }
		int NumTabs() const { return m_tabs.size(); }

		MenuTab* GetTab(int idx) { return &m_tabs[idx]; }

		static constexpr float SizeW = 150.f;
		static constexpr float SizeH = 50.f;

	private:
		std::vector<MenuTab> m_tabs;

		int m_navID;
		int m_selectedTab;
		const char* m_szNavName;
	};

	struct MenuNavBar
	{
		MenuNavBar()
		{

		}
		MenuNav* CreateNavItem(const char* name)
		{
			MenuNav nav(m_navItems.size(), name);
			m_navItems.push_back(nav);
			return& m_navItems.back();
		}
		void DrawNavBar();
		void DrawSelectedNavItem();

		int GetSelectedItem() { return m_selectedNavItem; }
		void SetSelectedItem(int item) { m_selectedNavItem = item; }

		ImVec2 Size()
		{
			auto window = ImGui::GetCurrentWindow();
			float padding = 10.f;
			float w = 170.f;
			float h = (window->ContentsRegionRect.GetHeight() - (padding * 2.f)) - 25.f;
			return ImVec2{ w, h };
		}
	private:
		std::vector<MenuNav> m_navItems;
		int m_selectedNavItem;
	};

	class Menu
	{
	public:
		Menu()
		{
			m_visible = false;
			m_externalWindowVisible = false;
			m_pNavBar = std::make_unique<MenuNavBar>();
		}

		~Menu()
		{
			m_pNavBar.reset();
			m_pNavBar.release();
		}

		void Initialize();
		void CreateStyle();
		void Render();
		void Toggle() { m_visible = !m_visible; }
		void ToggleExternal(bool val) { m_externalWindowVisible = val; }
		bool Visible() { return m_visible; }
	private:
		bool m_visible;
		bool m_externalWindowVisible;
		ImGuiStyle m_style;

		std::unique_ptr<MenuNavBar> m_pNavBar;
	};
}