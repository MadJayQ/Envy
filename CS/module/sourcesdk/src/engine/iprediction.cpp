#include "iprediction.hpp"
#include "csgosdk.h"
#include "structs.h"
#include "mathfuncs.h"

using namespace Envy::SourceEngine;
namespace Envy
{
	static char* cMoveData = nullptr;
	typedef void(__thiscall* oRunCommand)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);
	void IPrediction::RunCommand_Rebuilt(SourceEngine::CUserCmd* cmd)
	{
		auto movehelper = Interfaces::Instance()->GetInterface<IMoveHelper>()->get();
		auto globals = Interfaces::Instance()->GetInterface<CGlobalVarsBase>()->get();
		auto movement = Interfaces::Instance()->GetInterface<CGameMovement>()->get();

		if (!movehelper) return;
		if (!globals) return;
		if (!movement) return;

		if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) return;

		float curtime = globals->curtime;
		float frametime = globals->frametime;

		C_BasePlayer* local = g_LocalPlayer;
		CMoveData data;
		auto flags = local->m_fFlags();
		auto buttons = cmd->buttons;

		local->m_pCurrentCommand() = cmd;

		if (!cMoveData)
			cMoveData = (char*)(calloc(1, sizeof(CMoveData)));

		movehelper->SetHost(local);
		*m_pRandomSeed= MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
		*m_pPredictionPlayer = intptr_t(local);

		globals->curtime = local->m_nTickBase() * globals->interval_per_tick;
		globals->frametime = globals->interval_per_tick;

		//Check this in IDA
		cmd->buttons |= *reinterpret_cast<uint8_t*>(uintptr_t(local) + 0x3301); 

		if (cmd->impulse)
			*reinterpret_cast<uint8_t*>(uintptr_t(local) + 0x31EC) = cmd->impulse;

		data.m_nButtons = cmd->buttons;
		int buttonsChanged = cmd->buttons ^ *reinterpret_cast<int*>(uintptr_t(local) + 0x31E8);
		*reinterpret_cast<int*>(uintptr_t(local) + 0x31DC) = (uintptr_t(local) + 0x31E8);
		*reinterpret_cast<int*>(uintptr_t(local) + 0x31E8) = cmd->buttons;
		*reinterpret_cast<int*>(uintptr_t(local) + 0x31E0) = cmd->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
		*reinterpret_cast<int*>(uintptr_t(local) + 0x31E4) = buttonsChanged & ~cmd->buttons; //m_afButtonReleased ~ The ones not down are "released"

		movement->StartTrackPredictionErrors(local);

		auto tickbase = local->m_nTickBase();

		(*this)->SetupMove(local, cmd, movehelper, reinterpret_cast< CMoveData* >(cMoveData));
		movement->ProcessMovement(local, reinterpret_cast< CMoveData* >(cMoveData));
		(*this)->FinishMove(local, cmd, reinterpret_cast< CMoveData* >(cMoveData));

		local->m_nTickBase() = tickbase;
		movement->FinishTrackPredictionErrors(local);

		local->m_pCurrentCommand() = nullptr;
		*m_pRandomSeed = -1;
		*m_pPredictionPlayer = 0;
		movehelper->SetHost(NULL);

		local->m_fFlags() = flags;
		cmd->buttons = buttons;

	}
	void CGameMovement::FixMovement(SourceEngine::CUserCmd* cmd, SourceEngine::QAngle old_angles)
	{
		Vector move(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		float speed = move.Length();
		QAngle move_angle;
		Math::VectorAngles(move, move_angle);

		float yaw = DEG2RAD(cmd->viewangles.yaw - old_angles.yaw + move_angle.yaw);

		cmd->forwardmove = cos(yaw) * speed;
		cmd->sidemove = sin(yaw) * speed;

		if (cmd->forwardmove < -450) cmd->forwardmove = -450;
		if (cmd->sidemove < -450) cmd->sidemove = -450;
		if (cmd->forwardmove > 450) cmd->forwardmove = 450;
		if (cmd->sidemove > 450) cmd->sidemove = 450;
	}
}