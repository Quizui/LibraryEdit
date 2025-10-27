
#ifndef __SONIKLIB_SONIKSTRINGBASED_DEFINITION_H__
#define __SONIKLIB_SONIKSTRINGBASED_DEFINITION_H__

#include "./SonikStringEnums.h"
#include "../SmartPointer/SonikSmartPointer.hpp"

 //C++20 以前はchar8_t が無いので。
#if __cplusplus < 202002L
	//C++20 以前であれば　char8_t は uint8_t
#include <stdint.h>
using utf8_t = uint8_t;
#else
using utf8_t = char8_t;

#endif


namespace SonikLib
{	//スプリット用クラス(Stringクラスそのままコピーはマルチスレッドアクセスでのnewでロックがかかってしまって遅くなるので..。
	class SonikStringSplitObject;
	//スプリットクラスで使う用のクラスの前方宣言
	class SonikString;
	class SonikStringWIDE;
	class SonikStringUTF8;
	class SonikStringUTF16;
};

namespace BASED_STRINGCLASS_SONIKLIB
{
	class SonikStringBase
	{
	protected:
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	protected:
		class SonikString_pImpl;
		SonikString_pImpl* pImpl;

	protected:
		//コンストラクタ
		SonikStringBase(void);
		//コピーコンストラクタ
		SonikStringBase(const SonikStringBase&) = delete;
		//ムーヴコンストラクタ
		SonikStringBase(SonikStringBase&&) = delete;
		//代入演算子
		SonikStringBase& operator =(const SonikStringBase&) = delete;

	public:
		~SonikStringBase(void);

		//ロケールを設定します。
		bool SetStringLocale(SonikLibStringConvert::SonikLibConvertLocale _setlocale_);

		//SJIS形式に変換して取得します。(バッファタイプも書き換わります。)
		const char* str_c(void);
		//wchar_t形式に変換して取得します。(バッファタイプも書き換わります。)
		const wchar_t* str_wchar(void);
		//UTF16形式に変換して取得します。(バッファタイプも書き換わります。)
		const char16_t* str_utf16(void);
		//UTF8形式に変換して取得します。(バッファタイプも書き換わります。)
		const utf8_t* str_utf8(void);

		//SJIS形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
		//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
		uint64_t GetCpy_str_c(char* dstBuffer = nullptr);
		//wchar_t形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
		//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
		uint64_t GetCpy_str_wcstr(wchar_t* dstBuffer = nullptr);
		//UTF16形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
		//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
		uint64_t GetCpy_str_utf16(char16_t* dstBuffer = nullptr);
		//UTF8形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
		//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
		uint64_t GetCpy_str_utf8(utf8_t* dstBuffer = nullptr);

		//c:文字列のByte数を取得します。（Null終端文字をカウントに含まない)
		uint64_t Count_Byte_NotNull(void);
		//c:文字列数を取得します。（Null終端文字をカウントに含まない)
		uint64_t Count_Str_NotNull(void);

		//指定したAsciiコードをデリミタとしてSplitを行います。
		bool Split_Ascii(const char* delim, SonikLib::SonikStringSplitObject& _split_);

		//文字列中の全角英数字を半角英数字に変換します。
		bool ConvertFWANtoHWAN(void);
		//文字列中の全角カナを半角カナに変換します。
		bool ConvertFWKNtoHWKN(void);

		//c: 指定位置の文字を削除します。
		void EraseChar(uint64_t ChrPoint);
		//c: 指定の開始位置から指定された文字数を削除します。
		void EraseStr(uint64_t ChrStartPoint, uint64_t ChrEndPoint);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const char* Str);
		bool operator ==(const char16_t* w_Str);
		bool operator ==(const wchar_t* w_Str);
		bool operator ==(const utf8_t* utf8_Str);

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const char* Str);
		bool operator !=(const char16_t* w_Str);
		bool operator !=(const wchar_t* w_Str);
		bool operator !=(const utf8_t* utf8_Str);

	};

};


namespace SonikLib
{
	class SonikStringSplitObject
	{
		friend class BASED_STRINGCLASS_SONIKLIB::SonikStringBase;

	private:
		char* mp_split;
		uint64_t m_splitCnt;
		uint64_t textbuffersize;
		SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface> m_allocator;

	private:
		SonikStringSplitObject(const SonikStringSplitObject& _copy_) = delete;
		SonikStringSplitObject(SonikStringSplitObject&& _move_) = delete;
		SonikStringSplitObject& operator =(const SonikStringSplitObject& _copy_) = delete;
		SonikStringSplitObject& operator =(SonikStringSplitObject&& _move_) = delete;

	public:
		SonikStringSplitObject(void);
		SonikStringSplitObject(SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface> _allocator_);
		~SonikStringSplitObject(void);

		bool GetStr(uint64_t _splitnum_, SonikString& _getstr_);
		bool GetStr(uint64_t _splitnum_, SonikStringWIDE& _getstr_);
		bool GetStr(uint64_t _splitnum_, SonikStringUTF8& _getstr_);
		bool GetStr(uint64_t _splitnum_, SonikStringUTF16& _getstr_);

		bool Split(uint64_t _splitnum_, const char* _delim_, SonikStringSplitObject& _out_);

	};
};


#endif /* __SONIKLIB_SONIKSTRINGBASED_DEFINITION_H__ */