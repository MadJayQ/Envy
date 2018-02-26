#pragma once

#include <iostream>
#include <ntops.h>
#include <envydefs.hpp>

constexpr static const char* g_szBuildDate = __DATE__;
constexpr static const char* g_szBuildTime = __TIME__;
constexpr static const char* g_szVersionNumber = "Alpha v0.0.0";

constexpr static struct _VERSION_INFO
{
	const char* szBuildDate = __DATE__;
	const char* szBuildTime = __TIME__;
	const char* szVersionNumber = "Windows Alpha v1.3.1b - UNSTABLE";
	const char* szAuthor = "MadJayQ";
	const char* szName = "Envy - CS ";

	friend std::ostream& operator << (std::ostream& os, const _VERSION_INFO& version)
	{
		os << version.szName << " ";
		os << version.szVersionNumber << " ";
		os << "built: " << version.szBuildDate << "@" << version.szBuildTime << " EST ";
		os << "by: " << version.szAuthor << std::endl;
		return os;
	}
} g_Version; 

namespace Envy
{
	void ENVY_API Envy_Entry(uintptr_t instance);
}