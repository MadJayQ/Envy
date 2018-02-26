#pragma once

#include "csgosdk.h"
#include "structs.h"

namespace Envy
{
	class ESP
	{
	public:
		ESP();
		void Setup(SourceEngine::C_BaseEntity* ent);
		void Watermark();
		void DrawSpreadCircle();
		void Exec();
		void DrawAngleLines(float real, float fake, float lby);


		void OverrideBoneMatrix(SourceEngine::C_BasePlayer* player, SourceEngine::matrix3x4_t* m);
		bool ShouldOverride(int idx) { return m_bBoneMatrixOverriden[idx]; }
		SourceEngine::matrix3x4_t* GetOverridenMatrix(int idx) { return m_matBoneMatrices[idx].data(); }

		void ResetOverrideMatrix(int idx) 
		{
			if (idx >= 64) return;
			if (idx <= 0) return;
			m_bBoneMatrixOverriden[idx] = false; 
		}

	private:

		std::array<bool, 64> m_bBoneMatrixOverriden;
		std::array<std::array<SourceEngine::matrix3x4_t, MAXSTUDIOBONES>, 64> m_matBoneMatrices;
	};
}