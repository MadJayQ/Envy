#include "menutabs.h"

#include "imgui.h"
#include "csgosdk.h"
#include "structs.h"
#include "options.hpp"

#include "subsystem.h"
#include "inputsys.h"

#include "antiaim.h"

#include <vector>
#include <map>

namespace Envy
{

	namespace SourceEngine
	{
		auto hb = HITBOX_HEAD;
		std::string hitgroups[] = { "Head", "Chest", "Left Upper", "Left Lower", "Right Upper", "Right Lower" };
		std::string hitboxmap[][6] =
		{
			{"HITBOX_HEAD", "HITBOX_NECK", "HITBOX_LOWER_NECK"},
			{"Swag"},
			{ "Head" },
			{ "Swag" },
			{ "Head" },
			{ "Swag" }
		};
	}

	// Define initial vars for hitbox selector. 
	char HitboxSelector::buf[256] = { '\0' };
	std::vector<int> HitboxSelector::s_AllAvailableHitboxes = { ALLHITBOXES() };
	bool HitboxSelector::s_SelectedHitbox[HITBOX_MAX] = { false };
	bool HitboxSelector::s_SelectedHitboxAvailable[HITBOX_MAX] = { false };
	int HitboxSelector::s_AvailableIdx = 0;
	int HitboxSelector::s_SelectedIdx = 0;
	int HitboxSelector::sidx = 0;
	bool HitboxSelector::s_SelectedHitscan[32] = { false };
	std::vector<int> HitboxSelector::s_AvailableHitboxes = std::vector<int>();
	std::array<std::string, 20> HitboxSelector::hitboxNames =
	{
		"HEAD",
		"NECK",
		"LOWER NECK",
		"PELVIS",
		"STOMACH",
		"LOWER CHEST",
		"CHEST",
		"UPPER CHEST",
		"RIGHT THIGH",
		"LEFT THIGH",
		"RIGHT CALF",
		"LEFT CALF",
		"RIGHT FOOT",
		"LEFT FOOT",
		"RIGHT HAND",
		"LEFT HAND",
		"RIGHT UPPER ARM",
		"RIGHT FOREARM",
		"LEFT UPPER ARM",
		"LEFT FOREARM"
	};

	void DrawRageTab()
	{
		ImGui::Checkbox("Enable", &Options::Instance()->aimbot_enable());
		ImGui::Checkbox("Silent", &Options::Instance()->aimbot_silent_aim());
		ImGui::Checkbox("Auto-Fire", &Options::Instance()->aimbot_autofire());
		ImGui::Checkbox("RCS", &Options::Instance()->aimbot_enable_rcs());
		ImGui::Checkbox("Allow Penetration", &Options::Instance()->aimbot_allow_penetration());
		ImGui::SliderFloat("Maximum FOV", &Options::Instance()->aimbot_max_fov(), 1.f, 180.f);

		ImGui::Checkbox("Force Toggle", &Options::Instance()->aimbot_force_toggle());
		ImGui::Text("Aimbot Toggle:");
		ImGui::SameLine();
		char keyName[256];
		sprintf(keyName, "%d", Options::Instance()->toggle_aimbot());
		if (ImGui::Button(keyName, ImVec2(50, 0)))
		{
			g_Subsystems->Get<InputSubsystem>()->ConsumeNextKeyInput(&Options::Instance()->toggle_aimbot());
		}
	}
	void DrawLegitTab()
	{
		ImGui::Checkbox("Aim-Lock", &Options::Instance()->aimbot_autoaim());
		ImGui::Checkbox("Smooth", &Options::Instance()->aimbot_enable_smooth());
		ImGui::SliderFloat("RCS Pitch", &Options::Instance()->rcs_pitch(), 0.1f, 2.f);
		ImGui::SliderFloat("RCS Yaw", &Options::Instance()->rcs_yaw(), 0.1f, 2.f);
		ImGui::SliderInt("Smooth Factor", &Options::Instance()->smooth_factor(), 1, 10);

	}
	void DrawESPTab()
	{
		ImGui::BeginGroupBox("Player Options", ImVec2{ 150.f, 350.f });
		{
			ImGui::Checkbox("ESP", &Options::Instance()->esp_enabled());
			ImGui::Checkbox("Bounding Box", &Options::Instance()->esp_bbox());
			ImGui::Checkbox("Health Bar", &Options::Instance()->esp_hp_bar());
			ImGui::Checkbox("Armor Bar", &Options::Instance()->esp_armor_bar());
			ImGui::Checkbox("Headshot Circle", &Options::Instance()->esp_skeleton());
		}
		ImGui::EndGroupBox();
		ImGui::SameLine(0.f, 25.f);
		ImGui::BeginGroupBox("Weapon Options", ImVec2{ 150.f, 350.f });
		{
			ImGui::Checkbox("Show Dropped Weapons", &Options::Instance()->esp_dropped_weapons());
			ImGui::Checkbox("Show Dropped C4", &Options::Instance()->esp_dropped_c4());
			ImGui::Checkbox("Weapon Bounding Box", &Options::Instance()->esp_weapon_bbox());
			ImGui::Checkbox("Weapon Name", &Options::Instance()->esp_weapon_name());
		}
		ImGui::EndGroupBox();
	}
	void DrawGlowTab()
	{
		ImGui::Checkbox("Glow", &Options::Instance()->glow_enabled());
		ImGui::SliderFloat("R", &Options::Instance()->glow_r(), 0.1, 1.f);
		ImGui::SliderFloat("G", &Options::Instance()->glow_g(), 0.1, 1.f);
		ImGui::SliderFloat("B", &Options::Instance()->glow_b(), 0.1, 1.f);
		ImGui::SliderFloat("A", &Options::Instance()->glow_a(), 0.1, 1.f);
		ImGui::Checkbox("Render When Occluded", &Options::Instance()->glow_rwo());
		ImGui::Checkbox("Render when Unoccluded", &Options::Instance()->glow_rwu());
	}
	void DrawChamsTab()
	{

	}
	void DrawMiscTab()
	{
		ImGui::Checkbox("Spread Circle", &Options::Instance()->draw_spread_circle());
		ImGui::Checkbox("Random Fuckery", &Options::Instance()->misc_lag_exploit());
		ImGui::Checkbox("Resolver Last LBY", &Options::Instance()->resolver_last_lby());
		ImGui::Checkbox("Resolver Force LBY", &Options::Instance()->resolver_force_lby());
	}
	void DrawAntiAimTab()
	{
		ImGui::Checkbox("AA-Enable", &Options::Instance()->aa_enabled());
		ImGui::SameLine();
		ImGui::Checkbox("LBYT Update Prediction", &Options::Instance()->aa_prevent_lby());
		auto pitches = AntiAims::AntiAimList("Pitch");
		auto yaws = AntiAims::AntiAimList("Yaw");
		auto compiledYaws = yaws.Compile();
		ImGui::Combo("Pitch AA", &Options::Instance()->aa_pitch(), pitches.Compile());
		ImGui::Combo("Yaw AA", &Options::Instance()->aa_yaw(), compiledYaws);
		ImGui::Combo("Yaw Fake", &Options::Instance()->aa_yaw_fake(), compiledYaws);
		ImGui::Text("Custom Override - ");
		ImGui::SameLine();
		ImGui::Checkbox("Enable?", &Options::Instance()->aa_custom_override());
		ImGui::Separator();
		ImGui::SliderFloat("Fake Offset", &Options::Instance()->aa_fake_offset(), -180.f, 180.f);
		ImGui::SliderFloat("Real Offset", &Options::Instance()->aa_real_offset(), -180.f, 180.f);

		g_sActiveYaw = yaws[Options::Instance()->aa_yaw()];
		g_sActivePitch = pitches[Options::Instance()->aa_pitch()];
		g_sActiveFakeYaw = yaws[Options::Instance()->aa_yaw_fake()];

	}
}