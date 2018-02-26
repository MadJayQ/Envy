#include "menutabs.h"
#include "envydefs.hpp"
#include "options.hpp"
#include "imgui.h"
#include "csgosdk.h"

using namespace Envy::SourceEngine;

namespace Labels
{
	std::array<std::string, 20> hitboxNames = 
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
}
namespace Envy
{
	CACHED std::vector<int> s_AllAvailableHitboxes = { ALLHITBOXES() };
	CACHED char buf[256] = { '\0' };
	CACHED bool s_SelectedHitscan[32];
	CACHED bool s_SelectedHitbox[HITBOX_MAX] = { false };
	CACHED bool s_SelectedHitboxAvailable[HITBOX_MAX] = { false };
	CACHED int s_AvailableIdx = 0;
	CACHED int s_SelectedIdx = 0;
	CACHED int sidx = 0;
	CACHED std::vector<int> s_AvailableHitboxes;
	void DrawHitscanTab()
	{
		auto hsvector = &Options::Instance()->hitscan_map;
		s_AvailableHitboxes.clear();
		ImGui::BeginGroup();
		{
			ImGui::Text("Presets");
			ImGui::ListBoxHeader("##Presets", ImVec2{ 150.f, 300.f});
			{
				int idx = 0;
				for (auto preset : *hsvector)
				{
					if (ImGui::Selectable(preset.first.c_str(), &s_SelectedHitscan[idx]))
					{
						Options::Instance()->selected_hitscan_preset() = idx;
					}
					s_SelectedHitscan[idx] = (idx == Options::Instance()->selected_hitscan_preset());
					idx++;
				}
			}
			ImGui::ListBoxFooter();
			if (ImGui::Button("+", ImVec2{ (150.f / 2.f) / 2.f, 0.f }))
			{
				memset(buf, '\0', 256 * sizeof(char));
				ImGui::OpenPopup("New Hitscan Option");
				ImGui::SetNextWindowSize(ImVec2{ 150, 150 });
			}
			bool open = true;
			if (ImGui::BeginPopupModal("New Hitscan Option", &open, ImGuiWindowFlags_NoResize))
			{
				ImGui::Text("Name:");
				ImGui::SameLine();
				ImGui::InputText("##name", buf, 256);
				if (ImGui::Button("Ok"))
				{
					s_AvailableIdx = 0;
					s_SelectedIdx = 0;
					hsvector->push_back(
						std::make_pair<std::string, std::vector<int>>(std::string(buf), {})
					);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("-", ImVec2{ (150.f / 2.f) / 2.f, 0.f }))
			{
				if (Options::Instance()->selected_hitscan_preset() != 0) //Don't let us delete our all preset
				{
					hsvector->erase(hsvector->begin() + Options::Instance()->selected_hitscan_preset());
					if (Options::Instance()->selected_hitscan_preset() >= hsvector->size())
					{
						Options::Instance()->selected_hitscan_preset() = hsvector->size() - 1;
						memset(s_SelectedHitscan, false, 32);
						s_SelectedHitscan[Options::Instance()->selected_hitscan_preset()] = true;
					}
				}
			}
			ImGui::SameLine();
			ImGui::Button("Save", ImVec2{ 150.f / 2.f, 0.f });
		}
		ImGui::EndGroup();
		ImGui::SameLine(170.f);
		auto entry = &hsvector->at(Options::Instance()->selected_hitscan_preset()).second;
		int counter = 0;
		for (auto hb : s_AllAvailableHitboxes)
		{
			if (std::find(entry->begin(), entry->end(), hb) == entry->end())
				s_AvailableHitboxes.push_back(hb);
		}
		ImGui::BeginGroupBox("Available Hitboxes", ImVec2{ 150.f, 300.f });
		{
			int idx = 0;
			for (auto hb : s_AvailableHitboxes)
			{
				if (ImGui::Selectable(Labels::hitboxNames[hb].c_str(), &s_SelectedHitboxAvailable[idx]))
				{
					s_AvailableIdx = idx;
				}
				s_SelectedHitboxAvailable[idx] = (s_AvailableIdx == idx);
				idx++;
			}
		}
		ImGui::EndGroupBox();
		ImGui::SameLine(0.f, 15.f);
		ImGui::BeginGroup();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 100.f });
			ImGui::Spacing();
			ImGui::PopStyleVar();
			bool allowEdit = (Options::Instance()->selected_hitscan_preset() != 0);
			if (ImGui::Button("->") && allowEdit)
			{
				entry->push_back(s_AvailableHitboxes.at(s_AvailableIdx));
			}
			if (ImGui::Button("<-") && allowEdit)
			{
				entry->erase(
					entry->begin() + s_SelectedIdx
				);
			}
		}
		ImGui::EndGroup();
		ImGui::SameLine(0.f, 15.f);
		ImGui::BeginGroupBox("Selected Hitboxes", ImVec2{ 150.f, 300.f });
		{
			int idx = 0;
			for (auto hb : *entry)
			{
				if (ImGui::Selectable((Labels::hitboxNames[hb] + " ").c_str(), &s_SelectedHitbox[idx]))
				{
					s_SelectedIdx = idx;
				}
				s_SelectedHitbox[idx] = (s_SelectedIdx == idx);
				idx++;
			}
		}
		ImGui::EndGroupBox();
		ImGui::BeginGroupBox("Controls", ImVec2{ 500.f, 50.f });
		{
			std::string converted;
			for (auto& s : Labels::hitboxNames) converted += (s + '\0');
			ImGui::Combo("##Hitbox", &sidx, converted.c_str());
			ImGui::SameLine();
			ImGui::Checkbox("Multipoint", &Options::Instance()->aimbot_multipoint()[sidx]);
		}
		ImGui::EndGroupBox();
	}
}