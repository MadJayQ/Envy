#include "..\..\include\features\esp.h"
#include "envydefs.hpp"
#include "envy.h"
#include "mathfuncs.h"
#include "ehandle.hpp"

#include "options.hpp"
#include <functional>

using namespace Envy::SourceEngine;

namespace Envy
{
	namespace Fonts
	{
		vgui::HFont esp;
		vgui::HFont defuse;
		vgui::HFont weapons;
		vgui::HFont watermark;
		vgui::HFont watermark_smaller;

		namespace Icons
		{
			static constexpr wchar_t ICON_ARMOR = L'a';
			static constexpr wchar_t ICON_HEALTH = L'b';
			static constexpr wchar_t ICON_C4 = L'd';
			static constexpr wchar_t ICON_DEFUSER = L'e';
			static constexpr wchar_t ICON_TIMER = L'f';
			static constexpr wchar_t ICON_KEV = L'r';
			static constexpr wchar_t ICON_KEVHELM = L's';

			std::map<int, wchar_t> weapon_icons = 
			{
				{ WEAPON_MP9, 'A' },
				{ WEAPON_UMP45, 'B' },
				{ WEAPON_TASER, 'C' },
				{ WEAPON_NOVA, 'D' },
				{ WEAPON_KNIFE_TACTICAL, 'E' },
				{ WEAPON_P90, 'F' },
				{ WEAPON_HKP2000, 'G' },
				{ WEAPON_REVOLVER, 'H' },
				{ WEAPON_MAG7, 'I' },
				{ WEAPON_SAWEDOFF, 'J' },
				{ WEAPON_SCAR20, 'K' },
				{ WEAPON_SG556, 'L' },
				{ WEAPON_XM1014, 'M' },
				{ WEAPON_USP_SILENCER, 'N' },
				{ WEAPON_MOLOTOV, 'O' },
				{ WEAPON_MP7, 'P' },
				{ WEAPON_KNIFE_PUSH, 'Q' },
				{ WEAPON_M4A1, 'R' },
				{ WEAPON_NEGEV, 'S' },
				{ WEAPON_M4A1_SILENCER, 'T' },
				{ WEAPON_MAC10, 'U' },
				{ WEAPON_TEC9, 'V' },
				{ WEAPON_KNIFE_T, 'W' },
				{ WEAPON_SSG08, 'X' },
				{ WEAPON_M249, 'Y' },
				{ WEAPON_SMOKEGRENADE, 'Z' },
				{ WEAPON_FAMAS, 'a' },
				{ WEAPON_KNIFE_GUT, 'b' },
				{ WEAPON_KNIFE_FALCHION, 'c' },
				{ WEAPON_FLASHBANG, 'd' },
				{ WEAPON_AWP, 'e' },
				{ WEAPON_G3SG1, 'f' },
				{ WEAPON_GALILAR, 'g' },
				{ WEAPON_GLOCK, 'h' },
				{ WEAPON_DEAGLE, 'i' },
				{ WEAPON_HEGRENADE, 'j' },
				{ WEAPON_P250,'k' },
				{ WEAPON_INCGRENADE, 'l' },
				{ WEAPON_KNIFE_M9_BAYONET, 'm' },
				{ WEAPON_KNIFE_KARAMBIT, 'n' },
				{ WEAPON_DECOY, 'o' },
				{ WEAPON_ELITE, 'p' },
				{ WEAPON_AK47, 'q' },
				{ WEAPON_BAYONET, 'r' },
				{ WEAPON_FIVESEVEN, 's' },
				{ WEAPON_BIZON, 't' },
				{ WEAPON_CZ75A, 'u' },
				{ WEAPON_KNIFE_FLIP, 'v' },
				{ WEAPON_AUG, 'w' },
				{ WEAPON_KNIFE_BUTTERFLY, 'x' },
				{ WEAPON_C4, 'y' },
				{ WEAPON_KNIFE, 'z' }
			};
		}
	}
	using DrawFunc = std::function<void(ESP* thisptr)>;
	enum ESPType
	{
		ESP_PLAYER = 0,
		ESP_WEAPON,
		ESP_C4,
		ESP_PROJECTILE,
		ESP_MISC
	};
	struct ESPObject
	{
		C_BaseEntity* ent = NULL;
		Color clr = Color::Black;
		bool setup = false;
		bool draw = false;
		ESPType type = ESP_PLAYER;
		struct bbox_t //Screen coordinates, integers because this is in pixels
		{
			int l = 0;
			int r = 0;
			int t = 0;
			int b = 0;
		} bbox;

		std::initializer_list<DrawFunc>* drawfuncs;
	} ctx;

	_ISurface_* renderer = nullptr;


	void DrawPlayerName(ESP* thisptr)
	{
		wchar_t buf[128];
		player_info_t info = ((C_BasePlayer*)ctx.ent)->GetPlayerInfo();
		if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 128) > 0)
		{
			int w, h;
			renderer->GetTextSize(Fonts::esp, buf, w, h);

			renderer->DrawSetTextFont(Fonts::esp);
			renderer->DrawSetTextColor(Color::White);
			int x = ctx.bbox.l + ((ctx.bbox.r - ctx.bbox.l) / 2);
			renderer->DrawSetTextPos(x - (w / 2), ctx.bbox.t - h);
			renderer->DrawPrintText(buf, wcslen(buf));
		}
	}

	void DrawArmorBar(ESP* thisptr)
	{
		if (!Options::Instance()->esp_armor_bar()) return;
		auto armour = ((C_BasePlayer*)ctx.ent)->m_ArmorValue();
		if (!armour) return; //No point in drawing in armor bar if you don't have any armor?
		float box_h = (float)fabs(ctx.bbox.b - ctx.bbox.t);
		//float off = (box_h / 6.f) + 5;
		float off = 4;

		auto height = (((box_h * armour) / 100));

		int x = ctx.bbox.r + off;
		int y = ctx.bbox.t;
		int w = 4;
		int h = box_h;

		renderer->DrawSetColor(Color::Black);
		renderer->DrawFilledRect(x, y, x + w, y + h);

		renderer->DrawSetColor(Color(0, 50, 255, 255));
		renderer->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 2);
	}

	void ESP::DrawSpreadCircle()
	{
		static int width = 0;
		static int height = 0;
		if (height == 0 || width == 0)
			(*Interfaces::Instance()->GetInterface<IVEngineClient>())->GetScreenSize(width, height);

		auto wep = g_LocalPlayer->m_hActiveWeapon().Get();
		if (g_LocalPlayer->IsAlive() && wep)
		{
			float spreadDist = ((wep->GetInaccuracy() + wep->GetSpread()) * 320.f) / std::tan(DEG2RAD(68.f) * 0.5f);
			float spreadRadius = spreadDist * (height / 480.f);

			renderer->DrawSetColor(Color::Red);
			renderer->DrawOutlinedCircle(width / 2, height / 2, (int)spreadRadius, 24);
		}
	}

	void DrawSkeletonESP(ESP* thisptr)
	{
		return;
		if (!Options::Instance()->esp_skeleton()) return;
		CACHED auto DrawLine = [&](const Vector& a, const Vector& b)
		{
			renderer->DrawLine(a.x, a.y, b.x, b.y);
		};
		C_BasePlayer* ent = (C_BasePlayer*)ctx.ent;
		std::array<Vector, HITBOX_MAX> bones;
		for (int i = 0; i < HITBOX_MAX; i++)
		{
			
			auto idx = ent->EntIndex();
			bool override = thisptr->ShouldOverride(idx);
			Vector pos = (override) ? ent->TranslateHitboxPos(i, thisptr->GetOverridenMatrix(idx)) : ent->GetHitboxPos(i);
			Vector screenpos;
			if (!Math::WorldToScreen(pos, screenpos))
				return;
			bones[i] = screenpos;
		}
		renderer->DrawSetColor(Color::Red);
		renderer->DrawOutlinedCircle(bones[HITBOX_HEAD].x, bones[HITBOX_HEAD].y, 4, 36);
		//renderer->DrawSetColor(Color::White);
		//DrawLine(bones[HITBOX_HEAD], bones[HITBOX_NECK]);
		//DrawLine(bones[HITBOX_NECK], bones[HITBOX_CHEST]);
		//DrawLine(bones[HITBOX_CHEST], bones[HITBOX_LEFT_UPPER_ARM]);
		//DrawLine(bones[HITBOX_CHEST], bones[HITBOX_RIGHT_UPPER_ARM]);
		//DrawLine(bones[HITBOX_LEFT_UPPER_ARM], bones[HITBOX_LEFT_FOREARM]);
		//DrawLine(bones[HITBOX_RIGHT_UPPER_ARM], bones[HITBOX_RIGHT_FOREARM]);
		//DrawLine(bones[HITBOX_LEFT_UPPER_ARM], bones[HITBOX_LEFT_FOREARM]);
		//DrawLine(bones[HITBOX_RIGHT_UPPER_ARM], bones[HITBOX_RIGHT_FOREARM]);
		//DrawLine(bones[HITBOX_LEFT_FOREARM], bones[HITBOX_LEFT_HAND]);
		//DrawLine(bones[HITBOX_RIGHT_FOREARM], bones[HITBOX_RIGHT_HAND]);
		//DrawLine(bones[HITBOX_CHEST], bones[HITBOX_PELVIS]);
		//DrawLine(bones[HITBOX_PELVIS], bones[HITBOX_LEFT_CALF]);
		//DrawLine(bones[HITBOX_PELVIS], bones[HITBOX_RIGHT_CALF]);
		//DrawLine(bones[HITBOX_LEFT_CALF], bones[HITBOX_LEFT_FOOT]);
		//DrawLine(bones[HITBOX_RIGHT_CALF], bones[HITBOX_RIGHT_FOOT]);
	}

	void DrawHealthBar(ESP* thisptr)
	{
		if (!Options::Instance()->esp_hp_bar()) return;
		C_BasePlayer* player = (C_BasePlayer*)(ctx.ent);
		auto health = player->m_iHealth();
		int numsegs = health / 10;
		Color hpcolor = Color::Green;
		if (numsegs < 7) hpcolor = Color(255, 255, 0, 255);
		if (numsegs < 4) hpcolor = Color::Red;

		int x = ctx.bbox.l - 10;
		int y = ctx.bbox.t;
		int w = 5;
		int h = std::labs(ctx.bbox.b - ctx.bbox.t);
		auto height = (((h * health) / 100));
		renderer->DrawSetColor(Color::Black);
		renderer->DrawOutlinedRect(x, y, x + w, y + h);
		renderer->DrawSetColor(hpcolor);
		renderer->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 2);
		/*
		int seg_h = h / 10;
		if (seg_h > 10)
		{
			for (int i = 0; i < 10; i++)
			{
				int _x0 = x;
				int _x1 = x + w;
				int _y0 = y + (i * seg_h);
				int _y1 = y + ((i + 1) * seg_h);
				renderer->DrawSetColor(Color::Black);
				renderer->DrawOutlinedRect(_x0, _y0, _x1, _y1);
				renderer->DrawSetColor(i < numsegs ? hpcolor : Color::Black);
				renderer->DrawFilledRect(_x0 + 1, _y0 + 1, _x1 - 1, _y1 - 1);
			}
		}
		else
		{
			renderer->DrawSetColor(Color::Black);
			renderer->DrawOutlinedRect(x, y, x + w, y + h);
			renderer->DrawSetColor(hpcolor);
			int yo = (numsegs == 10) ? 1 : 0;
			renderer->DrawFilledRect(x + 1, y + 1, (x + w) - 1, (y + (numsegs * seg_h)) - yo);
		}
		/*
		wchar_t buf[128];
		swprintf(buf, L"%c", Fonts::Icons::ICON_HEALTH, health);
		int s_w, s_h;
		renderer->GetTextSize(Fonts::icons, buf, s_w, s_h);
		renderer->DrawSetTextFont(Fonts::status_icons);
		renderer->DrawSetTextColor(hpcolor);
		renderer->DrawSetTextPos(x - w - (s_w / 2), (y + (h / 2)) + (s_h / 2));
		renderer->DrawPrintText(buf, wcslen(buf));
		*/

	}

	void DrawPlayerStatus(ESP* thisptr)
	{
		auto player = (C_BasePlayer*)ctx.ent;
		QAngle view;
		(*Interfaces::Instance()->GetInterface<IVEngineClient>())->GetViewAngles(view);

		auto halfway = (player->GetCollideable()->OBBMins() + player->GetCollideable()->OBBMaxs()) / 2;
		auto dst = player->m_vecOrigin() + halfway;
		float dmg;
		auto start = g_LocalPlayer->GetEyePos();
		auto distance = (dst - start).Length();
		auto fov = fabsf(Math::GetFOV(start, dst, view));
		int x = ctx.bbox.r + 10;
		int y = ctx.bbox.t + 5;
		//renderer->DrawSetTextPos(x, y);
		//renderer->DrawSetTextFont(Fonts::esp);
		//renderer->DrawSetColor(Color::White);
		//wchar_t buf[256];
		//swprintf(buf, L"D: %.2f F: %.2f %d %s", distance, fov, ctx.ent->GetClientClass()->m_ClassID, ctx.ent->GetClientClass()->m_pNetworkName);
		//renderer->DrawPrintText(buf, wcslen(buf));

	}

	void DrawWeaponName(ESP* thisptr)
	{
		CACHED auto CleanItemName = [&](const char* name) -> const char*
		{
			if (name[0] == 'C')
				name++;

			auto start = strstr(name, "Weapon");
			if (start != nullptr)
				name = start + 6;

			return name;
		};
		if (!Options::Instance()->esp_weapon_name()) return;
		auto name = CleanItemName(ctx.ent->GetClientClass()->m_pNetworkName);
		wchar_t buf[80];
		if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0) {
			int w = ctx.bbox.r - ctx.bbox.l;
			int tw, th;
			renderer->GetTextSize(Fonts::esp, buf, tw, th);

			renderer->DrawSetTextFont(Fonts::esp);
			renderer->DrawSetTextColor(Color::White);
			renderer->DrawSetTextPos((ctx.bbox.l + w * 0.5f) - tw * 0.5f, ctx.bbox.b + 1);
			renderer->DrawPrintText(buf, wcslen(buf));
		}
		/*
		auto wepID = ((C_BaseCombatWeapon*)ctx.ent)->m_Item().m_iItemDefinitionIndex();
		auto entry = Fonts::Icons::weapon_icons.find(wepID);
		if (entry == Fonts::Icons::weapon_icons.end()) return;
		auto icon = entry->first;
		wchar_t buf[128];
		int i_w, i_h;
		int w = ctx.bbox.r - ctx.bbox.l;
		swprintf(buf, L"%c", icon);
		renderer->GetTextSize(Fonts::icons[10], buf, i_w, i_h);
		renderer->DrawSetTextFont(Fonts::icons[10]);
		renderer->DrawSetTextColor(Color::White);
		renderer->DrawSetTextPos((ctx.bbox.l + w * 0.5f) - i_w * 0.5f, ctx.bbox.b + 1);
		renderer->DrawPrintText(buf, wcslen(buf));
		*/
	}

	void DrawBoundingBox(ESP* thisptr)
	{
		if (ctx.ent->IsWeapon() && !Options::Instance()->esp_weapon_bbox()) return;
		if (ctx.ent->IsPlayer() && !Options::Instance()->esp_bbox()) return;
		renderer->DrawSetColor(ctx.clr);
		renderer->DrawOutlinedRect(ctx.bbox.l, ctx.bbox.t, ctx.bbox.r, ctx.bbox.b);
		renderer->DrawSetColor(Color::Black);
		renderer->DrawOutlinedRect(ctx.bbox.l - 1, ctx.bbox.t - 1, ctx.bbox.r + 1, ctx.bbox.b + 1);
		renderer->DrawOutlinedRect(ctx.bbox.l + 1, ctx.bbox.t + 1, ctx.bbox.r - 1, ctx.bbox.b - 1);
	}

	ESP::ESP()
	{
		renderer = Interfaces::Instance()->GetInterface<ISurface>()->get();
		Fonts::esp = renderer->CreateFont_();
		Fonts::weapons = renderer->CreateFont_();
		Fonts::defuse = renderer->CreateFont_();
		Fonts::watermark = renderer->CreateFont_();
		Fonts::watermark_smaller = renderer->CreateFont_();

		auto test = AddFontResourceEx(
			"F:\\icons.ttf", // font file name
			FR_PRIVATE,             // font characteristics
			NULL             // reserved
		);

		m_bBoneMatrixOverriden.fill(false);
		renderer->SetFontGlyphSet(Fonts::esp, "Arial", 12, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
		renderer->SetFontGlyphSet(Fonts::defuse, "Arial", 15, 700, 0, 0, FONTFLAG_DROPSHADOW);
		renderer->SetFontGlyphSet(Fonts::weapons, "Arial", 10, 700, 0, 0, FONTFLAG_DROPSHADOW);
		renderer->SetFontGlyphSet(Fonts::watermark, "Arial", 25, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
		renderer->SetFontGlyphSet(Fonts::watermark_smaller, "Arial", 13, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
		/*
		for (int i = 1; i <= 15; i++)
		{
			Fonts::status_icons[i - 1] = renderer->CreateFont_();
			Fonts::icons[i - 1] = renderer->CreateFont_();
			renderer->SetFontGlyphSet(Fonts::status_icons[i - 1], "Counter-Strike", i, 300, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE);
			renderer->SetFontGlyphSet(Fonts::icons[i - 1], "icomoon", 14, 500, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW | FONTFLAG_OUTLINE);
		}
		*/
	}

	CACHED std::initializer_list<DrawFunc> alive = { DrawBoundingBox, DrawPlayerStatus, DrawPlayerName, DrawHealthBar, DrawArmorBar, DrawSkeletonESP };
	CACHED std::initializer_list<DrawFunc> dead = {};
	CACHED std::initializer_list<DrawFunc> weapon = { DrawBoundingBox, DrawWeaponName };
	CACHED std::initializer_list<DrawFunc> c4 = { DrawBoundingBox };
	void ESP::Setup(C_BaseEntity* ent)
	{

		CACHED auto bounds = [&](ESPObject& object) mutable -> bool
		{
			if (!object.ent) return false;
			auto collideable = object.ent->GetCollideable();

			if (!collideable) return false; //Entity does not have a collision component??

			auto min = collideable->OBBMins();
			auto max = collideable->OBBMaxs();

			const matrix3x4_t& frame = ent->m_rgflCoordinateFrame();

			std::array<Vector, 8> points = {
				Vector(min.x, min.y, min.z),
				Vector(min.x, max.y, min.z),
				Vector(max.x, max.y, min.z),
				Vector(max.x, min.y, min.z),
				Vector(max.x, max.y, max.z),
				Vector(min.x, max.y, max.z),
				Vector(min.x, min.y, max.z),
				Vector(max.x, min.y, max.z)
			};

			std::array<Vector, 8> screen;
			Vector pointsTransformed[8];
			for (int i = 0; i < 8; i++) {
				Math::VectorTransform(points[i], frame, pointsTransformed[i]);
			}

			Vector screen_points[8] = {};

			for (int i = 0; i < 8; i++) {
				if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i])){}
					//return false;
			}

			auto left = screen_points[0].x;
			auto top = screen_points[0].y;
			auto right = screen_points[0].x;
			auto bottom = screen_points[0].y;

			for (int i = 1; i < 8; i++) {
				if (left > screen_points[i].x)
					left = screen_points[i].x;
				if (top > screen_points[i].y)
					top = screen_points[i].y;
				if (right < screen_points[i].x)
					right = screen_points[i].x;
				if (bottom < screen_points[i].y)
					bottom = screen_points[i].y;
			}
			object.bbox.l = std::lroundf(left);
			object.bbox.r = std::lroundf(right);
			object.bbox.t = std::lroundf(top);
			object.bbox.b = std::lroundf(bottom);

			return true;
		};
		ctx.ent = ent;
		renderer = Interfaces::Instance()->GetInterface<ISurface>()->get();
		ctx.drawfuncs = nullptr;
		auto classID = ent->GetClientClass()->m_ClassID;
		bool isPlayer = (classID == 40);///SourceEngine::ClassId::ClassId_CCSPlayer);
		if (isPlayer && ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{
			C_BasePlayer* player = reinterpret_cast<C_BasePlayer*>(ent);
			ctx.draw = !player->IsDormant();
			ctx.setup = bounds(ctx);
			ctx.type = ESP_PLAYER;
			ctx.clr = (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) ? Color::Green : Color::Red;
			std::initializer_list<DrawFunc>* list = (player->IsAlive()) ? &alive : &dead;
			ctx.drawfuncs = list;
		}
		else if (Options::Instance()->esp_dropped_weapons() && ent->IsWeapon() && !ent->m_hOwnerEntity().IsValid())
		{
			ctx.draw = true;
			ctx.setup = bounds(ctx);
			ctx.type = ESP_WEAPON;
			ctx.clr = Color::Blue;
			ctx.drawfuncs = &weapon;
		}
		else if (Options::Instance()->esp_dropped_c4() && ent->IsPlantedC4())
		{
			ctx.draw = true;
			ctx.setup = bounds(ctx);
			ctx.type = ESP_C4;
			ctx.clr = Color::White;
			ctx.drawfuncs = &c4;
		}
		else
		{
			ctx.setup = false;
			ctx.draw = false;
		}
	}
	CACHED wchar_t name[1024];
	CACHED wchar_t date[1024];
	CACHED wchar_t version[1024];
	CACHED bool once = false;
	CACHED Color s_watermarkColor = Color(77, 197, 76);
	void ESP::Watermark()
	{
		if (!once)
		{
			CACHED auto convert = [&](const char* c) -> const wchar_t*
			{
				const size_t cSize = strlen(c) + 1;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs(wc, c, cSize);

				return wc;
			};
			swprintf(name, L"Envycheat.cc - %s", convert(g_Version.szAuthor));
			swprintf(date, L"Built: %s@%s EST", convert(g_Version.szBuildDate), convert(g_Version.szBuildTime));
			swprintf(version, L"Version: %s", convert(g_Version.szVersionNumber));
			once = true;
		}
#if 1
		renderer->DrawSetTextFont(Fonts::watermark);
		renderer->DrawSetTextColor(s_watermarkColor);
		int x = 50, y = 50;
		int t_w = 0, t_h = 0;
		renderer->DrawSetTextPos(x, y);
		renderer->DrawPrintText(name, wcslen(name));
		renderer->GetTextSize(Fonts::watermark, name, t_w, t_h);
		y += t_h;
		renderer->DrawSetTextPos(x, y);
		renderer->DrawSetTextFont(Fonts::watermark_smaller);
		renderer->DrawPrintText(date, wcslen(date));
		renderer->GetTextSize(Fonts::watermark_smaller, date, t_w, t_h);
		y += t_h;
		renderer->DrawSetTextPos(x, y);
		renderer->DrawPrintText(version, wcslen(version));
#else
		const wchar_t* huh = L"Envycheat.cc";
		renderer->DrawSetTextFont(Fonts::esp);
		renderer->DrawSetTextColor(Color(77, 197, 76));
		int x = 50, y = 300;
		renderer->DrawSetTextPos(x, y);
		renderer->DrawPrintText(huh, wcslen(huh));
#endif
	}
	void ESP::Exec()
	{
		if (!ctx.ent || !ctx.setup) return;
		if (!ctx.draw) return;
		if (!renderer) return;
		for (auto func : *ctx.drawfuncs)
		{
			func(this);
		}
	}
	void ESP::DrawAngleLines(float real, float fake, float lby)
	{
		Vector origin = g_LocalPlayer->m_vecOrigin();
		Vector forward[3];
		Math::AngleVectors(QAngle(0.f, real, 0.f), forward[0]);
		Math::AngleVectors(QAngle(0.f, fake, 0.f), forward[1]);
		Math::AngleVectors(QAngle(0.f, lby, 0.f), forward[2]);
		Color colors[3] = { Color::Green, Color::Red, Color(128, 128, 0) };

		Vector start[3];
		Vector end[3];

		for (int i = 0; i < 3; i++)
		{
			Vector dst = origin + forward[i] * 32;
			if (Math::WorldToScreen(origin, start[i]) && Math::WorldToScreen(dst, end[i]))
			{
				renderer->DrawSetColor(colors[i]);
				renderer->DrawLine(start[i].x, start[i].y, end[i].x, end[i].y);
			}
		}
	}
	void ESP::OverrideBoneMatrix(SourceEngine::C_BasePlayer * player, SourceEngine::matrix3x4_t* m)
	{
		int idx = player->EntIndex();
		m_bBoneMatrixOverriden[idx] = true;
		memcpy(m_matBoneMatrices[idx].data(), m, MAXSTUDIOBONES * sizeof(matrix3x4_t));
		//m_matBoneMatrices[idx] = *m;
	}
}
