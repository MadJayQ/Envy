#pragma once

#include "subsystem.h"
#include "csgosdk.h"
#include "aimbot.h"
#include "antiaim.h"
#include "triggerbot.h"

#include <memory>

namespace Envy
{
	class ViewangleSubssytem : public ICheatSubsystem
	{
	public:
		ViewangleSubssytem()
		{

		}
		virtual void Init() override;
		void OnCreateMove(SourceEngine::CUserCmd* usercmd, bool& sendpacket, int sequence);
		void OnFrameStageNotify(SourceEngine::ClientFrameStage_t stage);
		void OnPredictionUpdate(int startframe, bool validframe, int incomingack, int outgoingcmd);
		int& m_nTickbaseShift() { return _m_nTickbaseShift; }
	private:
		int _m_nTickbaseShift = 0;
		int m_nLastTickbase = 0;
		int m_nProcessedCMD;
		SourceEngine::QAngle m_safeAngle;
		SourceEngine::QAngle m_angRealAngle;
		SourceEngine::QAngle m_angRenderAngle;
	private:
		std::unique_ptr<Aimbot> m_Aimbot;
		std::unique_ptr<AntiAim> m_AntiAim;
		std::unique_ptr<Triggerbot> m_Triggerbot;
	};


}