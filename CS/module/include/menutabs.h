#pragma once
#include "studio.hpp"
#include <vector>
#include <string>
using namespace Envy::SourceEngine;
namespace Envy
{
	void DrawRageTab();
	void DrawLegitTab();
	void DrawTriggerbotTab();
	void DrawESPTab();
	void DrawGlowTab();
	void DrawChamsTab();
	void DrawMiscTab();
	void DrawHitscanTab();
	void DrawAntiAimTab();

	class HitboxSelector {
	public:
		static std::array<std::string, 20> hitboxNames;
		static char buf[256];
		static std::vector<int> s_AllAvailableHitboxes;
		static bool s_SelectedHitbox[HITBOX_MAX];
		static bool s_SelectedHitboxAvailable[HITBOX_MAX];
		static bool s_SelectedHitscan[32];
		static int s_AvailableIdx;
		static int s_SelectedIdx;
		static int sidx;
		static std::vector<int> s_AvailableHitboxes;
	};
}