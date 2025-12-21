
#ifndef __SONIKLIB_SONIKSTRING_H__
#define __SONIKLIB_SONIKSTRING_H__

#include <SonikString/SonikStringBase.h>
#include <CPPGrammarDefines.h>

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
	class SonikStringDefault : public BASED_STRINGCLASS_SONIKLIB::SonikStringBase
	{
		friend class SonikStringWIDE;
		friend class SonikStringUTF8;
		friend class SonikStringUTF16;

	public:
		//コンストラクタ
		SonikStringDefault(void);
		SonikStringDefault(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const SonikStringDefault& t_his);
		SonikStringDefault(const SonikStringWIDE& t_his);
		SonikStringDefault(const SonikStringUTF8& t_his);
		SonikStringDefault(const SonikStringUTF16& t_his);
		SonikStringDefault(const char* SetStr);
		SonikStringDefault(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const utf16_t* SetStr);
		SonikStringDefault(const utf16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const wchar_t* SetStr);
		SonikStringDefault(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const utf8_t* SetStr);
		SonikStringDefault(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const int8_t SetValue);
		SonikStringDefault(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const uint8_t SetValue);
		SonikStringDefault(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const int16_t SetValue);
		SonikStringDefault(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const uint16_t SetValue);
		SonikStringDefault(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const int32_t SetValue);
		SonikStringDefault(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const uint32_t SetValue);
		SonikStringDefault(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const int64_t SetValue);
		SonikStringDefault(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const uint64_t SetValue);
		SonikStringDefault(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const float SetValue);
		SonikStringDefault(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringDefault(const double SetValue);
		SonikStringDefault(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//ムーヴコンストラクタ
		SonikStringDefault(SonikStringDefault&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;

		//デストラクタ
		~SonikStringDefault(void);

		//define切り替えのstrゲット
		const char* definition_str(void);
		//define切り替えのStrCopy
		uint64_t GetCpy_str_definition(char* dstBuffer);

		//各入力フォーマットから現在のバッファに変換して代入します。
		SonikStringDefault& operator =(const SonikStringDefault& t_his);
		SonikStringDefault& operator =(const SonikStringWIDE& t_his);
		SonikStringDefault& operator =(const SonikStringUTF8& t_his);
		SonikStringDefault& operator =(const SonikStringUTF16& t_his);
		SonikStringDefault& operator =(SonikStringDefault&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault& operator =(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault& operator =(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringDefault& operator =(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;

		SonikStringDefault& operator =(const char* Str);
		SonikStringDefault& operator =(const utf16_t* w_Str);
		SonikStringDefault& operator =(const wchar_t* w_Str);
		SonikStringDefault& operator =(const utf8_t* utf8_Str);
		SonikStringDefault& operator =(const int8_t SetValue);
		SonikStringDefault& operator =(const uint8_t SetValue);
		SonikStringDefault& operator =(const int16_t SetValue);
		SonikStringDefault& operator =(const uint16_t SetValue);
		SonikStringDefault& operator =(const int32_t SetValue);
		SonikStringDefault& operator =(const uint32_t SetValue);
		SonikStringDefault& operator =(const int64_t SetValue);
		SonikStringDefault& operator =(const uint64_t SetValue);
		SonikStringDefault& operator =(const float SetValue);
		SonikStringDefault& operator =(const double SetValue);

		//各入力フォーマットから現在のバッファに変換して結合します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringDefault& operator +=(const SonikStringDefault& t_his);
		SonikStringDefault& operator +=(const SonikStringWIDE& t_his);
		SonikStringDefault& operator +=(const SonikStringUTF8& t_his);
		SonikStringDefault& operator +=(const SonikStringUTF16& t_his);
		SonikStringDefault& operator +=(const char* Str);
		SonikStringDefault& operator +=(const utf16_t* w_Str);
		SonikStringDefault& operator +=(const wchar_t* w_Str);
		SonikStringDefault& operator +=(const utf8_t* utf8_Str);
		SonikStringDefault& operator +=(const int8_t SetValue);
		SonikStringDefault& operator +=(const uint8_t SetValue);
		SonikStringDefault& operator +=(const int16_t SetValue);
		SonikStringDefault& operator +=(const uint16_t SetValue);
		SonikStringDefault& operator +=(const int32_t SetValue);
		SonikStringDefault& operator +=(const uint32_t SetValue);
		SonikStringDefault& operator +=(const int64_t SetValue);
		SonikStringDefault& operator +=(const uint64_t SetValue);
		SonikStringDefault& operator +=(const float SetValue);
		SonikStringDefault& operator +=(const double SetValue);

		//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringDefault operator +(const SonikStringDefault& t_his);
		SonikStringDefault operator +(const SonikStringWIDE& t_his);
		SonikStringDefault operator +(const SonikStringUTF8& t_his);
		SonikStringDefault operator +(const SonikStringUTF16& t_his);
		SonikStringDefault operator +(const char* Str);
		SonikStringDefault operator +(const utf16_t* w_Str);
		SonikStringDefault operator +(const wchar_t* w_Str);
		SonikStringDefault operator +(const utf8_t* utf8_Str);
		SonikStringDefault operator +(const int8_t SetValue);
		SonikStringDefault operator +(const uint8_t SetValue);
		SonikStringDefault operator +(const int16_t SetValue);
		SonikStringDefault operator +(const uint16_t SetValue);
		SonikStringDefault operator +(const int32_t SetValue);
		SonikStringDefault operator +(const uint32_t SetValue);
		SonikStringDefault operator +(const int64_t SetValue);
		SonikStringDefault operator +(const uint64_t SetValue);
		SonikStringDefault operator +(const float SetValue);
		SonikStringDefault operator +(const double SetValue);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const SonikStringDefault& t_his) const;
		bool operator ==(const SonikStringWIDE& t_his) const;
		bool operator ==(const SonikStringUTF8& t_his) const;
		bool operator ==(const SonikStringUTF16& t_his) const;

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const SonikStringDefault& t_his) const;
		bool operator !=(const SonikStringWIDE& t_his) const;
		bool operator !=(const SonikStringUTF8& t_his) const;
		bool operator !=(const SonikStringUTF16& t_his) const;

		//小なり
		bool operator <(const SonikStringDefault& _Greater_) const;
		bool operator <(const SonikStringWIDE& _Greater_) const;
		bool operator <(const SonikStringUTF8& _Greater_) const;
		bool operator <(const SonikStringUTF16& _Greater_) const;

		//大なり
		bool operator >(const SonikStringDefault& _Less_) const;
		bool operator >(const SonikStringWIDE& _Less_) const;
		bool operator >(const SonikStringUTF8& _Less_) const;
		bool operator >(const SonikStringUTF16& _Less_) const;

		//小なりイコール
		bool operator <=(const SonikStringDefault& _GreaterEqual_) const;
		bool operator <=(const SonikStringWIDE& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF8& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF16& _GreaterEqual_) const;

		//大なりイコール
		bool operator >=(const SonikStringDefault& _LessEqual_) const;
		bool operator >=(const SonikStringWIDE& _LessEqual_) const;
		bool operator >=(const SonikStringUTF8& _LessEqual_) const;
		bool operator >=(const SonikStringUTF16& _LessEqual_) const;

	};
};


#endif /* __SONIKLIB_SONIKSTRING_H__ */