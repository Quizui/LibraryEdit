#pragma once

#ifndef SONIKString_SONIKStringCONVERT_H_
#define SONIKString_SONIKStringCONVERT_H_

#include "./SonikStringLocaleEnmus.h"
#include "../SmartPointer/SonikSmartPointer.hpp"

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

namespace SonikLibStringConvert
{
	//ロケール定数からロケール名を取得します。
	void ConvertLocaleCharacter(uint32_t& _out_size_, char* _buffer_, SonikLibConvertLocale _locale_);

	//文字列のタイプが、SJISかUTF-8かを判別します。
	//完全な判別は不可能（らしい）なので可能性の範囲でチェックされた結果を返却します。
	//ASCIIはSJISとして結果を返却します。
	//UTF-16 は返却値としては返りません。
	SonikLibConvertType CheckConvertType(const char* pCheckSrc);

	//文字列のタイプをチェックし、その文字列で使用されているバイト数を返却します。
	//文字列のタイプチェックは可能性の範囲のチェックとなります。
	//SJIS, UTF-8のみに対応。
	uint64_t GetStringLengthByte(const char* pCheckSrc);

	//unicode(UTF-16)文字の文字列バイト数を返却します。
	uint64_t GetStringLengthByte(const char16_t* pCheckSrc);

	//unicode(UTF-32)文字の文字列バイト数を返却します。
	//ただのループ。
	uint64_t GetStringLengthByte(const char32_t* pCheckSrc);

	//文字列のタイプをチェックし、その文字列で使用されている文字数を返却します。
	//文字列のタイプチェックは可能性の範囲のチェックとなります。
	//SJIS, UTF-8のみに対応。
	uint64_t GetStringCount(const char* pCheckSrc);

	//SJIS, UTF-8のみに対応。
	//第２引数に指定したタイプで強制的にカウントを行います。(SJIS混じりUTF8の対処)
	uint64_t GetStringCount(const char* pCheckSrc, SonikLibConvertType CountStrType);

	//unicode(UTF-16)文字の文字数を返却します。
	uint64_t GetStringCount(const char16_t* pCheckSrc);

	//unicode(UTF-32)文字の文字数を返却します。
	uint64_t GetStringCount(const char32_t* pCheckSrc);

	//UTF-8をUNICODE(UTF-32)に変換します。
	bool ConvertUTF8ToUTF32(const utf8_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF8ToUTF32(const utf8_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UNICODE(UTF-32)をUTF-8に変換します。
	bool ConvertUTF32ToUTF8(const char32_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF32ToUTF8(const char32_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UNICODE(UTF-32)をUNICODE(UTF-16)に変換します。
	bool ConvertUTF32ToUTF16(const char32_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF32ToUTF16(const char32_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UNICODE(UTF16)をUNICODE(UTF-32)に変換します。
	bool ConvertUTF16ToUTF32(const char16_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF16ToUTF32(const char16_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UNICODE(UTF-16)をUTF8に変換します。
	bool ConvertUTF16ToUTF8(const char16_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF16ToUTF8(const char16_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF8をUNICODE(UTF-16)に変換します。
	bool ConvertUTF8ToUTF16(const utf8_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize);
	bool ConvertUTF8ToUTF16(const utf8_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);


	//マルチバイト文字列をUTF8文字列に変換します。
	//第１引数の文字列は、可能性の判定として、SJIS判定であれば処理を行います。
	//第１引数の文字列に対して、Null終端がない場合の動作は、strlenと同様にバッファオーバーランを起こします。
	bool ConvertMBSToUTF8(const char* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertMBSToUTF8(const char* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF8文字列をマルチバイト文字列に変換します。
	//第１引数の文字列は、可能性の判定として、UTF8判定であれば処理を行います。
	//第１引数の文字列に対して、Null終端がない場合の動作は、strlenと同様にバッファオーバーランを起こします。
	bool ConvertUTF8ToMBS(const utf8_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertUTF8ToMBS(const utf8_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//マルチバイト文字列をUTF16文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	bool ConvertMBStoUTF16(const char* pSrc, char16_t* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertMBStoUTF16(const char* pSrc, char16_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF16文字列をマルチバイト文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	bool ConvertUTF16toMBS(const char16_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertUTF16toMBS(const char16_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//マルチバイト文字列をUTF32文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	bool ConvertMBStoUTF32(const char* pSrc, char32_t* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertMBStoUTF32(const char* pSrc, char32_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF32文字列をマルチバイト文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	bool ConvertUTF32toMBS(const char32_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale);
	bool ConvertUTF32toMBS(const char32_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF8文字列における、全角英数字及び、感嘆符(！(エクスクラメーションマーク))等の記号を半角英数字に変換します。
	//UTF-8における全角から半角変換において、領域オーバーをすることがないはずなのでそのまま領域を変換し、終端コードを最後に付与します。
	//第３引数は終端コードを含めた配列の使用要素数です。
	//FWC = Full Width Charcter
	//HWC = Half Width Charcter
	//AN = Alphabet and Number
	bool ConvertUTF8FWCToHWCForAN(const char* ConvertStr, char* dst, uint64_t& UsedSize);
	bool ConvertUTF8FWCToHWCForAN(const char* ConvertStr, char* dst, uint64_t& UsedSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	//UTF8文字列のおける、全角カナを半角カナに変換します。
	bool ConvertUTF8FWCToHWCForKANA(const char* ConvertStr, char* dst, uint64_t& UsedSize);
	bool ConvertUTF8FWCToHWCForKANA(const char* ConvertStr, char* dst, uint64_t& UsedSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

};

namespace SonikLibStringControl
{
	//c:指定された位置から指定された文字数を削除します。
	bool StringPointEraser(char* ControlStr, uint64_t StartPoint, uint64_t EndPoint, uint64_t MaxStrCnt);
};


#endif /* SONIKString_SONIKStringCSonikERT_H_ */