#include "..\..\include\subsystem\viewanglesys.h"

#include "inputsys.h"
#include "animsys.h"

#include <cinput.hpp>
#include <mathfuncs.h>
#include <visualsys.h>

using namespace Envy::SourceEngine;

namespace Envy
{

	void ViewangleSubssytem::Init()
	{
		m_Aimbot = std::make_unique<Aimbot>();
		m_AntiAim = std::make_unique<AntiAim>();
		m_Triggerbot = std::make_unique<Triggerbot>();
	}

	struct BoneAccessor
	{
		const void* m_pAnimating;

		matrix3x4_t* m_pBones;

		int m_ReadableBones;		// Which bones can be read.
		int m_WritableBones;		// Which bones can be written.
	};

	struct EnvyCMD
	{
		bool m_bProcessAnim = false;
		bool m_bLowerBodyYawUpdate = false;
		CUserCmd* m_pUserCmd;
	};

	float ApproachLBYSafely(float src, float dst, float dt)
	{
		if (src < 0.f) src += 360.f;
		if (dst < 0.f) dst += 360.f;
		float angle = src + 90.f;
		return angle;
	}

	CACHED std::vector<EnvyCMD> s_EnvyCmds;
	CACHED float curtime = 0.f;
	CACHED float g_flLastTurnTime = 0.f;
	CACHED bool g_bJustUpdated = false;
	void ViewangleSubssytem::OnCreateMove(
		CUserCmd* usercmd,
		bool& sendpacket,
		int sequence
	)
	{
		auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>()->get();
		auto globals = Interfaces::Instance()->GetInterface<CGlobalVarsBase>()->get();
		auto prediction = Interfaces::Instance()->GetInterface<IPrediction>();
		if (!engine->IsInGame()) return;

		m_safeAngle = usercmd->viewangles;
		prediction->RunCommand_Rebuilt(usercmd);

		//Disable this shit for now

		auto animsys = g_Subsystems->Get<AnimationSubsystem>();
#if 0
		float fake = usercmd->viewangles.yaw - 90.f;
		float real = usercmd->viewangles.yaw + 90.f;
		float moving = usercmd->viewangles.yaw + 180.f;


		bool ismoving = g_LocalPlayer->m_vecVelocity().Length() > 0.1;
		if (counter < 13)
		{
			sendpacket = false;
			QAngle realangle = usercmd->viewangles;
			if (g_Subsystems->Get<InputSubsystem>()->GetKeyState(18) == KeyState::Down)
			{
				usercmd->viewangles.yaw = (ismoving) ? moving : real;
				realangle.yaw = (ismoving) ? moving : real;
				if (counter == 0)
				{
					if (animsys->IsSet() && g_LocalPlayer->GetPredictionTime() > animsys->GetNextTurnTime())
					{
						usercmd->viewangles.yaw = fake;
						animsys->IsSet() = false;
					}
				}
			}
			if (counter == 0)
			{
				animsys->SimulateServerAnimationCode(realangle, g_LocalPlayer->GetPredictionTime());
			}
			counter++;
		}
		else
		{
			sendpacket = true;
			if (g_Subsystems->Get<InputSubsystem>()->GetKeyState(18) == KeyState::Down)
			{
				usercmd->viewangles.yaw = fake;
			}
			counter = 0;
		}
#endif
		//float yaw = usercmd->viewangles.yaw;
		//float real = yaw + Options::Instance()->aa_real_offset();
		//float fake = yaw + Options::Instance()->aa_fake_offset();
		//float lby = yaw + Options::Instance()->aa_lby_offset();

		//if (lby < -180.f) lby += 360.f;
		//if (lby > 180.f) lby -= 360.f;
		//if (real < -180.f) real += 360.f;
		//if (real > 180.f) real -= 360.f;
		//if (fake < -180.f) fake += 360.f;
		//if (fake > 180.f) fake -= 360.f;


		auto visualsys = g_Subsystems->Get<VisualSubsystem>();
#if 0 
		if (g_Subsystems->Get<InputSubsystem>()->GetKeyState(18) == KeyState::Down)
		{
			if (counter < 1)
			{
				sendpacket = false;
				m_angRealAngle = usercmd->viewangles;
				usercmd->viewangles.yaw = real;
				m_angRenderAngle = usercmd->viewangles;
				if (g_bJustUpdated)
				{
					g_bJustUpdated = false;
					usercmd->viewangles.yaw = ApproachLBYSafely(lby, real, (2 * globals->interval_per_tick));
				}
				if (counter == 0)
				{
					float nextturntime = animsys->GetNextTurnTime();//*(float*)((uintptr_t)g_AnimState + 0xFC);
					visualsys->SetNextTurnTime(nextturntime);
					//visualsys->SetLastTurnTime(g_flLastTurnTime);
					if (g_LocalPlayer->GetPredictionTime() > nextturntime && nextturntime != g_flLastTurnTime)
					{
						g_flLastTurnTime = nextturntime;
						usercmd->viewangles.yaw = lby;
						animsys->IsSet() = false;
						g_bJustUpdated = true;
						m_angRenderAngle = usercmd->viewangles;
					}
					else if (((g_LocalPlayer->m_nTickBase() + 2) * globals->interval_per_tick) > nextturntime && nextturntime != g_flLastTurnTime)
					{
						usercmd->viewangles.yaw = usercmd->viewangles.yaw = ApproachLBYSafely(real, lby, (2 * globals->interval_per_tick));
					}
					//visualsys->SetLBYDelta(fabsf(Math::AngleDiff(usercmd->viewangles.yaw, g_LocalPlayer->m_flLowerBodyYawTarget())));
					curtime = g_LocalPlayer->GetPredictionTime();
					//visualsys->SetCurrentTime(curtime);
					m_angRealAngle.yaw = g_LocalPlayer->m_flLowerBodyYawTarget() + 180.f;
					animsys->SimulateServerAnimationCode(m_angRealAngle, g_LocalPlayer->GetPredictionTime());
				}
				counter++;
			}
			else
			{
				counter = 0;
				sendpacket = true;
				usercmd->viewangles.yaw = fake;
			}
			usercmd->viewangles.pitch = 89.f;
			m_angRenderAngle.pitch = usercmd->viewangles.pitch;
		}
		else
		{
			m_angRealAngle = usercmd->viewangles;
			m_angRenderAngle = usercmd->viewangles;
			curtime = g_LocalPlayer->GetPredictionTime();
		}
#endif
		if (true)
		{
			if (m_Triggerbot->ProcessTriggerBot(m_safeAngle, sendpacket))
			{
				usercmd->buttons |= IN_ATTACK;
			}
		}
		if (false/*Options::Instance()->aa_enabled()*/)
		{
			m_AntiAim->ProcessAntiAim(usercmd, sendpacket, sequence);
			m_angRenderAngle = m_AntiAim->GetRealAngle();

		}
		if (false/*Options::Instance()->aimbot_enable()*/)
		{
			m_Aimbot->ProcessAimbot(m_safeAngle, sendpacket);
			if (m_Aimbot->Valid())
			{
				if (Options::Instance()->aimbot_autofire())
				{
					usercmd->buttons |= IN_ATTACK;
				}
				if (Options::Instance()->aimbot_autoaim())
				{
					usercmd->viewangles = m_Aimbot->GetAimAngle();
					m_angRenderAngle = m_Aimbot->GetAimAngle();
				}
				else if (usercmd->buttons & IN_ATTACK)
				{
					usercmd->viewangles = m_Aimbot->GetAimAngle();
					m_angRenderAngle = m_Aimbot->GetAimAngle();
				}

			}
		}
		Interfaces::Instance()->GetInterface<CGameMovement>()->FixMovement(usercmd, m_safeAngle);

		if (!Options::Instance()->aimbot_silent_aim())
		{
			Interfaces::Instance()->GetInterface<IVEngineClient>()->get()->SetViewAngles(usercmd->viewangles);
		}
	}
	void ViewangleSubssytem::OnFrameStageNotify(
		SourceEngine::ClientFrameStage_t stage
	)
	{
		if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			//Player cache is stale
		}
		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		{
			auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>()->get();
			auto input = Interfaces::Instance()->GetInterface<CInput>();
			if (engine->IsInGame() && input->ThirdPerson())
			{
				g_LocalPlayer->m_angThirdpersonAngles() = m_angRenderAngle;
			}
		}
	}
	CACHED int s_lastOutgoing = -1;
	void ViewangleSubssytem::OnPredictionUpdate(
		int startframe,
		bool validframe,
		int incomingack,
		int outgoingcmd
	)
	{

	}
}
