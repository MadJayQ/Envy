#include "envy.h"

#define ENVY_INTERNAL
#include "peb.h"
#include "address.h"
#include "function.hpp"
#include "interfaces.h"
#include "netvars.h"
#include "subsystem.h"
#include "vmtmanager.h"
#include "clientdll.hpp"
#include "hooks.h"
#include "csgosdk.h"
#include "structs.h"

#include "visualsys.h"

using namespace Envy;
using namespace Envy::SourceEngine;

namespace Envy
{
	std::unique_ptr<CheatSubsystems> g_Subsystems;

	void ENVY_API Envy_Entry(
		uintptr_t instance
		)
	{
		AllocConsole();
		AttachConsole(
			GetCurrentProcessId()
			);
		FILE* STANDARD_OUT;
		freopen_s(
			&STANDARD_OUT,
			"CON",
			"w",
			__acrt_iob_func(
				STD_OSTREAM
				)
			);

		std::cout << "Size of CUserCmd Struct is: 0x" << std::hex << sizeof(CUserCmd) << std::dec << std::endl;
		assert(sizeof(CUserCmd) == 0x64);

		g_Subsystems = std::make_unique<CheatSubsystems>();

		std::cout << g_Version;
		std::cout << "Enumerating the PEB...";
		auto peb = Peb::Instance();
		std::cout << "Done!" << std::endl;
		Netvars::Instance();

		InitializeHooks();
	}
}

