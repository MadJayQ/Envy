#include "envy.h"

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hinstDLL);
			CreateThread(
				NULL,
				NULL,
				reinterpret_cast<LPTHREAD_START_ROUTINE>(Envy::Envy_Entry),
				hinstDLL,
				NULL,
				NULL
			);
			break;
		}
		case DLL_PROCESS_DETACH: 
		{
			RemoveFontResourceEx(
				"F:\\icons.ttf", // font file name
				FR_PRIVATE,             // font characteristics
				NULL             // reserved
			);
			break;
		}
	}
	return TRUE;
}
