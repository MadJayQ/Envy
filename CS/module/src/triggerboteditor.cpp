#include "menutabs.h"
#include "envydefs.hpp"
#include "options.hpp"
#include "imgui.h"
#include "csgosdk.h"
#include "inputsys.h"

namespace Envy
{
	void DrawTriggerbotTab()
	{

		char keyName[256];

		ImGui::Checkbox("Trigger Bot Enabled", &Options::Instance()->triggerbot_enabled());

		ImGui::Text("Triggerbot Toggle:");

		ImGui::SameLine();

		sprintf(keyName, "%d", Options::Instance()->toggle_triggerbot());

		if (ImGui::Button(keyName, ImVec2(50, 0)))
		{
			g_Subsystems->Get<InputSubsystem>()->ConsumeNextKeyInput(&Options::Instance()->toggle_triggerbot());
		}

		ImGui::Checkbox("Triggerbot Random Delay", &Options::Instance()->triggerbot_delay_random());

		if (!Options::Instance()->triggerbot_delay_random())
		{
			ImGui::SliderFloat("Triggerbot Delay: ", &Options::Instance()->triggerbot_delay(), 0.f, 500.f);
		}
		else
		{
			ImGui::SliderFloat("Triggerbot Delay - Min: ", &Options::Instance()->triggerbot_delay_min(), 0.f, 500.f);
			ImGui::SliderFloat("Triggerbot Delay - Max: ", &Options::Instance()->triggerbot_delay_max(), 0.f, 500.f);
		}

		ImGui::Separator();

		/*
			Hitbox selector.
		*/
		auto hsvector = &Options::Instance()->hitscan_map;
		HitboxSelector::s_AvailableHitboxes.clear();
		ImGui::BeginGroup();
		{
			ImGui::Text("Presets");
			ImGui::ListBoxHeader("##Presets", ImVec2{ 150.f, 300.f });
			{
				int idx = 0;
				for (auto preset : *hsvector)
				{
					if (ImGui::Selectable(preset.first.c_str(), &HitboxSelector::s_SelectedHitscan[idx]))
					{
						Options::Instance()->selected_hitscan_preset() = idx;
					}
					HitboxSelector::s_SelectedHitscan[idx] = (idx == Options::Instance()->selected_hitscan_preset());
					idx++;
				}
			}
			ImGui::ListBoxFooter();
			if (ImGui::Button("+", ImVec2{ (150.f / 2.f) / 2.f, 0.f }))
			{
				memset(HitboxSelector::buf, '\0', 256 * sizeof(char));
				ImGui::OpenPopup("New Hitscan Option");
				ImGui::SetNextWindowSize(ImVec2{ 150, 150 });
			}
			bool open = true;
			if (ImGui::BeginPopupModal("New Hitscan Option", &open, ImGuiWindowFlags_NoResize))
			{
				ImGui::Text("Name:");
				ImGui::SameLine();
				ImGui::InputText("##name", HitboxSelector::buf, 256);
				if (ImGui::Button("Ok"))
				{
					HitboxSelector::s_AvailableIdx = 0;
					HitboxSelector::s_SelectedIdx = 0;
					hsvector->push_back(
						std::make_pair<std::string, std::vector<int>>(std::string(HitboxSelector::buf), {})
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
						memset(HitboxSelector::s_SelectedHitscan, false, 32);
						HitboxSelector::s_SelectedHitscan[Options::Instance()->selected_hitscan_preset()] = true;
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
		for (auto hb : HitboxSelector::s_AllAvailableHitboxes)
		{
			if (std::find(entry->begin(), entry->end(), hb) == entry->end())
				HitboxSelector::s_AvailableHitboxes.push_back(hb);
		}
		ImGui::BeginGroupBox("Available Hitboxes", ImVec2{ 150.f, 300.f });
		{
			int idx = 0;
			for (auto hb : HitboxSelector::s_AvailableHitboxes)
			{
				if (ImGui::Selectable(HitboxSelector::hitboxNames[hb].c_str(), &HitboxSelector::s_SelectedHitboxAvailable[idx]))
				{
					HitboxSelector::s_AvailableIdx = idx;
				}
				HitboxSelector::s_SelectedHitboxAvailable[idx] = (HitboxSelector::s_AvailableIdx == idx);
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
				entry->push_back(HitboxSelector::s_AvailableHitboxes.at(HitboxSelector::s_AvailableIdx));
			}
			if (ImGui::Button("<-") && allowEdit)
			{
				entry->erase(
					entry->begin() + HitboxSelector::s_SelectedIdx
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
				if (ImGui::Selectable((HitboxSelector::hitboxNames[hb] + " ").c_str(), &HitboxSelector::s_SelectedHitbox[idx]))
				{
					HitboxSelector::s_SelectedIdx = idx;
				}
				HitboxSelector::s_SelectedHitbox[idx] = (HitboxSelector::s_SelectedIdx == idx);
				idx++;
			}

			// Update selection
			Options::Instance()->triggerbot_hitboxes() = *entry;
		}
		ImGui::EndGroupBox();
		ImGui::BeginGroupBox("Controls", ImVec2{ 500.f, 50.f });
		{
			std::string converted;
			for (auto& s : HitboxSelector::hitboxNames) converted += (s + '\0');
			ImGui::Combo("##Hitbox", &HitboxSelector::sidx, converted.c_str());
			ImGui::SameLine();
			ImGui::Checkbox("Multipoint", &Options::Instance()->aimbot_multipoint()[HitboxSelector::sidx]);
		}
		ImGui::EndGroupBox();
	}
}