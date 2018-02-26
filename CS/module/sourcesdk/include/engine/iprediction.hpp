#pragma once

#include "qangle.hpp"
#include "vector.hpp"
#include "cusercmd.hpp"
#include "imovehelper.hpp"
#include "vmtmanager.h"
#include "interfaces.h"

namespace Envy
{
	namespace SourceEngine
	{
		class CMoveData
		{
		public:
			bool    m_bFirstRunOfFunctions : 1;
			bool    m_bGameCodeMovedPlayer : 1;
			int     m_nPlayerHandle;        // edict index on server, client entity handle on client=
			int     m_nImpulseCommand;      // Impulse command issued.
			Vector  m_vecViewAngles;        // Command view angles (local space)
			Vector  m_vecAbsViewAngles;     // Command view angles (world space)
			int     m_nButtons;             // Attack buttons.
			int     m_nOldButtons;          // From host_client->oldbuttons;
			float   m_flForwardMove;
			float   m_flSideMove;
			float   m_flUpMove;
			float   m_flMaxSpeed;
			float   m_flClientMaxSpeed;
			Vector  m_vecVelocity;          // edict::velocity        // Current movement direction.
			Vector  m_vecAngles;            // edict::angles
			Vector  m_vecOldAngles;
			float   m_outStepHeight;        // how much you climbed this move
			Vector  m_outWishVel;           // This is where you tried 
			Vector  m_outJumpVel;           // This is your jump velocity
			Vector  m_vecConstraintCenter;
			float   m_flConstraintRadius;
			float   m_flConstraintWidth;
			float   m_flConstraintSpeedFactor;
			float   m_flUnknown[5];
			Vector  m_vecAbsOrigin;
		};

		class C_BasePlayer;

		class IGameMovement
		{
		public:
			virtual			~IGameMovement(void) {}

			virtual void	          ProcessMovement(C_BasePlayer *pPlayer, CMoveData *pMove) = 0;
			virtual void	          Reset(void) = 0;
			virtual void	          StartTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
			virtual void	          FinishTrackPredictionErrors(C_BasePlayer *pPlayer) = 0;
			virtual void	          DiffPrint(char const *fmt, ...) = 0;
			virtual Vector const&	  GetPlayerMins(bool ducked) const = 0;
			virtual Vector const&	  GetPlayerMaxs(bool ducked) const = 0;
			virtual Vector const&	  GetPlayerViewOffset(bool ducked) const = 0;
			virtual bool		      IsMovingPlayerStuck(void) const = 0;
			virtual C_BasePlayer*	  GetMovingPlayer(void) const = 0;
			virtual void		      UnblockPusher(C_BasePlayer *pPlayer, C_BasePlayer *pPusher) = 0;
			virtual void			  SetupMovementBounds(CMoveData *pMove) = 0;
		};

		class _CGameMovement_
			: public IGameMovement
		{
		public:
			virtual ~_CGameMovement_(void) {}
		};

		class _IPrediction_
		{
		public:
			bool InPrediction()
			{
				typedef bool(__thiscall* oInPrediction)(void*);
				//return CallVFunction<oInPrediction>(this, 14)(this);
				return VMTManager::Instance()->GetVirtualFunction<oInPrediction>(this, 14)(this);
			}

			void RunCommand(C_BasePlayer *player, CUserCmd *ucmd, _IMoveHelper_ *moveHelper)
			{
				typedef void(__thiscall* oRunCommand)(void*, C_BasePlayer*, CUserCmd*, _IMoveHelper_*);
				//return CallVFunction<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
				return VMTManager::Instance()->GetVirtualFunction<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
			}

			void SetupMove(C_BasePlayer *player, CUserCmd *ucmd, _IMoveHelper_ *moveHelper, void* pMoveData)
			{
				typedef void(__thiscall* oSetupMove)(void*, C_BasePlayer*, CUserCmd*, _IMoveHelper_*, void*);
				//return CallVFunction<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
				return VMTManager::Instance()->GetVirtualFunction<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
			}

			void FinishMove(C_BasePlayer *player, CUserCmd *ucmd, void*pMoveData)
			{
				typedef void(__thiscall* oFinishMove)(void*, C_BasePlayer*, CUserCmd*, void*);
				//return CallVFunction<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
				return VMTManager::Instance()->GetVirtualFunction<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
			}
		};
	}

	class CGameMovement : public SourceInterface<SourceEngine::_CGameMovement_>
	{
	public:
		CGameMovement() {}
		
		virtual Interface* Init() override
		{
			m_data = reinterpret_cast<Interface*>(
				Interfaces::Instance()->GetInterfaceAddr("GameMovement")
			);
			SourceInterface<SourceEngine::_CGameMovement_>::Init();
			return m_data;
		}

		void FixMovement(SourceEngine::CUserCmd* cmd, SourceEngine::QAngle angle);
	};

	class IPrediction : public SourceInterface<SourceEngine::_IPrediction_>
	{
	public:
		IPrediction() {}

		virtual Interface* Init() override
		{
			m_data = reinterpret_cast<Interface*>(
				Interfaces::Instance()->GetInterfaceAddr("VClientPrediction")
			);
			SourceInterface<SourceEngine::_IPrediction_>::Init();
			m_pRandomSeed = *(int**)(Peb::Instance()->GetModule("client.dll").FindPattern("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 0x2);
			m_pPredictionPlayer = *(int**)(Peb::Instance()->GetModule("client.dll").FindPattern("89 35 ? ? ? ? F3 0F 10 48 20") + 0x2);
			return m_data;
		}

		void RunCommand_Rebuilt(SourceEngine::CUserCmd* test);

	private:
		int* m_pPredictionPlayer;
		int* m_pRandomSeed;
	};


}