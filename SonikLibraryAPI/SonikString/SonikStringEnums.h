#pragma once

#ifndef SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_
#define SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_

#include "./TypePermissibleTemplate.hpp"
#include "../CPPGrammarDefines.h"

SLIB_CVR_ENUMCLASS(SonikLibConvertType)
{
	SCHTYPE_SJIS = 0,
	SCHTYPE_UTF8,
	SCHTYPE_BOMUTF8,
	SCHTYPE_UTF16,
	SCHTYPE_UTF32,
	SCHTYPE_UNKNOWN,
	SCHTYPE_NULLTEXT,
};

#if defined(__cplusplus) && __cplusplus >= 201103L
	#if __cplusplus >= 202002L
		using enum SonikLibConvertType;

	#else
		//SonikLibConvertType 定数を名前解決記述なしで使用するためのusing郡
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_SJIS		= SonikLibConvertType::SCHTYPE_SJIS;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF8		= SonikLibConvertType::SCHTYPE_UTF8;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_BOMUTF8	= SonikLibConvertType::SCHTYPE_BOMUTF8;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF16		= SonikLibConvertType::SCHTYPE_UTF16;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF32		= SonikLibConvertType::SCHTYPE_UTF32;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UNKNOWN	= SonikLibConvertType::SCHTYPE_UNKNOWN;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_NULLTEXT	= SonikLibConvertType::SCHTYPE_NULLTEXT;
	#endif

#else

	#if defined(_MSC_VER) && _MSC_VER >= 1900
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_SJIS		= SonikLibConvertType::SCHTYPE_SJIS;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF8		= SonikLibConvertType::SCHTYPE_UTF8;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_BOMUTF8	= SonikLibConvertType::SCHTYPE_BOMUTF8;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF16		= SonikLibConvertType::SCHTYPE_UTF16;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UTF32		= SonikLibConvertType::SCHTYPE_UTF32;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_UNKNOWN	= SonikLibConvertType::SCHTYPE_UNKNOWN;
		static SLIB_CVR_CONSTEXPR SonikLibConvertType SCHTYPE_NULLTEXT	= SonikLibConvertType::SCHTYPE_NULLTEXT;

	#else
		//define定義
		#define SCHTYPE_SJIS		SonikLibConvertType::SCHTYPE_SJIS
		#define SCHTYPE_UTF8		SonikLibConvertType::SCHTYPE_UTF8
		#define SCHTYPE_BOMUTF8		SonikLibConvertType::SCHTYPE_BOMUTF8
		#define SCHTYPE_UTF16		SonikLibConvertType::SCHTYPE_UTF16
		#define SCHTYPE_UTF32		SonikLibConvertType::SCHTYPE_UTF32
		#define SCHTYPE_UNKNOWN		SonikLibConvertType::SCHTYPE_UNKNOWN
		#define SCHTYPE_NULLTEXT	SonikLibConvertType::SCHTYPE_NULLTEXT
		#define SCHTYPE_SJIS		SonikLibConvertType::SCHTYPE_SJIS

	#endif
#endif

#endif /* SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_ */