#include "mathfuncs.h"

using namespace Envy::SourceEngine;

namespace Envy
{
	namespace SourceEngine
	{
		namespace Math
		{
			void SinCos(float radians, PFLOAT sine, PFLOAT cosine)
			{
				__asm
				{
					fld dword ptr[radians]
					fsincos
					mov edx, dword ptr[cosine]
					mov eax, dword ptr[sine]
					fstp dword ptr[edx]
					fstp dword ptr[eax]
				}
			}
			void NormalizeAngles(QAngle& angles)
			{
				for (auto i = 0; i < 3; i++) {
					while (angles[i] < -180.0f) angles[i] += 360.0f;
					while (angles[i] > 180.0f) angles[i] -= 360.0f;
				}
			}
			//--------------------------------------------------------------------------------
			void ClampAngles(QAngle& angles)
			{
				if (angles.pitch > 89.0f) angles.pitch = 89.0f;
				else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

				if (angles.yaw > 180.0f) angles.yaw = 180.0f;
				else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

				angles.roll = 0;
			}
			//--------------------------------------------------------------------------------
			void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
			{
				out[0] = in1.Dot(in2[0]) + in2[0][3];
				out[1] = in1.Dot(in2[1]) + in2[1][3];
				out[2] = in1.Dot(in2[2]) + in2[2][3];
			}
			//--------------------------------------------------------------------------------
			void AngleVectors(const QAngle &angles, Vector& forward)
			{
				float	sp, sy, cp, cy;

				DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
				DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

				forward.x = cp*cy;
				forward.y = cp*sy;
				forward.z = -sp;
			}
			//--------------------------------------------------------------------------------
			void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
			{
				float sr, sp, sy, cr, cp, cy;

				DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
				DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
				DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

				forward.x = (cp * cy);
				forward.y = (cp * sy);
				forward.z = (-sp);
				right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
				right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
				right.z = (-1 * sr * cp);
				up.x = (cr * sp * cy + -sr*-sy);
				up.y = (cr * sp * sy + -sr*cy);
				up.z = (cr * cp);
			}
			//--------------------------------------------------------------------------------
			void VectorAngles(const Vector& forward, QAngle& angles)
			{
				float	tmp, yaw, pitch;

				if (forward[1] == 0 && forward[0] == 0) {
					yaw = 0;
					if (forward[2] > 0)
						pitch = 270;
					else
						pitch = 90;
				}
				else {
					yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
					if (yaw < 0)
						yaw += 360;

					tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
					pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
					if (pitch < 0)
						pitch += 360;
				}

				angles[0] = pitch;
				angles[1] = yaw;
				angles[2] = 0;
			}
			//--------------------------------------------------------------------------------
			static bool screen_transform(const Vector& in, Vector& out)
			{

				static auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>()->get();
				static auto& w2sMatrix = engine->WorldToScreenMatrix();

				out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
				out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
				out.z = 0.0f;

				float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

				if (w < 0.001f) {
					out.x *= 100000;
					out.y *= 100000;
					return false;
				}

				out.x /= w;
				out.y /= w;

				return true;
			}
			//--------------------------------------------------------------------------------
			bool WorldToScreen(const Vector& in, Vector& out)
			{
				if (screen_transform(in, out)) {
					int w, h;
					static auto engine = Interfaces::Instance()->GetInterface<IVEngineClient>()->get();
					engine->GetScreenSize(w, h);

					out.x = (w / 2.0f) + (out.x * w) / 2.0f;
					out.y = (h / 2.0f) - (out.y * h) / 2.0f;

					return true;
				}
				return false;
			}
			void AngleMatrix(const SourceEngine::QAngle & angles, const SourceEngine::Vector & position, SourceEngine::matrix3x4_t & matrix_out)
			{
				AngleMatrix(angles, matrix_out);
				MatrixSetColumn(position, 3, matrix_out);
			}
			void AngleMatrix(const SourceEngine::QAngle & angles, SourceEngine::matrix3x4_t & matrix)
			{
				float sr, sp, sy, cr, cp, cy;

				SinCos(DEG2RAD(angles[YAW]), &sy, &cy); //yaw
				SinCos(DEG2RAD(angles[PITCH]), &sp, &cp); //pitch
				SinCos(DEG2RAD(angles[ROLL]), &sr, &cr); //roll

														 // matrix = (YAW * PITCH) * ROLL
				matrix[0][0] = cp * cy;
				matrix[1][0] = cp * sy;
				matrix[2][0] = -sp;

				float crcy = cr * cy;
				float crsy = cr * sy;
				float srcy = sr * cy;
				float srsy = sr * sy;

				matrix[0][1] = sp * srcy - crsy;
				matrix[1][1] = sp * srsy + crcy;
				matrix[2][1] = sr * cp;

				matrix[0][2] = (sp * crcy + srsy);
				matrix[1][2] = (sp * crsy - srcy);
				matrix[2][2] = cr * cp;

				matrix[0][3] = 0.0f;
				matrix[1][3] = 0.0f;
				matrix[2][3] = 0.0f;
			}
			void MatrixSetColumn(const SourceEngine::Vector & in, int column, SourceEngine::matrix3x4_t & out)
			{
				out[0][column] = in.x;
				out[1][column] = in.y;
				out[2][column] = in.z;
			}
			float GetFOV(const SourceEngine::Vector & from, const SourceEngine::Vector & to, const SourceEngine::QAngle & viewangles)
			{
#if 0
				Vector Forward;
				Math::AngleVectors(viewangles, Forward);

				// Get delta vector between our local eye position and passed vector
				Vector Delta = to - from;

				float Distance = Delta.Length();

				// Normalize our delta vector
				Delta.NormalizeInPlace();

				// Get dot product between delta position and directional forward vectors
				float DotProduct = Forward.Dot(Delta);

				// Time to calculate the field of view
				auto final = QAngle(0.f, RAD2DEG(sin(DEG2RAD(acos(DotProduct))) * Distance), 0.f);
				Math::NormalizeAngles(final);
				return final.yaw;
#endif
				QAngle angles;
				Vector delta = to - from;
				delta.NormalizeInPlace();
				Math::VectorAngles(delta, angles);
				QAngle angle_delta = (angles - viewangles);
				Math::NormalizeAngles(angle_delta);
				return angle_delta.Length();
			}
			float AngleDiff(float destAngle, float srcAngle)
			{
				float delta;

				delta = fmodf(destAngle - srcAngle, 360.0f);
				if (destAngle > srcAngle)
				{
					if (delta >= 180)
						delta -= 360;
				}
				else
				{
					if (delta <= -180)
						delta += 360;
				}
				return delta;
			}
			float anglemod(float a)
			{
				a = (360.0 / 65536) * ((int)(a*(65536 / 360.0)) & 65535);
				return a;
			}
			float ApproachAngle(float target, float value, float speed)
			{
				target = anglemod(target);
				value = anglemod(value);

				float delta = target - value;

				// Speed is assumed to be positive
				if (speed < 0)
					speed = -speed;

				if (delta < -180)
					delta += 360;
				else if (delta > 180)
					delta -= 360;

				if (delta > speed)
					value += speed;
				else if (delta < -speed)
					value -= speed;
				else
					value = target;

				return value;
			}
		}
	}
}