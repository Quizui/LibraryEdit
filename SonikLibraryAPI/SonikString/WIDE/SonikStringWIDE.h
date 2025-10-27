
#ifndef __SONIKLIB_SONIKSTRING_WIDE_H__
#define __SONIKLIB_SONIKSTRING_WIDE_H__

#include "../SonikStringBase.h"

 //前方宣言===================================
namespace SonikLib
{
	namespace Container
	{
		template <class T>
		class SonikVariableArrayContainer;
	};

	class SonikString;
	class SonikStringUTF8;
	class SonikStringUTF16;
};
//=========================================


namespace SonikLib
{
	class SonikStringWIDE : public BASED_STRINGCLASS_SONIKLIB::SonikStringBase
	{
		friend class SonikString;
		friend class SonikStringUTF8;
		friend class SonikStringUTF16;

	public:
		SonikStringWIDE(SonikLibStringConvert::SonikLibConvertLocale _setlocale_ = SonikLibStringConvert::SonikLibConvertLocale::LC_JPN);
		SonikStringWIDE(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, SonikLibStringConvert::SonikLibConvertLocale _setlocale_ = SonikLibStringConvert::SonikLibConvertLocale::LC_JPN);
		SonikStringWIDE(const SonikStringWIDE& t_his);
		SonikStringWIDE(const SonikString& t_his);
		SonikStringWIDE(const SonikStringUTF8& t_his);
		SonikStringWIDE(const SonikStringUTF16& t_his);
		SonikStringWIDE(const char* SetStr);
		SonikStringWIDE(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const char* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const char* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const char16_t* SetStr);
		SonikStringWIDE(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const char16_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const char16_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const wchar_t* SetStr);
		SonikStringWIDE(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const wchar_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const wchar_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const utf8_t* SetStr);
		SonikStringWIDE(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const utf8_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const utf8_t* SetStr, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int8_t SetValue);
		SonikStringWIDE(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int8_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const int8_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint8_t SetValue);
		SonikStringWIDE(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint8_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const uint8_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int16_t SetValue);
		SonikStringWIDE(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int16_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const int16_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint16_t SetValue);
		SonikStringWIDE(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint16_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const uint16_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int32_t SetValue);
		SonikStringWIDE(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int32_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const int32_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint32_t SetValue);
		SonikStringWIDE(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint32_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const uint32_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int64_t SetValue);
		SonikStringWIDE(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const int64_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const int64_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint64_t SetValue);
		SonikStringWIDE(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const uint64_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const uint64_t SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const float SetValue);
		SonikStringWIDE(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const float SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const float SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const double SetValue);
		SonikStringWIDE(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringWIDE(const double SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_);
		SonikStringWIDE(const double SetValue, SonikLibStringConvert::SonikLibConvertLocale _setlocale_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//ムーヴコンストラクタ
		SonikStringWIDE(SonikString&& _move_) noexcept;
		SonikStringWIDE(SonikStringWIDE&& _move_) noexcept;
		SonikStringWIDE(SonikStringUTF8&& _move_) noexcept;
		SonikStringWIDE(SonikStringUTF16&& _move_) noexcept;

		//デストラクタ
		~SonikStringWIDE(void);

		//define切り替えのstrゲット
		const wchar_t* definition_str(void);

		//define切り替えのStrCopy
		uint64_t GetCpy_str_definition(wchar_t* dstBuffer = nullptr);

		//各入力フォーマットから現在のバッファに変換して代入します。
		SonikStringWIDE& operator =(const SonikStringWIDE& t_his);
		SonikStringWIDE& operator =(const SonikString& t_his);
		SonikStringWIDE& operator =(const SonikStringUTF8& t_his);
		SonikStringWIDE& operator =(const SonikStringUTF16& t_his);
		SonikStringWIDE& operator =(SonikString&& _move_) noexcept;
		SonikStringWIDE& operator =(SonikStringWIDE&& _move_) noexcept;
		SonikStringWIDE& operator =(SonikStringUTF8&& _move_) noexcept;
		SonikStringWIDE& operator =(SonikStringUTF16&& _move_) noexcept;
		SonikStringWIDE& operator =(const char* Str);
		SonikStringWIDE& operator =(const char16_t* w_Str);
		SonikStringWIDE& operator =(const wchar_t* w_Str);
		SonikStringWIDE& operator =(const utf8_t* utf8_Str);
		SonikStringWIDE& operator =(const int8_t SetValue);
		SonikStringWIDE& operator =(const uint8_t SetValue);
		SonikStringWIDE& operator =(const int16_t SetValue);
		SonikStringWIDE& operator =(const uint16_t SetValue);
		SonikStringWIDE& operator =(const int32_t SetValue);
		SonikStringWIDE& operator =(const uint32_t SetValue);
		SonikStringWIDE& operator =(const int64_t SetValue);
		SonikStringWIDE& operator =(const uint64_t SetValue);
		SonikStringWIDE& operator =(const float SetValue);
		SonikStringWIDE& operator =(const double SetValue);

		//各入力フォーマットから現在のバッファに変換して結合します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringWIDE& operator +=(const SonikStringWIDE& t_his);
		SonikStringWIDE& operator +=(const SonikString& t_his);
		SonikStringWIDE& operator +=(const SonikStringUTF8& t_his);
		SonikStringWIDE& operator +=(const SonikStringUTF16& t_his);
		SonikStringWIDE& operator +=(const char* Str);
		SonikStringWIDE& operator +=(const char16_t* w_Str);
		SonikStringWIDE& operator +=(const wchar_t* w_Str);
		SonikStringWIDE& operator +=(const utf8_t* utf8_Str);
		SonikStringWIDE& operator +=(const int8_t SetValue);
		SonikStringWIDE& operator +=(const uint8_t SetValue);
		SonikStringWIDE& operator +=(const int16_t SetValue);
		SonikStringWIDE& operator +=(const uint16_t SetValue);
		SonikStringWIDE& operator +=(const int32_t SetValue);
		SonikStringWIDE& operator +=(const uint32_t SetValue);
		SonikStringWIDE& operator +=(const int64_t SetValue);
		SonikStringWIDE& operator +=(const uint64_t SetValue);
		SonikStringWIDE& operator +=(const float SetValue);
		SonikStringWIDE& operator +=(const double SetValue);

		//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringWIDE operator +(const SonikStringWIDE& t_his);
		SonikStringWIDE operator +(const SonikString& t_his);
		SonikStringWIDE operator +(const SonikStringUTF8& t_his);
		SonikStringWIDE operator +(const SonikStringUTF16& t_his);
		SonikStringWIDE operator +(const char* Str);
		SonikStringWIDE operator +(const char16_t* w_Str);
		SonikStringWIDE operator +(const wchar_t* w_Str);
		SonikStringWIDE operator +(const utf8_t* utf8_Str);
		SonikStringWIDE operator +(const int8_t SetValue);
		SonikStringWIDE operator +(const uint8_t SetValue);
		SonikStringWIDE operator +(const int16_t SetValue);
		SonikStringWIDE operator +(const uint16_t SetValue);
		SonikStringWIDE operator +(const int32_t SetValue);
		SonikStringWIDE operator +(const uint32_t SetValue);
		SonikStringWIDE operator +(const int64_t SetValue);
		SonikStringWIDE operator +(const uint64_t SetValue);
		SonikStringWIDE operator +(const float SetValue);
		SonikStringWIDE operator +(const double SetValue);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const SonikStringWIDE& t_his) const;
		bool operator ==(const SonikString& t_his) const;
		bool operator ==(const SonikStringUTF8& t_his) const;
		bool operator ==(const SonikStringUTF16& t_his) const;

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const SonikStringWIDE& t_his) const;
		bool operator !=(const SonikString& t_his) const;
		bool operator !=(const SonikStringUTF8& t_his) const;
		bool operator !=(const SonikStringUTF16& t_his) const;

		//小なり
		bool operator <(const SonikStringWIDE& _Greater_) const;
		bool operator <(const SonikString& _Greater_) const;
		bool operator <(const SonikStringUTF8& _Greater_) const;
		bool operator <(const SonikStringUTF16& _Greater_) const;

		//大なり
		bool operator >(const SonikStringWIDE& _Less_) const;
		bool operator >(const SonikString& _Less_) const;
		bool operator >(const SonikStringUTF8& _Less_) const;
		bool operator >(const SonikStringUTF16& _Less_) const;

		//小なりイコール
		bool operator <=(const SonikStringWIDE& _GreaterEqual_) const;
		bool operator <=(const SonikString& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF8& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF16& _GreaterEqual_) const;

		//大なりイコール
		bool operator >=(const SonikStringWIDE& _LessEqual_) const;
		bool operator >=(const SonikString& _LessEqual_) const;
		bool operator >=(const SonikStringUTF8& _LessEqual_) const;
		bool operator >=(const SonikStringUTF16& _LessEqual_) const;

	};

};

#endif /* __SONIKLIB_SONIKSTRING_WIDE_H__ */