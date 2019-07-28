#pragma once
#include "singleton.hpp"
#include "enums.hpp"
#include "studio.hpp"
#include <map>
#include <vector>
#include <array>

#define OPTION(type, name, defaultvalue)							\
	type& name##() const											\
	{																\
		static type _##name = defaultvalue;							\
		return _##name;												\
	}

#define TOGGLEKEY(name, defaultvalue) \
	int& toggle_##name##() \
	{ \
		static int _##name = defaultvalue; \
		return _##name; \
	} \

namespace Labels
{
	namespace Comboboxes
	{

	}
}

using namespace Envy::SourceEngine;
class Options : public Singleton<Options>
{
public:
	Options() 
	{
		hitscan_map = 
		{
			std::make_pair<std::string, std::vector<int>>("ALL", {ALLHITBOXES()}),
			std::make_pair<std::string, std::vector<int>>("Head Only", { HITBOX_HEAD }),
			std::make_pair<std::string, std::vector<int>>("Test", {MOSTHITBOXES()})
		};
	}

	OPTION(bool, esp_enabled, false);
	OPTION(bool, esp_bbox, false);
	OPTION(bool, esp_hp_bar, false);
	OPTION(bool, esp_armor_bar, false);
	OPTION(bool, esp_skeleton, false);
	OPTION(bool, esp_dropped_weapons, false);
	OPTION(bool, esp_dropped_c4, false);
	OPTION(bool, esp_weapon_bbox, false);
	OPTION(bool, esp_weapon_name, false);
	OPTION(bool, draw_spread_circle, false);
	OPTION(int, selected_hitscan_preset, 1);
	OPTION(int, smooth_factor, 2);
	OPTION(bool, aimbot_enable_rcs, false);
	OPTION(bool, aimbot_enable_smooth, true);
	OPTION(bool, aimbot_enable, false);
	OPTION(bool, aimbot_force_toggle, false);
	OPTION(bool, aimbot_autofire, false);
	OPTION(bool, aimbot_autoaim, false);
	OPTION(bool, aimbot_silent_aim, true);
	OPTION(float, rcs_pitch, 2.f);
	OPTION(float, rcs_yaw, 2.f);
	OPTION(float, aimbot_max_fov, 180.f);
	OPTION(bool, aimbot_allow_penetration, false);
	OPTION(bool, aa_enabled, false);
	OPTION(float, aa_fake_offset, 90.f);
	OPTION(float, aa_real_offset, -90.f);
	OPTION(float, aa_lby_offset, 90.f);
	OPTION(int, aa_pitch, 0);
	OPTION(int, aa_yaw, 0);
	OPTION(int, aa_yaw_fake, 0);
	OPTION(bool, aa_prevent_lby, false);
	OPTION(bool, aa_custom_override, false);

	OPTION(bool, misc_watermark, true);
	OPTION(bool, misc_visualize_aa, true);

	OPTION(bool, glow_enabled, false);
	OPTION(float, glow_r, 1.0);
	OPTION(float, glow_g, 0.0);
	OPTION(float, glow_b, 0.0);
	OPTION(float, glow_a, 0.0);
	OPTION(bool, glow_rwo, true);
	OPTION(bool, glow_rwu, false);
	OPTION(bool, glow_bloom, false);
	OPTION(int, glow_style, 0);

	OPTION(bool, misc_lag_exploit, false);

	OPTION(bool, resolver_force_lby, false);
	OPTION(bool, resolver_last_lby, false);

	OPTION(bool, triggerbot_enabled, false);
	OPTION(float, triggerbot_delay, 0.f);
	OPTION(bool, triggerbot_delay_random, false);
	OPTION(float, triggerbot_delay_min, 1.f);
	OPTION(float, triggerbot_delay_max, 500.f);
	OPTION(std::vector<int>, triggerbot_hitboxes, std::vector<int>());

	OPTION(bool, chams_enabled, false);
	OPTION(bool, chams_reflective, false);

	TOGGLEKEY(aimbot, 18);
	TOGGLEKEY(triggerbot, 18);

	std::vector<std::pair<std::string, std::vector<int>>> hitscan_map;

	std::array<bool, HITBOX_MAX>& aimbot_multipoint() const
	{
		static std::array<bool, HITBOX_MAX> _aimbot_multipoint = { false };
		return _aimbot_multipoint;
	}
	
};