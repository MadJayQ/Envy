#pragma once

#include "process.h"

class PELoader
{
public:
	PELoader(Process p);
	~PELoader();

	HMODULE InjectImage(const std::string& path);
	HMODULE InjectImage(byte* buffer, const size_t size);

private:
	Process target_process;
};