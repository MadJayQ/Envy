#pragma once

namespace Envy
{
	namespace SourceEngine
	{
		typedef float RadianEuler[3];
		class Quaternion				// same data-layout as engine's vec4_t,
		{
		public:
			inline Quaternion(void) {

				// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
				x = y = z = w = float_NAN;
#endif
#endif
			}
			inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }
			inline Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

			inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

			bool IsValid() const;
			void Invalidate();

			bool operator==(const Quaternion &src) const;
			bool operator!=(const Quaternion &src) const;

			float* Base() { return (float*)this; }
			const float* Base() const { return (float*)this; }

			// array access...
			float operator[](int i) const;
			float& operator[](int i);

			float x, y, z, w;
		};
		inline float& Quaternion::operator[](int i)
		{
			return ((float*)this)[i];
		}
		inline float Quaternion::operator[](int i) const
		{
			return ((float*)this)[i];
		}
		inline bool Quaternion::operator==(const Quaternion &src) const
		{
			return (x == src.x) && (y == src.y) && (z == src.z) && (w == src.w);
		}

		inline bool Quaternion::operator!=(const Quaternion &src) const
		{
			return !operator==(src);
		}


		inline Quaternion::Quaternion(RadianEuler const &angle)
		{
			//AngleQuaternion(angle, *this);
		}

		inline bool Quaternion::IsValid() const
		{
			//return IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w);
			return true;
		}

		inline void Quaternion::Invalidate()
		{
			//#ifdef _DEBUG
			//#ifdef VECTOR_PARANOIA
			//x = y = z = w = float_NAN;
			//#endif
			//#endif
		}

		class __declspec(align(16)) QuaternionAligned : public Quaternion
		{
		public:
			inline QuaternionAligned(void) {};
			inline QuaternionAligned(float X, float Y, float Z, float W)
			{
				Init(X, Y, Z, W);
			}

#ifdef VECTOR_NO_SLOW_OPERATIONS

		private:
			// No copy constructors allowed if we're in optimal mode
			QuaternionAligned(const QuaternionAligned& vOther);
			QuaternionAligned(const Quaternion &vOther);

#else
		public:
			explicit QuaternionAligned(const Quaternion &vOther)
			{
				Init(vOther.x, vOther.y, vOther.z, vOther.w);
			}

			QuaternionAligned& operator=(const Quaternion &vOther)
			{
				Init(vOther.x, vOther.y, vOther.z, vOther.w);
				return *this;
			}

#endif
		};
	}
}