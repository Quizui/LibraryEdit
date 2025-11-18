#ifndef __SONIKLIB_COMMON_PLATFORM_DEFINITIONS_H__
#define __SONIKLIB_COMMON_PLATFORM_DEFINITIONS_H__

//環境識別用のdefine分けをするヘッダです。

//OS PLATFORM DEFINITIONS=============================================
#if defined(_WIN32) || defined(_WIN64)
    #define SLIB_PLATFORM_DEFS_WINDOWS 1

#elif defined(__APPLE__) || defined(__MACH__)
    #define SLIB_PLATFORM_DEFS_MAC 1

#elif defined(__linux__) || defined(__unix__) || defined(__unix)
    #define SLIB_PLATFORM_DEFS_POSIX 1

#endif
//=====================================================================

//ToolChains DEFINITIONS===============================================
#if defined(__MINGW32__) || defined(__MINGW64__)
    #define SLIB_TOOLCHAIN_DEF_MINGW 1

#endif

#if defined(__CYGWIN__)
    #define SLIB_TOOLCHAIN_DEF_CYGWIN 1

#endif
//=====================================================================

//Compiler DEFINITIONS=================================================
#if defined(__INTEL_COMPILER) //ICC/ICPC（Intel Compiler Classic）
    #define SLIB_COMPILER_DEF_INTEL_CLASSIC 1

#elif defined(__INTEL_LLVM_COMPILER) //oneAPI ICX/ICPX（LLVM ベース）
    #define SLIB_COMPILER_DEF_INTEL_LLVM 1

#elif defined(_MSC_VER) // Microsoft Visual C++ (および Windows 向け Intel C++ も _MSC_VER が立つ)
    #define SLIB_COMPILER_DEF_MSVC 1

#elif defined(__clang__) //clang compiler
    #define SLIB_COMPILER_DEF_CLANG 1

#elif defined(__GNUC__) //GNUC(GCC) compiler
    #define SLIB_COMPILER_DEF_GCC 1

#endif
//=====================================================================

#endif //__SONIKLIB_COMMON_PLATFORM_DEFINITIONS_H__