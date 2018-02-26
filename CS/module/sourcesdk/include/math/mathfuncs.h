#pragma once

#include "csgosdk.h"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define PITCH 0
#define YAW 1
#define ROLL 2

namespace Envy
{
	namespace SourceEngine
	{
		namespace Math
		{
			void NormalizeAngles(SourceEngine::QAngle& angles);
			void ClampAngles(SourceEngine::QAngle& angles);
			void VectorTransform(const SourceEngine::Vector& in1, const SourceEngine::matrix3x4_t& in2, SourceEngine::Vector& out);
			void AngleVectors(const SourceEngine::QAngle &angles, SourceEngine::Vector& forward);
			void AngleVectors(const SourceEngine::QAngle &angles, SourceEngine::Vector& forward, SourceEngine::Vector& right, SourceEngine::Vector& up);
			void VectorAngles(const SourceEngine::Vector& forward, SourceEngine::QAngle& angles);
			bool WorldToScreen(const SourceEngine::Vector& in, SourceEngine::Vector& out);
			void AngleMatrix(const SourceEngine::QAngle &angles, const SourceEngine::Vector &position, SourceEngine::matrix3x4_t& matrix_out);
			void AngleMatrix(const SourceEngine::QAngle &angles, SourceEngine::matrix3x4_t& matrix);
			void MatrixSetColumn(const SourceEngine::Vector &in, int column, SourceEngine::matrix3x4_t& out);
			float GetFOV(const SourceEngine::Vector& from, const SourceEngine::Vector& to, const SourceEngine::QAngle& viewangles);
			float AngleDiff(float destAngle, float srcAngle);
			float ApproachAngle(float target, float value, float speed);
		}
	}
}