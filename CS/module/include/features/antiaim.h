#pragma once

#include "csgosdk.h"
#include "function.hpp"

#include <map>

using AngleModifierFn = Envy::SourceEngine::QAngle(*)(Envy::SourceEngine::CUserCmd*);
extern std::map<const char*, std::map<const char*, AngleModifierFn>*> g_sAntiAims;

extern AngleModifierFn g_sActivePitch;
extern AngleModifierFn g_sActiveYaw;
extern AngleModifierFn g_sActiveFakeYaw;

namespace Envy
{
	namespace AntiAims
	{

		class AntiAimList
		{
		public:
			AntiAimList(const char* family) : m_szFamily(family)
			{
				//auto list = g_sAntiAims[family];
				//auto itr = list->begin();

				//for (itr; itr != list->end(); itr++)
				//{
				//	m_list.push_back(itr->first);
				//}
			}

			AngleModifierFn operator[](int idx) { return GetAntiAim(idx); }

			AngleModifierFn GetAntiAim(const char* name)
			{
				auto list = g_sAntiAims[m_szFamily];
				return (*list)[name];
			}
			AngleModifierFn GetAntiAim(int idx)
			{
				auto list = g_sAntiAims[m_szFamily];
				if (idx >= m_list.size())
				{
					return (AngleModifierFn)INVALID;
				}
				return (*list)[m_list[idx]];
			}
			const char* Compile() 
			{ 
				m_compiledList = "";
				for (auto& aa : m_list)
				{
					m_compiledList += aa;
					m_compiledList += '\0';
				}

				return m_compiledList.c_str();
			}

		private:
			const char* m_szFamily;
			std::vector<const char*> m_list;
			std::string m_compiledList;
		};
		void InitializeAntiAimList();

	}
	class AntiAim
	{
	public:
		AntiAim() { AntiAims::InitializeAntiAimList(); }
		void ProcessAntiAim(SourceEngine::CUserCmd* cmd, bool& sendpacket, int seq);
		SourceEngine::QAngle GetRealAngle() const { return m_angRealAngle; }
		SourceEngine::QAngle GetFakeAngle() const { return m_angFakeAngle; }

	private:
		void ProcessAnimBatch();
		void EncapsulateCMD(int seq);

		void ProcessWithLBY(SourceEngine::CUserCmd* cmd, bool& sendpacket, int seq);

		float EnsureLBYDesync();
	private:
		SourceEngine::QAngle m_angRealAngle;
		SourceEngine::QAngle m_angFakeAngle;

		float m_flLastTurnTime = 0.f;
		bool m_bJustUpdated = false;

	};
}