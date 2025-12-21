
#ifndef __SONIKLIB_SONIKSTRING_UTF8_H__
#define __SONIKLIB_SONIKSTRING_UTF8_H__

#include <SonikString/SonikString.h>

 //前方宣言===================================
namespace SonikLib
{
	namespace Container
	{
		template <class T>
		class SonikVariableArrayContainer;
	};

	class SonikStringDefault;
	class SonikStringWIDE;
	class SonikStringUTF16;
};
//=========================================


namespace SonikLib
{
	class SonikStringUTF8 : public BASED_STRINGCLASS_SONIKLIB::SonikStringBase
	{
		friend class SonikStringDefault;
		friend class SonikStringWIDE;
		friend class SonikStringUTF16;

	public:
		SonikStringUTF8(void);
		SonikStringUTF8(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const SonikStringUTF8& t_his);
		SonikStringUTF8(const SonikStringDefault& t_his);
		SonikStringUTF8(const SonikStringWIDE& t_his);
		SonikStringUTF8(const SonikStringUTF16& t_his);
		SonikStringUTF8(const char* SetStr);
		SonikStringUTF8(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const utf16_t* SetStr);
		SonikStringUTF8(const utf16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const wchar_t* SetStr);
		SonikStringUTF8(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const utf8_t* SetStr);
		SonikStringUTF8(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const int8_t SetValue);
		SonikStringUTF8(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const uint8_t SetValue);
		SonikStringUTF8(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const int16_t SetValue);
		SonikStringUTF8(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const uint16_t SetValue);
		SonikStringUTF8(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const int32_t SetValue);
		SonikStringUTF8(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const uint32_t SetValue);
		SonikStringUTF8(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const int64_t SetValue);
		SonikStringUTF8(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const uint64_t SetValue);
		SonikStringUTF8(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const float SetValue);
		SonikStringUTF8(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikStringUTF8(const double SetValue);
		SonikStringUTF8(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//ムーヴコンストラクタ
		SonikStringUTF8(SonikStringDefault&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;

		//デストラクタ
		~SonikStringUTF8(void);

		//define切り替えのstrゲット
		const utf8_t* definition_str(void);

		//define切り替えのStrCopy
		uint64_t GetCpy_str_definition(utf8_t* dstBuffer = nullptr);

		//各入力フォーマットから現在のバッファに変換して代入します。
		SonikStringUTF8& operator =(const SonikStringUTF8& t_his);
		SonikStringUTF8& operator =(const SonikStringDefault& t_his);
		SonikStringUTF8& operator =(const SonikStringWIDE& t_his);
		SonikStringUTF8& operator =(const SonikStringUTF16& t_his);
		SonikStringUTF8& operator =(SonikStringDefault&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8& operator =(SonikStringWIDE&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8& operator =(SonikStringUTF8&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8& operator =(SonikStringUTF16&& _move_) SLIB_CVR_NOEXCEPT;
		SonikStringUTF8& operator =(const char* Str);
		SonikStringUTF8& operator =(const utf16_t* w_Str);
		SonikStringUTF8& operator =(const wchar_t* w_Str);
		SonikStringUTF8& operator =(const utf8_t* utf8_Str);
		SonikStringUTF8& operator =(const int8_t SetValue);
		SonikStringUTF8& operator =(const uint8_t SetValue);
		SonikStringUTF8& operator =(const int16_t SetValue);
		SonikStringUTF8& operator =(const uint16_t SetValue);
		SonikStringUTF8& operator =(const int32_t SetValue);
		SonikStringUTF8& operator =(const uint32_t SetValue);
		SonikStringUTF8& operator =(const int64_t SetValue);
		SonikStringUTF8& operator =(const uint64_t SetValue);
		SonikStringUTF8& operator =(const float SetValue);
		SonikStringUTF8& operator =(const double SetValue);

		//各入力フォーマットから現在のバッファに変換して結合します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringUTF8& operator +=(const SonikStringUTF8& t_his);
		SonikStringUTF8& operator +=(const SonikStringDefault& t_his);
		SonikStringUTF8& operator +=(const SonikStringWIDE& t_his);
		SonikStringUTF8& operator +=(const SonikStringUTF16& t_his);
		SonikStringUTF8& operator +=(const char* Str);
		SonikStringUTF8& operator +=(const utf16_t* w_Str);
		SonikStringUTF8& operator +=(const wchar_t* w_Str);
		SonikStringUTF8& operator +=(const utf8_t* utf8_Str);
		SonikStringUTF8& operator +=(const int8_t SetValue);
		SonikStringUTF8& operator +=(const uint8_t SetValue);
		SonikStringUTF8& operator +=(const int16_t SetValue);
		SonikStringUTF8& operator +=(const uint16_t SetValue);
		SonikStringUTF8& operator +=(const int32_t SetValue);
		SonikStringUTF8& operator +=(const uint32_t SetValue);
		SonikStringUTF8& operator +=(const int64_t SetValue);
		SonikStringUTF8& operator +=(const uint64_t SetValue);
		SonikStringUTF8& operator +=(const float SetValue);
		SonikStringUTF8& operator +=(const double SetValue);

		//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
		//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikStringUTF8 operator +(const SonikStringUTF8& t_his);
		SonikStringUTF8 operator +(const SonikStringDefault& t_his);
		SonikStringUTF8 operator +(const SonikStringWIDE& t_his);
		SonikStringUTF8 operator +(const SonikStringUTF16& t_his);
		SonikStringUTF8 operator +(const char* Str);
		SonikStringUTF8 operator +(const utf16_t* w_Str);
		SonikStringUTF8 operator +(const wchar_t* w_Str);
		SonikStringUTF8 operator +(const utf8_t* utf8_Str);
		SonikStringUTF8 operator +(const int8_t SetValue);
		SonikStringUTF8 operator +(const uint8_t SetValue);
		SonikStringUTF8 operator +(const int16_t SetValue);
		SonikStringUTF8 operator +(const uint16_t SetValue);
		SonikStringUTF8 operator +(const int32_t SetValue);
		SonikStringUTF8 operator +(const uint32_t SetValue);
		SonikStringUTF8 operator +(const int64_t SetValue);
		SonikStringUTF8 operator +(const uint64_t SetValue);
		SonikStringUTF8 operator +(const float SetValue);
		SonikStringUTF8 operator +(const double SetValue);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const SonikStringUTF8& t_his) const;
		bool operator ==(const SonikStringDefault& t_his) const;
		bool operator ==(const SonikStringWIDE& t_his) const;
		bool operator ==(const SonikStringUTF16& t_his) const;

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const SonikStringUTF8& t_his) const;
		bool operator !=(const SonikStringDefault& t_his) const;
		bool operator !=(const SonikStringWIDE& t_his) const;
		bool operator !=(const SonikStringUTF16& t_his) const;

		//小なり
		bool operator <(const SonikStringUTF8& _Greater_) const;
		bool operator <(const SonikStringDefault& _Greater_) const;
		bool operator <(const SonikStringWIDE& _Greater_) const;
		bool operator <(const SonikStringUTF16& _Greater_) const;

		//大なり
		bool operator >(const SonikStringUTF8& _Less_) const;
		bool operator >(const SonikStringDefault& _Less_) const;
		bool operator >(const SonikStringWIDE& _Less_) const;
		bool operator >(const SonikStringUTF16& _Less_) const;

		//小なりイコール
		bool operator <=(const SonikStringUTF8& _GreaterEqual_) const;
		bool operator <=(const SonikStringDefault& _GreaterEqual_) const;
		bool operator <=(const SonikStringWIDE& _GreaterEqual_) const;
		bool operator <=(const SonikStringUTF16& _GreaterEqual_) const;

		//大なりイコール
		bool operator >=(const SonikStringUTF8& _LessEqual_) const;
		bool operator >=(const SonikStringDefault& _LessEqual_) const;
		bool operator >=(const SonikStringWIDE& _LessEqual_) const;
		bool operator >=(const SonikStringUTF16& _LessEqual_) const;

	};

};


#endif /* __SONIKLIB_SONIKSTRING_UTF8_H__ */