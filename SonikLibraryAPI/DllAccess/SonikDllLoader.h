/*
 * SonikDllLoader.h
 *
 *  Created on: 2019/02/16
 *      Author: SONIK
 */

#ifndef SONIKDLLLOADER_H_
#define SONIKDLLLOADER_H_

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	#define SONIKDEF_WINDOWS

	#define SonikLibLoder SonikDllLoader::WINDOWS_LoadLibrary
	#define SonikLibLoderFree SonikDllLoader::WINDOWS_FreeLibrary
	#define SonikLibLoderProcAddress SonikDllLoader::WINDOWS_GetProcAddress

#elif defined(__linux__)
	#define SONIKDEF_LINUX
	//#include LinuxFile

#endif

namespace SonikDllLoader
{

#if defined(_WIN32) || defined(_WIN64)

    enum LibLoderFlag : int64_t
    {
        LOAD_DEFAULT = LOAD_WITH_ALTERED_SEARCH_PATH,
    };

	HINSTANCE WINDOWS_LoadLibrary(const char* LibFileName, DWORD dwFlags);
	HINSTANCE WINDOWS_LoadLibrary(const char16_t* LibFileName, DWORD dwFlags);
	HINSTANCE WINDOWS_LoadLibrary(const wchar_t* LibFileName, DWORD dwFlags);
	BOOL WINDOWS_FreeLibrary(HMODULE hModule);
	FARPROC WINDOWS_GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

#elif defined(__linux__)
    enum LibLoderFlag : int64_t
    {
        LOAD_DEFAULT = RTLD_LAZY | RTLD_LOCAL,
    };


#endif //WIN or Linux

};



#endif /* SONIKDLLLOADER_H_ */
