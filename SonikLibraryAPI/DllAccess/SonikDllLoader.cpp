/*
 * SonikDllLoader_x64.cpp
 *
 *  Created on: 2019/02/16
 *      Author: SONIK
 */

#if defined(_WIN32) || defined(_WIN64)

#include "SonikDllLoader.h"

namespace SonikDllLoader
{
	HINSTANCE WINDOWS_LoadLibrary(const char* LibFileName, DWORD dwFlags)
	{
		return LoadLibraryExA(LibFileName, 0, dwFlags);
	};

	HINSTANCE WINDOWS_LoadLibrary(const char16_t* LibFileName, DWORD dwFlags)
	{
		return LoadLibraryExW(reinterpret_cast<wchar_t*>(const_cast<char16_t*>(LibFileName)), 0, dwFlags);
	};

	HINSTANCE WINDOWS_LoadLibrary(const wchar_t* LibFileName, DWORD dwFlags)
	{
		return LoadLibraryExW(LibFileName, 0, dwFlags);
	};

	BOOL WINDOWS_FreeLibrary(HMODULE hModule)
	{
		return FreeLibrary(hModule);
	};

	FARPROC WINDOWS_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
	{
		return GetProcAddress(hModule, lpProcName);
	};

};

#elif defined(__linux__)
//linux function

#endif

