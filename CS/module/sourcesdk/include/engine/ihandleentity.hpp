#pragma once

namespace Envy
{
	namespace SourceEngine
	{
		class CBaseHandle;

		class IHandleEntity
		{
		public:
			virtual ~IHandleEntity() {}
			virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
			virtual const CBaseHandle& GetRefEHandle() const = 0;
		};
	}
}