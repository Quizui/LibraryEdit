#pragma once

#ifndef SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_
#define SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_

#include <stdint.h>

//C++20 以前はchar8_t が無いので。
#if __cplusplus < 202002L
	//C++20 以前であれば　char8_t は uint8_t
using utf8_t = uint8_t;
#else
#include <cuchar>
using utf8_t = char8_t;

#endif

enum class SonikLibConvertType
{
	SCHTYPE_SJIS = 0,
	SCHTYPE_UTF8,
	SCHTYPE_BOMUTF8,
	SCHTYPE_UTF16,
	SCHTYPE_UTF32,
	SCHTYPE_UNKNOWN,
	SCHTYPE_NULLTEXT,
};

//SonikLibConvertType 定数を名前解決記述なしで使用するためのusing郡
using SonikLibConvertType::SCHTYPE_SJIS;
using SonikLibConvertType::SCHTYPE_UTF8;
using SonikLibConvertType::SCHTYPE_BOMUTF8;
using SonikLibConvertType::SCHTYPE_UTF16;
using SonikLibConvertType::SCHTYPE_UTF32;
using SonikLibConvertType::SCHTYPE_UNKNOWN;
using SonikLibConvertType::SCHTYPE_NULLTEXT;





#endif /* SONIKSTRING_SONIKSTRINGLOCALEENMUS_H_ */