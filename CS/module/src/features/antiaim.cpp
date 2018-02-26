#include "antiaim.h"
#include "structs.h"
#include "animsys.h"
#include "visualsys.h"
#include "envydefs.hpp"
#include "mathfuncs.h"

#if 1
using namespace Envy::SourceEngine;

AngleModifierFn g_sActivePitch;
AngleModifierFn g_sActiveYaw;
AngleModifierFn g_sActiveFakeYaw;

namespace Envy
{
	struct EnvyCMD
	{
		int tickbase;
		int seq;
	};


	std::vector<EnvyCMD> cmds;
	constexpr int choke = 1;
	CACHED int counter = 0;
	CACHED void* g_AnimState = (void*)0x096ABC00;
	static float lastlocalyaw = 0.f;
	static float lastdelta = 0.f;
	void AntiAim::ProcessAnimBatch()
	{
#if 0
		auto input = Interfaces::Instance()->GetInterface<CInput>();


		int seq = cmds[0].seq;
		auto cmd = input->GetUserCmd(seq);
		auto verified = input->GetVerifiedCmd(seq);
		//*(float*)((uintptr_t)g_AnimState + 0xFC);
		float myturntime = animsys->GetNextTurnTime();

		uintptr_t v3 = (uintptr_t)g_AnimState;
		float v73 = *(float*)(v3 + 0x70);
		float v166 = Math::AngleDiff(v73, *(float*)(v3 + 0x68));
		float lastprocessedyaw = *(float*)((uintptr_t)g_AnimState + 0x68);
		std::cout << "turntime: " << nextturntime << " my time: " << myturntime;
		std::cout << " last processed yaw: " << lastprocessedyaw << " last local processed yaw: " << lastlocalyaw;
		std::cout << " last processed delta: " << v166 << " last local delta: " << lastdelta << std::endl;

		int stb = (*(float*)((uintptr_t)g_AnimState + 0xFC) - 1.1f) / globals->interval_per_tick;
		int ptb = (*(float*)((uintptr_t)g_AnimState + 0xFC)) / globals->interval_per_tick;

		g_Subsystems->Get<VisualSubsystem>()->SetNextTurnTime(nextturntime);
		g_Subsystems->Get<VisualSubsystem>()->SetAnimTime(*(float*)((uintptr_t)g_AnimState + 0xFC));



		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();

		lastlocalyaw = cmd->viewangles.yaw;
		lastdelta = animsys->GetLBYDelta();

		/*
		for (auto& ecmd : cmds)
		{
			int seq = ecmd.seq;
			auto cmd = input->GetUserCmd(seq);
			
				Do anim check
			
			float curtime = ecmd.tickbase * globals->interval_per_tick;
			animsys->SimulateServerAnimationCode(cmd->viewangles, curtime);
		}
		g_Subsystems->Get<VisualSubsystem>()->SetNextTurnTime(nextturntime);
		g_Subsystems->Get<VisualSubsystem>()->SetPredictedTickbase(processedtb);
		g_Subsystems->Get<VisualSubsystem>()->SetAnimInformation(
			*(float*)((uintptr_t)g_AnimState + 0x68),
			*(float*)((uintptr_t)g_AnimState + 0x70),
			*(float*)((uintptr_t)g_AnimState + 0x74)
		);
		*/
#endif
	}
	void AntiAim::EncapsulateCMD(int seq)
	{
		EnvyCMD ecmd;
		ecmd.tickbase = g_LocalPlayer->m_nTickBase();
		ecmd.seq = seq;
		cmds.push_back(ecmd);
	}

	float Normalize(float yaw)
	{
		float target = yaw;
		if (target < -180.f) target += 360.f;
		if (target > 180.f) target -= 360.f;

		return target;
	}
	float AntiAim::EnsureLBYDesync()
	{
		float srcyaw = Options::Instance()->aa_real_offset();
		float dstyaw = Options::Instance()->aa_fake_offset();

		if (srcyaw < 0.f) srcyaw += 360.f;
		if (dstyaw < 0.f) dstyaw += 360.f;

		float flSide = (srcyaw < dstyaw) ? -1.f : 1.f;

		constexpr float flMinimumDelta = 110.f;
		float flAngleDelta = fabsf(dstyaw - srcyaw);
		float flFinalDelta = 0.f;

		if (flAngleDelta < flMinimumDelta)
		{
			flFinalDelta = flSide * (flMinimumDelta - flAngleDelta);
		}

		return m_angRealAngle.yaw + flFinalDelta;
	}

	QAngle CombineAAAngles(CUserCmd* cmd, bool fake)
	{

		auto yawfn = (fake) ? g_sActiveFakeYaw : g_sActiveYaw;
		auto pitchfn = g_sActivePitch;
		QAngle yaw = ((uintptr_t)yawfn != INVALID) ? yawfn(cmd) : cmd->viewangles;
		QAngle pitch = ((uintptr_t)pitchfn != INVALID) ? pitchfn(cmd) : cmd->viewangles;

		return QAngle(pitch.pitch, yaw.yaw, cmd->viewangles.roll);
	}
	void AntiAim::ProcessWithLBY(SourceEngine::CUserCmd * cmd, bool & sendpacket, int seq)
	{
		auto globals = (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>());
		int processedtb = g_LocalPlayer->m_nTickBase();
		float nextprocessedcurtime = (processedtb + 2) * globals->interval_per_tick;
		float processedcurtime = processedtb * globals->interval_per_tick;
		auto animsys = g_Subsystems->Get<AnimationSubsystem>();

		if (counter < choke)
		{
			cmd->viewangles = m_angRealAngle;
			counter++;
			sendpacket = false;
			float nextturntime = animsys->GetNextTurnTime();
			if (processedcurtime > nextturntime && nextturntime != m_flLastTurnTime)
			{
				cmd->viewangles = m_angFakeAngle;
				m_flLastTurnTime = nextturntime;
				m_bJustUpdated = true;
			}
			else if (nextprocessedcurtime > nextturntime && nextturntime != m_flLastTurnTime)
			{
				cmd->viewangles = QAngle(m_angRealAngle.pitch, EnsureLBYDesync(), 0.f);
			}
			float delta = g_LocalPlayer->m_flLowerBodyYawTarget() + (fmaxf(fabsf(Math::AngleDiff(Options::Instance()->aa_fake_offset(), Options::Instance()->aa_real_offset())), 120.f));
			g_Subsystems->Get<AnimationSubsystem>()->SimulateServerAnimationCode(
				QAngle(0.f, g_LocalPlayer->m_flLowerBodyYawTarget() + 180.f, 0.f),
				processedcurtime
			);
		}
		else
		{
			cmd->viewangles = m_angFakeAngle;
			counter = 0;
			sendpacket = true;
		}
	}
	void AntiAim::ProcessAntiAim(CUserCmd* cmd, bool& sendpacket, int sequence)
	{
		auto globals = (*Interfaces::Instance()->GetInterface<CGlobalVarsBase>());
		int processedtb = g_LocalPlayer->m_nTickBase();
		float nextprocessedcurtime = (processedtb + 2) * globals->interval_per_tick;
		float processedcurtime = processedtb * globals->interval_per_tick;
		auto animsys = g_Subsystems->Get<AnimationSubsystem>();
		m_angFakeAngle = CombineAAAngles(cmd, true);
		m_angRealAngle = CombineAAAngles(cmd, false);

		if (Options::Instance()->aa_prevent_lby())
		{
			ProcessWithLBY(cmd, sendpacket, sequence);
		}
		else
		{
			if (counter < choke)
			{
				cmd->viewangles = m_angRealAngle;
				counter++;
				sendpacket = false;
			}
			else
			{
				cmd->viewangles = m_angFakeAngle;
				counter = 0;
				sendpacket = true;
			}
		}

		auto visualsys = g_Subsystems->Get<VisualSubsystem>();
		visualsys->SetRealYaw(m_angRealAngle.yaw);
		visualsys->SetFakeYaw(m_angFakeAngle.yaw);
	}
}
#endif
