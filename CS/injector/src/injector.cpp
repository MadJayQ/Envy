#include <Windows.h>
#define WIN32_LEAN_AND_MEAN

#define ENVY_EXTERNAL

#include <direct.h>
#include <string>
#include <vector>
#include <iostream>

#include <helpers.hpp>
#include "process.h"
//#include "manualmap.h"
#include "nt-peloader.h"
#include "redirection-helpers.h"
#include "address.h"


int main(int argc, char** args)
{
	char directory[MAX_PATH];
	_getcwd(directory, sizeof(directory));
	const char* dll_name = "Envy Module";

	/*
	std::cout << "Building system file structure...";
	Helpers::Instance()->BuildSystemFilepaths(GetWindowsDirectories());
	std::cout << "Done!" << std::endl;
	std::cout << "Building File Redirection Hash Map...";
	RedirectionHelper::Instance();
	std::cout << "Done!" << std::endl;
	*/

	std::string file(directory);
	file.append("\\");
	file.append(dll_name);
	file.append(".dll");

	Process p = Process("csgo.exe");
	p.Attach();
	std::cout << p;
	//ManualMap map(p);
	//map.InjectImage(file);
	PELoader pe(p);
	ModuleFileInfo info = Helpers::Instance()->ReadModuleFile(file.c_str());
	std::cout << "MODULE LOADED AT: " << "0x" << std::hex << pe.InjectImage(file) << std::dec << std::endl;
	//ManualMap map(p);
	//map.PerformImageMap(
	//	image.data(),
		//"Cheat Module"
	//);
	p.Detach();

	return EXIT_SUCCESS;
}

