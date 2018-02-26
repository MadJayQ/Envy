#pragma once

#include "csgosdk.h"
#include "structs.h"


namespace Envy
{
	class Resolver
	{
	public:
		Resolver() {}

		void UpdateResolverData(SourceEngine::C_BasePlayer* player, const SourceEngine::LagRecord& record);


		SourceEngine::LagRecord GetResolverRecord(SourceEngine::C_BasePlayer* player);

	private:
		std::array<SourceEngine::LagRecord, 64> m_LBYTUpdateRecords;
		std::array<SourceEngine::LagRecord, 64> m_LatestUpdateRecords;
	};
}