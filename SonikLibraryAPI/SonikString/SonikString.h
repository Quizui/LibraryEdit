
#ifndef __SONIKLIB_SONIKSTRING_H__
#define __SONIKLIB_SONIKSTRING_H__

#include "SonikStringBase.h"
#include "../CPPGrammarDefines.h"

 //前方宣言===================================
namespace SonikLib
{
	namespace Container
	{
		template <class T>
		class SonikVariableArrayContainer;
	};

	class SonikStringWIDE;
	class SonikStringUTF8;
	class SonikStringUTF16;
};
//=========================================


namespace SonikLib
{
	class SonikString : public BASED_STRINGCLASS_SONIKLIB::SonikStringBase
	{
		friend class SonikStringWIDE;
		friend class SonikStringUTF8;
		friend class SonikStringUTF16;

	public:
		//コンストラクタ
		SonikString(void);
		SonikString(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const SonikString& t_his);
		SonikString(const SonikStringWIDE& t_his);
		SonikString(const SonikStringUTF8& t_his);
		SonikString(const SonikStringUTF16& t_his);
		SonikString(const char* SetStr);
		SonikString(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const utf16_t* SetStr);
		SonikString(const utf16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const wchar_t* SetStr);
		SonikString(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const utf8_t* SetStr);
		SonikString(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const int8_t SetValue);
		SonikString(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const uint8_t SetValue);
		SonikString(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const int16_t SetValue);
		SonikString(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const uint16_t SetValue);
		SonikString(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const int32_t SetValue);
		SonikString(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const uint32_t SetValue);
		SonikString(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const int64_t SetValue);
		SonikString(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const uint64_t SetValue);
		SonikString(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const float SetValue);
		SonikString(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString(const double SetValue);
		SonikString(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//ムーヴコンストラクタ
		SonikString(SonikString&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;

		//デストラクタ
		~SonikString(void);

		//define切り替えのstrゲット
		const char* definition_str(void);
		//define切り替えのStrCopy
		uint64_t GetCpy_str_definition(char* dstBuffer);

		//各入力フォーマットから現在のバッファに変換して代入します。
		SonikString& operator =(const SonikString& t_his);
		SonikString& operator =(const SonikStringWIDE& t_his);
		SonikString& operator =(const SonikStringUTF8& t_his);
		SonikString& operator =(const SonikStringUTF16& t_his);
		SonikString& operator =(SonikString&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString& operator =(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString& operator =(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikString& operator =(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;

		SonikString& operator =(const char* Str);
		SonikString& operator =(const utf16_t* w_Str);
		SonikString& operator =(const wchar_t* w_Str);
		SonikString& operator =(const utf8_t* utf8_Str);
		SonikString& operator =(const int8_t SetValue);
		SonikString& operator =(const uint8_t SetValue);
		SonikString& operator =(const int16_t SetValue);
		SonikString& operator =(const uint16_t SetValue);
		SonikString& operator =(const int32_t SetValue);
		SonikString& operator =(const uint32_t SetValue);
		SonikString& operator =(const int64_t SetValue);
		SonikString& operator =(const uint64_t SetValue);
		SonikString& operator =(const float SetValue);
		SonikString& operator =(const double SetValue);

		//各入力フォーマットから現在のバッファに変換して結合します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikString& operator +=(const SonikString& t_his);
		SonikString& operator +=(const SonikStringWIDE& t_his);
		SonikString& operator +=(const SonikStringUTF8& t_his);
		SonikString& operator +=(const SonikStringUTF16& t_his);
		SonikString& operator +=(const char* Str);
		SonikString& operator +=(const utf16_t* w_Str);
		SonikString& operator +=(const wchar_t* w_Str);
		SonikString& operator +=(const utf8_t* utf8_Str);
		SonikString& operator +=(const int8_t SetValue);
		SonikString& operator +=(const uint8_t SetValue);
		SonikString& operator +=(const int16_t SetValue);
		SonikString& operator +=(const uint16_t SetValue);
		SonikString& operator +=(const int32_t SetValue);
		SonikString& operator +=(const uint32_t SetValue);
		SonikString& operator +=(const int64_t SetValue);
		SonikString& operator +=(const uint64_t SetValue);
		SonikString& operator +=(const float SetValue);
		SonikString& operator +=(const double SetValue);

		//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikString operator +(const SonikString& t_his);
		SonikString operator +(const SonikStringWIDE& t_his);
		SonikString operator +(const SonikStringUTF8& t_his);
		SonikString operator +(const SonikStringUTF16& t_his);
		SonikString operator +(const char* Str);
		SonikString operator +(const utf16_t* w_Str);
		SonikString operator +(const wchar_t* w_Str);
		SonikString operator +(const utf8_t* utf8_Str);
		SonikString operator +(const int8_t SetValue);
		SonikString operator +(const uint8_t SetValue);
		SonikString operator +(const int16_t SetValue);
		SonikString operator +(const uint16_t SetValue);
		SonikString operator +(const int32_t SetValue);
		SonikString operator +(const uint32_t SetValue);
		SonikString operator +(const int64_t SetValue);
		SonikString operator +(const uint64_t SetValue);
		SonikString operator +(const float SetValue);
		SonikString operator +(const double SetValue);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const SonikString& t_his) const;
		bool operator ==(const SonikStringWIDE& t_his) const;
		bool operator ==(const SonikStringUTF8& t_his) const;
		bool operator ==(const SonikStringUTF16& t_his) const;

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const SonikString& t_his) const;
		bool operator !=(const SonikStringWIDE& t_his) const;
		bool operator !=(const SonikStringUTF8& t_his) const;
		bool operator !=(const SonikStringUTF16& t_his) const;

		//小なり
		bool operator <(const SonikString& _Greater_) const;
		bool operator <(const SonikStringWIDE& _Greater_) const;
		bool operator <(const SonikStringUTF8& _Greater_) const;
		bool operator <(const SonikStringUTF16& _Greater_) const;

		//大なり
		bool operator >(const SonikString& _Less_) const;
		bool operator >(const SonikStringWIDE& _Less_) const;
		bool operator >(const SonikStringUTF8& _Less_) const;
		bool operator >(const SonikStringUTF16& _Less_) const;

		//小なりイコール
		bool operator <=(const SonikString& _GreaterEqual_) const;
		bool operator <=(const SonikStringWIDE& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF8& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF16& _GreaterEqual_) const;

		//大なりイコール
		bool operator >=(const SonikString& _LessEqual_) const;
		bool operator >=(const SonikStringWIDE& _LessEqual_) const;
		bool operator >=(const SonikStringUTF8& _LessEqual_) const;
		bool operator >=(const SonikStringUTF16& _LessEqual_) const;

	};
};


#endif /* __SONIKLIB_SONIKSTRING_H__ */