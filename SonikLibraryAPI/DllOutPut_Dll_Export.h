/*
 * DllExPort.h
 *
 *  Created on: 2015/12/28
 *      Author: SONIC
 */

#ifndef BAUNDARYTAGEXDLLOUTPUT_DLLEXPORT_H_
#define BAUNDARYTAGEXDLLOUTPUT_DLLEXPORT_H_

#ifdef _WIN32

#ifdef _SONIK_DLL_EXPORT_
#define DLLEXPORT __declspec(dllexport)

#else
#define DLLEXPORT

#endif

#elif _WIN64

#ifdef _SONIK_DLL_EXPORT_
#define DLLEXPORT __declspec(dllexport)

#else
#define DLLEXPORT

#endif

#elif __linux__

#ifdef _SONIK_DLL_EXPORT_
#define DLLEXPORT

#else
#define DLLEXPORT

#endif

#endif

#endif /* BAUNDARYTAGEXDLLOUTPUT_DLLEXPORT_H_ */
