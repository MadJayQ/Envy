#include "antiaim.h"

#define EXPOSE_ANTIAIM(family, name) g_sAntiAims[#family]->emplace(std::pair<const char*, AngleModifierFn>(#name, AngleModifierFn((uintptr_t)family##::##name)));
#define PITCH_ADD(pitch) QAngle(pitch, 0.f, 0.f);
#define YAW_ADD(yaw) QAngle(0.f, yaw, 0.f);

std::map<const char*, AngleModifierFn> g_InternalPitchMap;
std::map<const char*, AngleModifierFn> g_InternalYawMap;
std::map<const char*, std::map<const char*, AngleModifierFn>*> g_sAntiAims;

using namespace Envy::SourceEngine;
namespace Envy
{

	namespace AntiAims
	{
		namespace Yaw
		{
			QAngle Backwards(CUserCmd* cmd)
			{
				return cmd->viewangles + YAW_ADD(180.f);
			}
			
			QAngle Left(CUserCmd* cmd)
			{
				return cmd->viewangles + YAW_ADD(90.f);
			}

			QAngle Right(CUserCmd* cmd)
			{
				return cmd->viewangles + YAW_ADD(-90.f);
			}
		}

		namespace Pitch
		{
			QAngle Down(CUserCmd* cmd)
			{
				return cmd->viewangles + PITCH_ADD(-cmd->viewangles.pitch);
			}
		}

		void InitializeAntiAimList()
		{
			g_sAntiAims.emplace(std::make_pair("Pitch", &g_InternalPitchMap));
			g_sAntiAims.emplace(std::make_pair("Yaw", &g_InternalYawMap));
			EXPOSE_ANTIAIM(Pitch, Down);
			EXPOSE_ANTIAIM(Yaw, Backwards);
			EXPOSE_ANTIAIM(Yaw, Left);
			EXPOSE_ANTIAIM(Yaw, Right);
		}
	}
}