
#include <new>
#include <algorithm>
#include <cstdint>
#include <stdio.h>
#include <inttypes.h>
#include "../SonikCAS/SonikAtomicLock.h"
#include "./SonikStringConvert.hpp"
#include "SonikStringBase.h"
#include "SonikString.h"
#include "./WIDE/SonikStringWIDE.h"
#include "./UTF8/SonikStringUTF8.h"
#include "./UTF16/SonikStringUTF16.h"
#include "../CompilersPreProcesser.h"

namespace BASED_STRINGCLASS_SONIKLIB
{
	//UTF8, UTF16, SJISが扱えるstringクラスです。
	//c:処理自体はスレッドセーフですが、文字列をgetした後にバッファを書き換えられる恐れがあります。
	class  SonikStringBase::SonikString_pImpl
	{
	private:
		//c:文字列領域
		utf8_t* Stringval_;

		//c:格納している最大文字数
		uint64_t MaxLength_;

		//c:現在の文字列のタイプ
		SonikLibConvertType CType;
		//c:文字列領域のサイズ
		uint64_t buffer_;
		//c:マルチスレッドブロック
		SonikLib::S_CAS::SonikAtomicLock string_atm_lock;
		//c:マルチスレッドブロック（オペレータ)
		SonikLib::S_CAS::SonikAtomicLock string_operator_lock;

		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	private:
		//c:内部用リサイズ関数です。
		bool ReAlloc(uint64_t ReSize);

		//c:現在のバッファタイプを指定のバッファタイプに変更します。
		//c:変更の際に、バッファに対して変換作業が行われます。
		//c:変換の際に失敗した場合は、falseになり、タイプの切り替えと変換作業は行われません。
		//SCHTYPE_UNKNOWNを指定した場合は、必ずfalseとなります。
		bool SetCharacterType(SonikLibConvertType SetType);

	public:
		//c:コンストラクタ
		SonikString_pImpl(void);
		SonikString_pImpl(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//c:コンストラクタオーバーロード
		SonikString_pImpl(const char* SetStr);
		SonikString_pImpl(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//c:コンストラクタオーバーロード
		SonikString_pImpl(const char16_t* SetStr);
		SonikString_pImpl(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//c:コンストラクタオーバーロード
		SonikString_pImpl(const wchar_t* SetStr);
		SonikString_pImpl(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//コンストラクタオーバーロード
		SonikString_pImpl(const utf8_t* SetStr);
		SonikString_pImpl(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//コンストラクタオーバーロード
		SonikString_pImpl(const int8_t SetValue);
		SonikString_pImpl(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const uint8_t SetValue);
		SonikString_pImpl(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const int16_t SetValue);
		SonikString_pImpl(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const uint16_t SetValue);
		SonikString_pImpl(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const int32_t SetValue);
		SonikString_pImpl(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const uint32_t SetValue);
		SonikString_pImpl(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const int64_t SetValue);
		SonikString_pImpl(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const uint64_t SetValue);
		SonikString_pImpl(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const float SetValue);
		SonikString_pImpl(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		SonikString_pImpl(const double SetValue);
		SonikString_pImpl(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

		//c:コピーコンストラクタ
		SonikString_pImpl(const SonikString_pImpl& t_his);

		//c:デストラクタ
		~SonikString_pImpl(void);

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

		//c: 文字列中の全角英数字を半角英数字に変換します。
		bool ConvertFWANtoHWAN(void);
		//c: 文字列中の全角カナを半角カナに変換します。
		bool ConvertFWKNtoHWKN(void);

		//c: 指定位置の文字を削除します。
		void EraseChar(uint64_t ChrPoint);
		//c: 指定の開始位置から指定された文字数を削除します。
		void EraseStr(uint64_t ChrStartPoint, uint64_t ChrEndPoint);

		//c: 指定したAsciiコードをデリミタとしてSplitを行います。
		bool Split_Ascii(const char* delim, char*& _outbuffer_, uint64_t& _out_splitcnt_, uint64_t& _out_buffersize_);

		//c: 各入力フォーマットから現在のバッファに変換して代入します。
		SonikString_pImpl& operator =(const SonikString_pImpl& t_his);
		SonikString_pImpl& operator =(const char* Str);
		SonikString_pImpl& operator =(const char16_t* w_Str);
		SonikString_pImpl& operator =(const wchar_t* w_Str);
		SonikString_pImpl& operator =(const utf8_t* utf8_Str);
		SonikString_pImpl& operator =(const int8_t SetValue);
		SonikString_pImpl& operator =(const uint8_t SetValue);
		SonikString_pImpl& operator =(const int16_t SetValue);
		SonikString_pImpl& operator =(const uint16_t SetValue);
		SonikString_pImpl& operator =(const int32_t SetValue);
		SonikString_pImpl& operator =(const uint32_t SetValue);
		SonikString_pImpl& operator =(const int64_t SetValue);
		SonikString_pImpl& operator =(const uint64_t SetValue);
		SonikString_pImpl& operator =(const float SetValue);
		SonikString_pImpl& operator =(const double SetValue);

		//c: 各入力フォーマットから現在のバッファに変換して結合します。
		//c: コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikString_pImpl& operator +=(const SonikString_pImpl& t_his);
		SonikString_pImpl& operator +=(const char* Str);
		SonikString_pImpl& operator +=(const char16_t* w_Str);
		SonikString_pImpl& operator +=(const wchar_t* w_Str);
		SonikString_pImpl& operator +=(const utf8_t* utf8_Str);
		SonikString_pImpl& operator +=(const int8_t SetValue);
		SonikString_pImpl& operator +=(const uint8_t SetValue);
		SonikString_pImpl& operator +=(const int16_t SetValue);
		SonikString_pImpl& operator +=(const uint16_t SetValue);
		SonikString_pImpl& operator +=(const int32_t SetValue);
		SonikString_pImpl& operator +=(const uint32_t SetValue);
		SonikString_pImpl& operator +=(const int64_t SetValue);
		SonikString_pImpl& operator +=(const uint64_t SetValue);
		SonikString_pImpl& operator +=(const float SetValue);
		SonikString_pImpl& operator +=(const double SetValue);

		//c: 現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
		//c: コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
		SonikString_pImpl operator +(const SonikString_pImpl& t_his);
		SonikString_pImpl operator +(const char* Str);
		SonikString_pImpl operator +(const char16_t* w_Str);
		SonikString_pImpl operator +(const wchar_t* w_Str);
		SonikString_pImpl operator +(const utf8_t* utf8_Str);
		SonikString_pImpl operator +(const int8_t SetValue);
		SonikString_pImpl operator +(const uint8_t SetValue);
		SonikString_pImpl operator +(const int16_t SetValue);
		SonikString_pImpl operator +(const uint16_t SetValue);
		SonikString_pImpl operator +(const int32_t SetValue);
		SonikString_pImpl operator +(const uint32_t SetValue);
		SonikString_pImpl operator +(const int64_t SetValue);
		SonikString_pImpl operator +(const uint64_t SetValue);
		SonikString_pImpl operator +(const float SetValue);
		SonikString_pImpl operator +(const double SetValue);

		//c: 文字列同士を比較します。(strcmp)
		//c: 一致の場合true 不一致の場合 falseを返却します。
		bool operator ==(const SonikString_pImpl& t_his);
		bool operator ==(const char* Str);
		bool operator ==(const char16_t* w_Str);
		bool operator ==(const wchar_t* w_Str);
		bool operator ==(const utf8_t* utf8_Str);

		//c: 文字列同士を比較します。(strcmp)
		//c: 不一致の場合true　一致の場合 falseを返却します。
		bool operator !=(const SonikString_pImpl& t_his);
		bool operator !=(const char* Str);
		bool operator !=(const char16_t* w_Str);
		bool operator !=(const wchar_t* w_Str);
		bool operator !=(const utf8_t* utf8_Str);

		//c:比較演算子
		bool Greater(const char* CompareArg2val);
		bool Less(const char* CompareArg2val);
		bool GreaterEqual(const char* CompareArg2val);
		bool LessEqual(const char* CompareArg2val);

	};


	SonikStringBase::SonikString_pImpl::SonikString_pImpl(void)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		
		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if(l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const char* SetStr)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		//NULL文字文追加(+1)して格納
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(SetStr) + 1;

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, SrcByte, Stringval_);
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		//NULL文字文追加(+1)して格納
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(SetStr) + 1;

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, SrcByte, Stringval_);
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const char16_t* SetStr)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UTF16)
	,buffer_(100)
	{
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(SetStr) + 2; //Null文字分 shortサイズ分追加。

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			SrcByte >>= 1; //要素数に変換
			std::copy_n(SetStr, SrcByte, reinterpret_cast<char16_t*>(Stringval_));
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char16_t*>(Stringval_));
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UTF16)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(SetStr) + 2; //Null文字分 shortサイズ分追加。

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			SrcByte >>= 1; //要素数に変換
			std::copy_n(SetStr, SrcByte, reinterpret_cast<char16_t*>(Stringval_));
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char16_t*>(Stringval_));
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const wchar_t* SetStr)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UNKNOWN)
	,buffer_(100)
	{

#if WCHAR_MAX <= 0xFFFFU
		uint32_t NULLSTR_SIZE = 2;
		using CONST_WCRTYPE = const char16_t*;
		using WCRTYPE = char16_t*;
		CType = SCHTYPE_UTF16;
#else
		uint32_t NULLSTR_SIZE = 4;
		using CONST_WCRTYPE = const char32_t*;
		using WCRTYPE = char32_t*;
		CType = SCHTYPE_UTF32;
#endif

		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_WCRTYPE>(SetStr)) + NULLSTR_SIZE; //Null文字サイズ分追加。

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};


		try
		{
#if defined(_S_STRING_COPY_STL)
#if WCHAR_MAX <= 0xFFFFU
			SrcByte >>= 1;
#else
			SrcByte >>= 2;
#endif

			std::copy_n(SetStr, SrcByte, reinterpret_cast<wchar_t*>(Stringval_));
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<WCRTYPE>(Stringval_));
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UNKNOWN)
	,buffer_(100)
	,m_allocator(_allocator_)
	{

#if WCHAR_MAX <= 0xFFFFU
		uint32_t NULLSTR_SIZE = 2;
		using CONST_WCRTYPE = const char16_t*;
		using WCRTYPE = char16_t*;
		CType = SCHTYPE_UTF16;
#else
		uint32_t NULLSTR_SIZE = 4;
		using CONST_WCRTYPE = const char32_t*;
		using WCRTYPE = char32_t*;
		CType = SCHTYPE_UTF32;
#endif

		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_WCRTYPE>(SetStr)) + NULLSTR_SIZE; //Null文字サイズ分追加。

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};


		try
		{
#if defined(_S_STRING_COPY_STL)
#if WCHAR_MAX <= 0xFFFFU
			SrcByte >>= 1;
#else
			SrcByte >>= 2;
#endif

			std::copy_n(SetStr, SrcByte, reinterpret_cast<wchar_t*>(Stringval_));
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<WCRTYPE>(Stringval_));
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const utf8_t* SetStr)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UTF16)
	,buffer_(100)
	{
		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(SetStr));

		//NULL文字文追加(+1)して格納
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(SetStr)) + 1;

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)

			std::copy_n(SetStr, SrcByte, Stringval_);
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_UTF16)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(SetStr));

		//NULL文字文追加(+1)して格納
		uint64_t SrcByte = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(SetStr)) + 1;

		if (SrcByte > buffer_)
		{
			buffer_ = SrcByte;
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)

			std::copy_n(SetStr, SrcByte, Stringval_);
#else
			std::memcpy(Stringval_, SetStr, SrcByte);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int8_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[5]{};
		snprintf(SetStr, sizeof(SetStr), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[5]{};
		snprintf(SetStr, sizeof(SetStr), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint8_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[4]{};
		snprintf(SetStr, sizeof(SetStr), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[4]{};
		snprintf(SetStr, sizeof(SetStr), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int16_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[7]{};
		snprintf(SetStr, sizeof(SetStr), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[7]{};
		snprintf(SetStr, sizeof(SetStr), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint16_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[6]{};
		snprintf(SetStr, sizeof(SetStr), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[6]{};
		snprintf(SetStr, sizeof(SetStr), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int32_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[12]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRId32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[12]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRId32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint32_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[11]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRIu32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[11]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRIu32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int64_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[21]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRId64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[21]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRId64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint64_t SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[20]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRIu64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[20]{};
		snprintf(SetStr, sizeof(SetStr), "%" PRIu64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const float SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[47]{};
		snprintf(SetStr, sizeof(SetStr), "%g", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[47]{};
		snprintf(SetStr, sizeof(SetStr), "%g", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const double SetValue)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	{
		char SetStr[94]{};
		snprintf(SetStr, sizeof(SetStr), "%lg", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		SonikLib::SLibAllocateInterface* l_defalloc = nullptr;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface();
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			l_defalloc->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};
	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	:Stringval_(nullptr)
	,MaxLength_(0)
	,CType(SCHTYPE_NULLTEXT)
	,buffer_(100)
	,m_allocator(_allocator_)
	{
		char SetStr[94]{};
		snprintf(SetStr, sizeof(SetStr), "%lg", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(SetStr);

		if (sizeof(SetStr) > buffer_)
		{
			buffer_ = sizeof(SetStr);
		};

		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(SetStr, sizeof(SetStr), Stringval_);
#else
			std::memcpy(Stringval_, SetStr, sizeof(SetStr));
#endif
		}
		catch (...)
		{
			_allocator_->memdel(Stringval_);
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		CType = tmpType;
	};

	SonikStringBase::SonikString_pImpl::SonikString_pImpl(const SonikString_pImpl& t_his)
	:Stringval_(nullptr)
	,MaxLength_(t_his.MaxLength_)
	,CType(t_his.CType)
	,buffer_(t_his.buffer_)
	,m_allocator(t_his.m_allocator)
	{
		try
		{

			void* l_allocbuffer = m_allocator->memal(sizeof(utf8_t) * t_his.buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			Stringval_ = new(l_allocbuffer) utf8_t[t_his.buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(t_his.Stringval_, buffer_, Stringval_);
#else
			std::memcpy(Stringval_, t_his.Stringval_, buffer_);
#endif

		}
		catch (...)
		{
			m_allocator->memdel(Stringval_);
			throw;
		};

	};

	SonikStringBase::SonikString_pImpl::~SonikString_pImpl(void)
	{
		if (Stringval_ != 0)
		{
			m_allocator->memdel(Stringval_);
			Stringval_ = nullptr;
		};
	};

	//現在のバッファタイプを指定のバッファタイプに変更します。
	//変更の際に、バッファに対して変換作業が行われます。
	//変換の際に失敗した場合は、falseになり、タイプの切り替えと変換作業は行われません。
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::SetCharacterType(SonikLibConvertType SetType)
	{
		string_atm_lock.lock();

		uint64_t convSize_ = 0;
		//		unsigned long bufsize_ = SonikLibStringConvert::GetStringLength(reinterpret_cast<char*>(Stringval_));
		switch (CType)
		{
		case SCHTYPE_SJIS:
			//SJIS から SetType へ各変換
			switch (SetType)
			{
			case SCHTYPE_SJIS:
				//SJISへの変換は行わない。行う必要がない
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF16:
				//UTF16への変換
				SonikLibStringConvert::ConvertMBStoUTF16(reinterpret_cast<char*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertMBStoUTF16(reinterpret_cast<char*>(Stringval_), reinterpret_cast<char16_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;
				break;

			case SCHTYPE_UTF8:
				//SJISからUTF8への変換を行う。
				SonikLibStringConvert::ConvertMBSToUTF8(reinterpret_cast<char*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertMBSToUTF8(reinterpret_cast<char*>(Stringval_), reinterpret_cast<utf8_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;
				break;

			case SCHTYPE_UTF32:
				//SJIS -> UTF32 への変換
				SonikLibStringConvert::ConvertMBStoUTF32(reinterpret_cast<char*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertMBStoUTF32(reinterpret_cast<char*>(Stringval_), reinterpret_cast<char32_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_NULLTEXT:

				CType = SetType;
				string_atm_lock.unlock();
				break;

			default:
				//SCHTYPE_UNKNOWN判定
				break;
			};

			break;

		case SCHTYPE_UTF16:
			//UTF16  から SetType へ各変換
			switch (SetType)
			{
			case SCHTYPE_SJIS:
				//UTF16からSJISへの変換
				SonikLibStringConvert::ConvertUTF16toMBS(reinterpret_cast<char16_t*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				//convSize_ = buffer_ << 1; // x * 2 = x << 1
				if (!SonikLibStringConvert::ConvertUTF16toMBS(reinterpret_cast<char16_t*>(Stringval_), reinterpret_cast<char*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF16:
				//UTF16 から UTF16への変換は行わない。行う必要がない。

				string_atm_lock.unlock();
				return true;
				break;

			case SCHTYPE_UTF8:
				//UTF16 から UTF8への変換
				SonikLibStringConvert::ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(Stringval_), reinterpret_cast<utf8_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF32:
				//UTF16 -> UTF32 への変換
				SonikLibStringConvert::ConvertUTF16ToUTF32(reinterpret_cast<char16_t*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF16ToUTF32(reinterpret_cast<char16_t*>(Stringval_), reinterpret_cast<char32_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_NULLTEXT:

				CType = SetType;
				string_atm_lock.unlock();
				break;

			default:
				//SCHTYPE_UNKNOWN判定

				string_atm_lock.unlock();
				break;
			};

			break;

		case SCHTYPE_UTF8:

			switch (SetType)
			{
			case SCHTYPE_SJIS:
				//UTF8からSJISへの変換
				SonikLibStringConvert::ConvertUTF8ToMBS(reinterpret_cast<utf8_t*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF8ToMBS(reinterpret_cast<utf8_t*>(Stringval_), reinterpret_cast<char*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF16:
				//UTF8からUTF16へ変換
				SonikLibStringConvert::ConvertUTF8ToUTF16(reinterpret_cast<utf8_t*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF8ToUTF16(reinterpret_cast<utf8_t*>(Stringval_), reinterpret_cast<char16_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF8:
				//UTF8からUTF8へ変換は行わない。行う必要がない。

				string_atm_lock.unlock();
				return true;
				break;

			case SCHTYPE_UTF32:
				//UTF8 -> UTF32 への変換
				SonikLibStringConvert::ConvertUTF8ToUTF32(reinterpret_cast<utf8_t*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF8ToUTF32(reinterpret_cast<utf8_t*>(Stringval_), reinterpret_cast<char32_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_NULLTEXT:

				CType = SetType;
				string_atm_lock.unlock();
				break;

			default:
				//SCHTYPE_UNKNOWN判定

				break;
			};

			break;

		case SCHTYPE_UTF32: //UTF32 からの各変換
			switch (SetType)
			{
			case SCHTYPE_SJIS:
				//UTF32 -> SJIS
				SonikLibStringConvert::ConvertUTF32toMBS(reinterpret_cast<char32_t*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF32toMBS(reinterpret_cast<char32_t*>(Stringval_), reinterpret_cast<char*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF16:
				//UTF32 -> UTF16
				SonikLibStringConvert::ConvertUTF32ToUTF16(reinterpret_cast<char32_t*>(Stringval_), nullptr, &convSize_, m_allocator);
				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF32ToUTF16(reinterpret_cast<char32_t*>(Stringval_), reinterpret_cast<char16_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF8:
				//UTF8からUTF32へ変換
				SonikLibStringConvert::ConvertUTF8ToUTF32(reinterpret_cast<utf8_t*>(Stringval_), nullptr, &convSize_, m_allocator);

				if (convSize_ > buffer_)
				{
					if (!this->ReAlloc(convSize_))
					{
						string_atm_lock.unlock();
						return false;
					};

				};

				if (!SonikLibStringConvert::ConvertUTF8ToUTF32(reinterpret_cast<utf8_t*>(Stringval_), reinterpret_cast<char32_t*>(Stringval_), nullptr, m_allocator))
				{
					string_atm_lock.unlock();
					return false;
				};

				CType = SetType;
				string_atm_lock.unlock();
				return true;

				break;

			case SCHTYPE_UTF32:
				//UTF32 -> UTF32 は何もしない。
				string_atm_lock.unlock();
				return true;
				break;

			case SCHTYPE_NULLTEXT:
				CType = SetType;
				string_atm_lock.unlock();
				break;

			default:
				//SCHTYPE_UNKNOWN判定

				break;
			}

			break;

		case SCHTYPE_NULLTEXT:

			CType = SetType;
			string_atm_lock.unlock();
			break;

		default:
			//SCHTYPE_UNKNOWN判定

			break;
		};

		string_atm_lock.unlock();
		return false;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::ReAlloc(uint64_t ReArraySize)
	{
		utf8_t* pTmp = 0;

		void* l_allocbuffer = m_allocator->memal(sizeof(utf8_t) * ReArraySize);
		if(l_allocbuffer == nullptr)
		{
			return false;
		};

		pTmp = new(l_allocbuffer) utf8_t[ReArraySize]{};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Stringval_, buffer_, pTmp);
#else
			std::memcpy(pTmp, Stringval_, buffer_);
#endif

		}
		catch (...)
		{
			m_allocator->memdel(pTmp);
			return false;
		};

		m_allocator->memdel(Stringval_);
		Stringval_ = pTmp;
		buffer_ = ReArraySize;

		return true;
	};

	DEF_FORCE_INLINE const char* SonikStringBase::SonikString_pImpl::str_c(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return "";
		};

		if (!this->SetCharacterType(SCHTYPE_SJIS))
		{
			return "";
		};

		return reinterpret_cast<char*>(Stringval_);

	};

	//wchar_t形式に変換して取得します。(バッファタイプも書き換わります。)
	DEF_FORCE_INLINE const wchar_t* SonikStringBase::SonikString_pImpl::str_wchar(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return L"";
		};

#if WCHAR_MAX <= 0xFFFFU
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;
#else
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		if (!this->SetCharacterType(CONVTYPE))
		{
			return L"";
		};

		return reinterpret_cast<wchar_t*>(Stringval_);
	};

	DEF_FORCE_INLINE const char16_t* SonikStringBase::SonikString_pImpl::str_utf16(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return u"";
		};

		if (!this->SetCharacterType(SCHTYPE_UTF16))
		{
			return u"";
		};

		return reinterpret_cast<char16_t*>(Stringval_);
	};

	DEF_FORCE_INLINE const utf8_t* SonikStringBase::SonikString_pImpl::str_utf8(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return reinterpret_cast<const utf8_t*>("");
		};

		if (!this->SetCharacterType(SCHTYPE_UTF8))
		{
			return reinterpret_cast<const utf8_t*>("");
		};

		return Stringval_;
	};

	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::GetCpy_str_c(char* dstBuffer)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return 0;
		};

		if (CType != SCHTYPE_SJIS)
		{
			//UNKNOWN以外　かつ SJIS以外ならSJISへ変換してカウントチェック
			if (!this->SetCharacterType(SCHTYPE_SJIS))
			{
				return 0;
			};
		};

		//NULL文字分追加 (+1)して格納
		uint64_t cpysize = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_)) + 1;

		if (dstBuffer == nullptr)
		{
			//サイズ返却して終了
			return cpysize;
		};

		//dstコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(reinterpret_cast<char*>(Stringval_), cpysize, dstBuffer);
#else
			std::memcpy(dstBuffer, Stringval_, cpysize);
#endif

		}
		catch (...)
		{
			return 0;
		};

		return cpysize;
	};

	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::GetCpy_str_wcstr(wchar_t* dstBuffer)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return 0;
		};

#if WCHAR_MAX <= 0xFFFFU
		uint32_t NULLSTR_SIZE = 2;
		//using CONST_CHRTYPE = const char16_t*;
		using CHRTYPE = char16_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;

#else
		uint32_t NULLSTR_SIZE = 4;
		//using CONST_CHRTYPE = const char32_t*;
		using CHRTYPE = char32_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		if (CType != CONVTYPE)
		{
			//UNKNOWN以外　かつ UTF16以外ならUTF16へ変換してカウントチェック
			if (!this->SetCharacterType(CONVTYPE))
			{
				return 0;
			};
		};

		//NULL文字分追加(wide = +2) して格納
		uint64_t cpysize = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CHRTYPE>(Stringval_)) + NULLSTR_SIZE;

		if (dstBuffer == nullptr)
		{
			//サイズ返却して終了
			return cpysize;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
	#if WCHAR_MAX <= 0xFFFFU
			cpysize >>= 1;
	#else
			cpysize >>= 2;
	#endif
			
			std::copy_n(reinterpret_cast<CHRTYPE>(Stringval_), cpysize, dstBuffer);
#else
			std::memcpy(dstBuffer, Stringval_, cpysize);
#endif

		}
		catch (...)
		{
			return 0;
		};

		return cpysize;
	};

	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::GetCpy_str_utf16(char16_t* dstBuffer)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return 0;
		};

		if (CType != SCHTYPE_UTF16)
		{
			//UNKNOWN以外　かつ UTF16以外ならUTF16へ変換してカウントチェック
			if (!this->SetCharacterType(SCHTYPE_UTF16))
			{
				return 0;
			};
		};

		//NULL文字分追加(wide = +2) して格納
		uint64_t cpysize = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_)) + 2;

		if (dstBuffer == nullptr)
		{
			//サイズ返却して終了
			return cpysize;
		};

		//dstコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			cpysize >>= 1;
			std::copy_n(reinterpret_cast<char16_t*>(Stringval_), cpysize, dstBuffer);
#else
			std::memcpy(dstBuffer, Stringval_, cpysize);
#endif

		}
		catch (...)
		{
			return 0;
		};

		return cpysize;
	};

	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::GetCpy_str_utf8(utf8_t* dstBuffer)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return 0;
		};

		if (CType != SCHTYPE_UTF8)
		{
			//UNKNOWN以外　かつ UTF8以外ならUTF8へ変換してカウントチェック
			if (!this->SetCharacterType(SCHTYPE_UTF8))
			{
				return 0;
			};
		};

		//NULL文字分追加 (+1)して格納
		uint64_t cpysize = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_)) + 1;

		if (dstBuffer == nullptr)
		{
			//サイズ返却して終了
			return cpysize;
		};

		//dstコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Stringval_, cpysize, dstBuffer);
#else
			std::memcpy(dstBuffer, Stringval_, cpysize);
#endif

		}
		catch (...)
		{
			return 0;
		};

		return cpysize;
	};

	//c:文字列のByte数を取得します。（Null終端文字をカウントに含まない)
	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::Count_Byte_NotNull(void)
	{
		return SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
	};

	//c:文字列数を取得します。（Null終端文字をカウントに含まない)
	DEF_FORCE_INLINE uint64_t SonikStringBase::SonikString_pImpl::Count_Str_NotNull(void)
	{
		return MaxLength_;
	};

	//文字列中の全角英数字を半角英数字に変換します。
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::ConvertFWANtoHWAN(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return false;
		};

		//全角半角変換はUTF-8しか作ってないのでUTF-8に一度変換する。
		if (!this->SetCharacterType(SCHTYPE_UTF8))
		{
			return false;
		};

		string_atm_lock.lock();
		//全角半角変換
		uint64_t Size = 0;
		//サイズ取得
		SonikLibStringConvert::ConvertUTF8FWCToHWCForAN(reinterpret_cast<char*>(Stringval_), nullptr, Size, m_allocator);
		//領域取得
		if (Size & 0x01)
		{
			++Size;
		};

		void* l_allocmemory = m_allocator->memal(sizeof(int8_t) * Size);
		if( l_allocmemory == nullptr )
		{
			string_atm_lock.unlock();
			return false;
		};

		int8_t* tmp_Str = new(l_allocmemory) int8_t[Size]{};

		//本番
		if (!SonikLibStringConvert::ConvertUTF8FWCToHWCForAN(reinterpret_cast<char*>(Stringval_), reinterpret_cast<char*>(tmp_Str), Size, m_allocator))
		{
			string_atm_lock.unlock();
			return false;
		};

		delete Stringval_;
		Stringval_ = reinterpret_cast<utf8_t*>(tmp_Str);

		if (buffer_ < Size)
		{
			buffer_ = Size;
		};

		string_atm_lock.unlock();
		return true;
	};

	//文字列中の全角カナを半角カナに変換します。
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::ConvertFWKNtoHWKN(void)
	{
		if (CType == SCHTYPE_UNKNOWN)
		{
			return false;
		};

		//全角半角変換はUTF-8しか作ってないのでUTF-8に一度変換する。
		if (!this->SetCharacterType(SCHTYPE_UTF8))
		{
			return false;
		};

		string_atm_lock.lock();
		//全角半角変換
		uint64_t Size = 0;
		//サイズ取得
		SonikLibStringConvert::ConvertUTF8FWCToHWCForKANA(reinterpret_cast<char*>(Stringval_), nullptr, Size, m_allocator);
		//領域取得
		if (Size & 0x01)
		{
			++Size;
		};

		void* l_allocmemory = m_allocator->memal(sizeof(int8_t) * Size);
		if (l_allocmemory == nullptr)
		{
			string_atm_lock.unlock();
			return false;
		};

		int8_t* tmp_Str = new(l_allocmemory) int8_t[Size]{};

		//本番
		if (!SonikLibStringConvert::ConvertUTF8FWCToHWCForKANA(reinterpret_cast<char*>(Stringval_), reinterpret_cast<char*>(tmp_Str), Size, m_allocator))
		{
			string_atm_lock.unlock();
			return false;
		};

		delete Stringval_;
		Stringval_ = reinterpret_cast<utf8_t*>(tmp_Str);

		if (buffer_ < Size)
		{
			buffer_ = Size;
		};

		string_atm_lock.unlock();

		return true;
	};

	//c: 指定位置の文字を削除します。
	DEF_FORCE_INLINE void SonikStringBase::SonikString_pImpl::EraseChar(uint64_t ChrPoint)
	{
		this->str_utf8();

		if (SonikLibStringControl::StringPointEraser(reinterpret_cast<char*>(Stringval_), ChrPoint, 1, MaxLength_))
		{
			MaxLength_ -= 1;
		};

	};

	//c: 指定の開始位置から指定された文字数を削除します。
	DEF_FORCE_INLINE void SonikStringBase::SonikString_pImpl::EraseStr(uint64_t ChrStartPoint, uint64_t ChrEndPoint)
	{
		this->str_utf8();

		if (SonikLibStringControl::StringPointEraser(reinterpret_cast<char*>(Stringval_), ChrStartPoint, ChrEndPoint, MaxLength_))
		{
			MaxLength_ -= ChrEndPoint;
		};
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::Split_Ascii(const char* delim, char*& _outbuffer_, uint64_t& _out_splitcnt_, uint64_t& _out_buffersize_)
	{
		this->str_wchar();
		SonikLib::SonikString delimconvert = delim;
		const wchar_t* l_delim = delimconvert.str_wchar();

		wchar_t* l_start = reinterpret_cast<wchar_t*>(Stringval_);
		wchar_t* l_end = nullptr;

		//確保バッファ数算出のため、１回wcsstrで最後までdelim検索をかける
		uint64_t l_splitcnt = 0;
		while ((l_end = wcsstr(l_start, l_delim)) != nullptr)
		{
			++l_splitcnt;
			l_start = (++l_end);
		};

		if (l_splitcnt == 0)
		{
			//１個も見つからなければ終了
			return false;
		};

		//splitの数分、null文字と先頭メタ領域にアドレスが必要なのでその分のバッファを確保(split数が0より上なら文字列先頭も必要なのでカウント数 + 1個必要なので加算しておく。またnull文字も必要なので最後に+1)
		// sizeof(wchar_t) * MaxLength_ + (l_splitcnt + 1) = 文字数分のサイズ + スプリット数分のnull(\0)文字数。
		// sizeof(uintptr_t) * (l_splitcnt +1) = 先頭メタアドレスタグ領域のサイズ
		++l_splitcnt;
		uint64_t l_bufsize = (sizeof(wchar_t) * (MaxLength_ + 1)) + (sizeof(uintptr_t) * l_splitcnt);

		if(_outbuffer_ == nullptr)
		{
			//必要バッファサイズを返却
			_out_buffersize_ = l_bufsize - (sizeof(uintptr_t) * l_splitcnt);
			return true;
		};

		//書き込み位置設定
		uintptr_t* l_meta_write = reinterpret_cast<uintptr_t*>(_outbuffer_);
		l_start = reinterpret_cast<wchar_t*>(l_meta_write + l_splitcnt);
		l_end = nullptr;

		//まず文字列コピー
		std::memcpy(l_start, Stringval_, MaxLength_ * sizeof(wchar_t));

		//先頭ポインタを設定
		(*l_meta_write) = reinterpret_cast<uintptr_t>(l_start);
		++l_meta_write;

		//Split本番
		while ((l_end = wcsstr(l_start, l_delim)) != nullptr)
		{
			//スプリット対象の文字をnull文字に変える。
			(*l_end) = 0x00;
			l_start = (++l_end);

			//アドレス記述領域に地点を記載
			(*l_meta_write) = reinterpret_cast<uintptr_t>(l_start);
			++l_meta_write;
		};

		_out_splitcnt_ = l_splitcnt;
		_out_buffersize_ = l_bufsize - (sizeof(uintptr_t) * l_splitcnt);

		return true;
	};


	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const SonikString_pImpl& t_his)
	{
		if (this == &t_his)
		{
			return (*this);
		};

		utf8_t* tmpbuffer = nullptr;
		char* tmp_localebuffer = nullptr;

		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_alloc = t_his.m_allocator;
		try
		{
			void* l_allocbuffer = l_alloc->memal(sizeof(utf8_t) * t_his.buffer_);
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			tmpbuffer = new(l_allocbuffer) utf8_t[t_his.buffer_]{};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

		if (Stringval_ != nullptr)
		{
			m_allocator->memdel(Stringval_);
		};

		Stringval_ = tmpbuffer;

		buffer_ = t_his.buffer_;
		CType = t_his.CType;
		MaxLength_ = t_his.MaxLength_;
		m_allocator = l_alloc;

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(t_his.Stringval_, buffer_, reinterpret_cast<utf8_t*>(tmpbuffer));
#else
			std::memcpy(tmpbuffer, t_his.Stringval_, buffer_);
#endif

		}
		catch (...)
		{
			l_alloc->memdel(Stringval_);
			throw;
		};

		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const char* Str)
	{
		if (Str == 0)
		{
			return (*this);
		};

		string_operator_lock.lock();

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc((Size_ + 1)))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);


		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const char16_t* w_Str)
	{
		if (w_Str == 0)
		{
			return (*this);
		};

		string_operator_lock.lock();

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(w_Str);
		if (buffer_ < (Size_ + 2))
		{
			//null終端分追加
			if (!this->ReAlloc((Size_ + 2)))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if ((*w_Str) == 0x00)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SCHTYPE_UTF16;

		try
		{
#if defined(_S_STRING_COPY_STL)
			Size_ >>= 1;
			std::copy_n(w_Str, Size_, reinterpret_cast<char16_t*>(Stringval_));
#else
			std::memcpy(Stringval_, w_Str, Size_);
#endif

		}
		catch (...)
		{
			throw;
		};


		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char16_t*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const wchar_t* w_Str)
	{
		if (w_Str == 0)
		{
			return (*this);
		};

		string_operator_lock.lock();

#if WCHAR_MAX <= 0xFFFFU
		uint32_t NULLSTR_SIZE = 2;
		using CONST_CHRTYPE = const char16_t*;
		using CHRTYPE = char16_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;

#else
		uint32_t NULLSTR_SIZE = 4;
		using CONST_CHRTYPE = const char32_t*;
		using CHRTYPE = char32_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_CHRTYPE>(w_Str));

		try
		{
			if (buffer_ < (Size_ + NULLSTR_SIZE))
			{
				//null終端分追加
				if (!this->ReAlloc((Size_ + NULLSTR_SIZE)))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};

		}catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if ((*w_Str) == 0x00)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = CONVTYPE;

		try
		{
#if defined(_S_STRING_COPY_STL)
	#if WCHAR_MAX <= 0xFFFFU
			Size_ >>= 1;
	#else
			Size_ >>= 2;
	#endif
			std::copy_n(w_Str, Size_, reinterpret_cast<wchar_t*>(Stringval_));
#else
			std::memcpy(Stringval_, w_Str, Size_);
#endif

		}
		catch (...)
		{
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<CHRTYPE>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const utf8_t* utf8_Str)
	{
		if (utf8_Str == nullptr)
		{
			return (*this);
		};

		string_operator_lock.lock();

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(utf8_Str));

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc((Size_ + 1)))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(reinterpret_cast<const char*>(utf8_Str), "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(utf8_Str));

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf8_Str, Size_, Stringval_);
#else
			std::memcpy(Stringval_, utf8_Str, Size_);
#endif

		}
		catch (...)
		{
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const int8_t SetValue)
	{
		string_operator_lock.lock();

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try 
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const uint8_t SetValue)
	{
		string_operator_lock.lock();

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const int16_t SetValue)
	{
		string_operator_lock.lock();

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const uint16_t SetValue)
	{
		string_operator_lock.lock();

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif
		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const int32_t SetValue)
	{
		string_operator_lock.lock();

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRId32, SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
		
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const uint32_t SetValue)
	{
		string_operator_lock.lock();

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRIu32, SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);
		if (buffer_ < (Size_ + 1))
		{
			//null終端分追加
			if (!this->ReAlloc(Size_ + 1))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const int64_t SetValue)
	{
		string_operator_lock.lock();

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRId64, SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const uint64_t SetValue)
	{
		string_operator_lock.lock();

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRIu64, SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}
		catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const float SetValue)
	{
		string_operator_lock.lock();

		char Str[47]{};
		snprintf(Str, sizeof(Str), "%g", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};
		}catch (std::bad_alloc&)
		{
			throw;
		};

#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator =(const double SetValue)
	{
		string_operator_lock.lock();

		char Str[94]{};
		snprintf(Str, sizeof(Str), "%lg", SetValue);

		uint64_t Size_ = SonikLibStringConvert::GetStringLengthByte(Str);

		try
		{
			if (buffer_ < (Size_ + 1))
			{
				//null終端分追加
				if (!this->ReAlloc(Size_ + 1))
				{
					string_operator_lock.unlock();
					throw std::bad_alloc();
				};
			};

		}catch (std::bad_alloc&)
		{
			throw;
		};


#if defined(_S_STRING_COPY_STL)
		std::fill_n(Stringval_, buffer_, 0);
#else
		std::memset(Stringval_, 0, buffer_);
#endif

		if (strcmp(Str, "") == 0)
		{
			CType = SCHTYPE_NULLTEXT;
			string_operator_lock.unlock();
			return (*this);
		};

		CType = SonikLibStringConvert::CheckConvertType(Str);

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(Str, Size_, reinterpret_cast<char*>(Stringval_));
#else
			std::memcpy(Stringval_, Str, Size_);
#endif

		}
		catch (...)
		{
			string_operator_lock.unlock();
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};


	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const SonikString_pImpl& t_his)
	{
		string_operator_lock.lock();

		if (t_his.CType == SCHTYPE_NULLTEXT || t_his.CType == SCHTYPE_UNKNOWN)
		{
			//c:結合を行わない
			string_operator_lock.unlock();
			return (*this);
		};

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != t_his.CType)
		{
			SetCharacterType(t_his.CType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_;
		uint64_t bufuse_;
		uint32_t NullStrLengthByte = 0;
		if (t_his.CType == SCHTYPE_UTF16)
		{
			//UTF16
			CopySize_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(t_his.Stringval_));
			bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));

			NullStrLengthByte = 2;

		}
		else if (t_his.CType == SCHTYPE_UTF32)
		{
			//UTF32
			CopySize_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char32_t*>(t_his.Stringval_));
			bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char32_t*>(Stringval_));

			NullStrLengthByte = 4;
		}
		else
		{

			CopySize_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(t_his.Stringval_));
			bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

			NullStrLengthByte = 1;
		};

		uint64_t newbuffersize = (CopySize_ + bufuse_) + NullStrLengthByte;
		if (buffer_ < newbuffersize)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			utf8_t* l_offsetPoint = Stringval_ + bufuse_;
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(t_his.Stringval_, CopySize_, l_offsetPoint);

#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, t_his.Stringval_, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		if (CType == SCHTYPE_UTF16)
		{
			MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char16_t*>(Stringval_));
		}
		else if (CType == SCHTYPE_UTF32)
		{
			MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char32_t*>(Stringval_));
		}
		else
		{
			MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));
		};

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const char* Str)
	{
		string_operator_lock.lock();

		if (Str == nullptr)
		{
			string_operator_lock.unlock();
			return (*this);
		}

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);

#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const char16_t* w_Str)
	{
		string_operator_lock.lock();

		if (w_Str == nullptr)
		{
			string_operator_lock.unlock();
			return (*this);
		}

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。(ここではUTF16確定になる)
		if (CType != SCHTYPE_UTF16)
		{
			SetCharacterType(SCHTYPE_UTF16);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(w_Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 2;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char16_t* l_offsetPoint = reinterpret_cast<char16_t*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(reinterpret_cast<char*>(l_offsetPoint), (buffer_ - bufuse_), 0);

			CopySize_ >>= 1;
			std::copy_n(w_Str, CopySize_, l_offsetPoint);

#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, w_Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char16_t*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const wchar_t* w_Str)
	{
		string_operator_lock.lock();

		if (w_Str == nullptr)
		{
			string_operator_lock.unlock();
			return (*this);
		}

#if WCHAR_MAX <= 0xFFFFU
		uint32_t NULLSTR_SIZE = 2;
		using CONST_CHRTYPE = const char16_t*;
		using CHRTYPE = char16_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;

#else
		uint32_t NULLSTR_SIZE = 4;
		using CONST_CHRTYPE = const char32_t*;
		using CHRTYPE = char32_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != CONVTYPE)
		{
			SetCharacterType(CONVTYPE);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_CHRTYPE>(w_Str));
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CHRTYPE>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + NULLSTR_SIZE;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{

			wchar_t* l_offsetPoint = reinterpret_cast<wchar_t*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(reinterpret_cast<char*>(l_offsetPoint), (buffer_ - bufuse_), 0);

	#if WCHAR_MAX <= 0xFFFFU
			CopySize_ >>= 1;
	#else
			CopySize_ >>= 2;
	#endif
			std::copy_n(w_Str, CopySize_, l_offsetPoint);

#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, w_Str, CopySize_);
#endif

		}catch (...)
		{
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<CHRTYPE>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const utf8_t* utf8_Str)
	{
		string_operator_lock.lock();

		if (utf8_Str == nullptr)
		{
			string_operator_lock.unlock();
			return (*this);
		}

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(utf8_Str));

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(utf8_Str));
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{

			utf8_t* l_offsetPoint = reinterpret_cast<utf8_t*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(reinterpret_cast<char*>(l_offsetPoint), (buffer_ - bufuse_), 0);
			std::copy_n(utf8_Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, utf8_Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		};

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);

	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const int8_t SetValue)
	{
		string_operator_lock.lock();

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const uint8_t SetValue)
	{
		string_operator_lock.lock();

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const int16_t SetValue)
	{
		string_operator_lock.lock();

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const uint16_t SetValue)
	{
		string_operator_lock.lock();

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const int32_t SetValue)
	{
		string_operator_lock.lock();

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRId32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const uint32_t SetValue)
	{
		string_operator_lock.lock();

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRIu32, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const int64_t SetValue)
	{
		string_operator_lock.lock();

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRId64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif
		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const uint64_t SetValue)
	{
		string_operator_lock.lock();

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRIu64, SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif
		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const float SetValue)
	{
		string_operator_lock.lock();

		char Str[47]{};
		snprintf(Str, sizeof(Str), "%g", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif
		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl& SonikStringBase::SonikString_pImpl::operator +=(const double SetValue)
	{
		string_operator_lock.lock();

		char Str[94]{};
		snprintf(Str, sizeof(Str), "%lg", SetValue);

		SonikLibConvertType tmpType = SonikLibStringConvert::CheckConvertType(Str);

		//コピー元とタイプが違えばコピー元に合わせて変換をかける。
		if (CType != tmpType)
		{
			SetCharacterType(tmpType);
		};

		//カウントを行い、結合時にはみ出すようなら再確保
		uint64_t CopySize_ = SonikLibStringConvert::GetStringLengthByte(Str);
		uint64_t bufuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));

		uint64_t newbuffersize = (CopySize_ + bufuse_) + 1;
		if (newbuffersize > buffer_)
		{
			if (!this->ReAlloc(newbuffersize))
			{
				string_operator_lock.unlock();
				throw std::bad_alloc();
			};
		};

		try
		{
			char* l_offsetPoint = reinterpret_cast<char*>(Stringval_ + bufuse_);
			//合計数がbuffer_以内の場合、ReAllocはコールされず、ゴミが後ろに残ってしまうので現在の文字数より後ろの配列部分を0クリア
#if defined(_S_STRING_COPY_STL)
			std::fill_n(l_offsetPoint, (buffer_ - bufuse_), 0);
			std::copy_n(Str, CopySize_, l_offsetPoint);
#else
			std::memset(l_offsetPoint, 0, (buffer_ - bufuse_));
			std::memcpy(l_offsetPoint, Str, CopySize_);
#endif

		}
		catch (...)
		{
			throw;
		}

		MaxLength_ = SonikLibStringConvert::GetStringCount(reinterpret_cast<char*>(Stringval_));

		string_operator_lock.unlock();
		return (*this);
	};

	//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const SonikString_pImpl& t_his)
	{
		SonikString_pImpl tmpStr = (*this);

		tmpStr += t_his;

		return tmpStr;
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const char* Str)
	{
		SonikString_pImpl tmpStr = (*this);

		tmpStr += Str;

		return tmpStr;
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const char16_t* w_Str)
	{
		SonikString_pImpl tmpStr = (*this);

		tmpStr += w_Str;

		return tmpStr;

	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const wchar_t* w_Str)
	{
		SonikString_pImpl tmpStr = (*this);

		tmpStr += w_Str;

		return tmpStr;
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const utf8_t* utf8_Str)
	{
		SonikString_pImpl tmpStr = (*this);

		tmpStr += utf8_Str;

		return tmpStr;
	};

	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const int8_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const uint8_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[5]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const int16_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%d", SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const uint16_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[7]{};
		snprintf(Str, sizeof(Str), "%u", SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const int32_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRId32, SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const uint32_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[12]{};
		snprintf(Str, sizeof(Str), "%" PRIu32, SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const int64_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRId64, SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const uint64_t SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[21]{};
		snprintf(Str, sizeof(Str), "%" PRIu64, SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const float SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[47]{};
		snprintf(Str, sizeof(Str), "%g", SetValue);

		tmpStr += Str;

		return tmpStr;
	};
	DEF_FORCE_INLINE SonikStringBase::SonikString_pImpl SonikStringBase::SonikString_pImpl::operator +(const double SetValue)
	{
		SonikString_pImpl tmpStr = (*this);

		char Str[94]{};
		snprintf(Str, sizeof(Str), "%lg", SetValue);

		tmpStr += Str;

		return tmpStr;
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 一致の場合true 不一致の場合 falseを返却します。
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator ==(const SonikString_pImpl& t_his)
	{
		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != t_his.CType)
		{
			if (!SetCharacterType(t_his.CType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return false;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = 0;
		uint64_t targetuse_ = 0;
		if (CType == SCHTYPE_UTF16)
		{
			myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));
			targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(t_his.Stringval_));

		}
		else
		{
			myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
			targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(t_his.Stringval_));
		};

		//c: サイズが一緒なら次の精査へ
		if (myuse_ != targetuse_)
		{
			return false;
		};

		//c: 文字数も一緒であればByte精査
		utf8_t* target_str = t_his.Stringval_;
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != target_str[i])
			{
				return false;
			};

		};

		//c: 全部越えたら一致と判定
		return true;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator ==(const char* Str)
	{
		SonikLibConvertType StrType = SonikLibStringConvert::CheckConvertType(Str);

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != StrType)
		{
			if (!SetCharacterType(StrType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return false;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(Str);

		if (myuse_ != targetuse_)
		{
			return false;
		};

		//c: 文字数も一緒であればByte精査
		const utf8_t* target_str = reinterpret_cast<const utf8_t*>(Str);
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != target_str[i])
			{
				return false;
			};

		};

		//c: 全部越えたら一致と判定
		return true;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator ==(const char16_t* w_Str)
	{
		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != SCHTYPE_UTF16)
		{
			if (!SetCharacterType(SCHTYPE_UTF16))
			{
				//c: 変換ミスが発生したら不一致として返却
				return false;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(w_Str);

		if (myuse_ != targetuse_)
		{
			return false;
		};

		//c: 文字数も一緒であればByte精査
		const utf8_t* target_str = reinterpret_cast<const utf8_t*>(w_Str);
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != target_str[i])
			{
				return false;
			};

		};

		//c: 全部越えたら一致と判定
		return true;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator ==(const wchar_t* w_Str)
	{
#if WCHAR_MAX <= 0xFFFFU
		//uint32_t NULLSTR_SIZE = 2;
		using CONST_CHRTYPE = const char16_t*;
		using CHRTYPE = char16_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;

#else
		//uint32_t NULLSTR_SIZE = 4;
		using CONST_CHRTYPE = const char32_t*;
		using CHRTYPE = char32_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != CONVTYPE)
		{
			if (!SetCharacterType(CONVTYPE))
			{
				//c: 変換ミスが発生したら不一致として返却
				return false;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CHRTYPE>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_CHRTYPE>(w_Str));

		if (myuse_ != targetuse_)
		{
			return false;
		};

		//c: 文字数も一緒であればByte精査
		const utf8_t* target_str = reinterpret_cast<const utf8_t*>(w_Str);
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != w_Str[i])
			{
				return false;
			};

		};

		//c: 全部越えたら一致と判定
		return true;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator ==(const utf8_t* utf8_Str)
	{
		SonikLibConvertType StrType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(utf8_Str));

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != StrType)
		{
			if (!SetCharacterType(StrType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return false;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(utf8_Str));

		if (myuse_ != targetuse_)
		{
			return false;
		};

		//c: 文字数も一緒であればByte精査
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != utf8_Str[i])
			{
				return false;
			};

		};

		//c: 全部越えたら一致と判定
		return true;
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 不一致の場合true　一致の場合 falseを返却します。
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator !=(const SonikString_pImpl& t_his)
	{
		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != t_his.CType)
		{
			if (!SetCharacterType(t_his.CType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return true;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = 0;
		uint64_t targetuse_ = 0;
		if (CType == SCHTYPE_UTF16)
		{
			myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));
			targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(t_his.Stringval_));

		}
		else
		{
			myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
			targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(t_his.Stringval_));
		};

		//c: サイズが一緒なら次の精査へ
		if (myuse_ != targetuse_)
		{
			return true;
		};

		//c: 文字数も一緒であればByte精査
		utf8_t* target_str = t_his.Stringval_;
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != target_str[i])
			{
				return true;
			};

		};

		//c: 全部越えたら一致と判定
		return false;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator !=(const char* Str)
	{
		SonikLibConvertType StrType = SonikLibStringConvert::CheckConvertType(Str);

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != StrType)
		{
			if (!SetCharacterType(StrType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return true;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(Str);

		if (myuse_ != targetuse_)
		{
			return true;
		};

		//c: 文字数も一緒であればByte精査
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != Str[i])
			{
				return true;
			};

		};

		//c: 全部越えたら一致と判定
		return false;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator !=(const char16_t* w_Str)
	{
		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != SCHTYPE_UTF16)
		{
			if (!SetCharacterType(SCHTYPE_UTF16))
			{
				//c: 変換ミスが発生したら不一致として返却
				return true;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char16_t*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(w_Str);

		if (myuse_ != targetuse_)
		{
			return true;
		};

		//c: 文字数も一緒であればByte精査
		const utf8_t* target_str = reinterpret_cast<const utf8_t*>(w_Str);
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != target_str[i])
			{
				return true;
			};

		};

		//c: 全部越えたら一致と判定
		return false;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator !=(const wchar_t* w_Str)
	{

#if WCHAR_MAX <= 0xFFFFU
		//uint32_t NULLSTR_SIZE = 2;
		using CONST_CHRTYPE = const char16_t*;
		using CHRTYPE = char16_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF16;

#else
		//uint32_t NULLSTR_SIZE = 4;
		using CONST_CHRTYPE = const char32_t*;
		using CHRTYPE = char32_t*;
		SonikLibConvertType CONVTYPE = SCHTYPE_UTF32;
#endif

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != CONVTYPE)
		{
			if (!SetCharacterType(CONVTYPE))
			{
				//c: 変換ミスが発生したら不一致として返却
				return true;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CHRTYPE>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<CONST_CHRTYPE>(w_Str));

		if (myuse_ != targetuse_)
		{
			return true;
		};

		//c: 文字数も一緒であればByte精査
		const utf8_t* target_str = reinterpret_cast<const utf8_t*>(w_Str);
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != w_Str[i])
			{
				return true;
			};

		};

		//c: 全部越えたら一致と判定
		return false;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::operator !=(const utf8_t* utf8_Str)
	{
		SonikLibConvertType StrType = SonikLibStringConvert::CheckConvertType(reinterpret_cast<const char*>(utf8_Str));

		//c: 文字タイプが違えば相手と同じ文字タイプに変換
		if (CType != StrType)
		{
			if (!SetCharacterType(StrType))
			{
				//c: 変換ミスが発生したら不一致として返却
				return true;
			};
		};

		//c: 文字数が違えば違う文字列として判定(不一致)
		uint64_t myuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<char*>(Stringval_));
		uint64_t targetuse_ = SonikLibStringConvert::GetStringLengthByte(reinterpret_cast<const char*>(utf8_Str));

		if (myuse_ != targetuse_)
		{
			return true;
		};

		//c: 文字数も一緒であればByte精査
		for (uint64_t i = 0; i < myuse_; ++i)
		{
			if (Stringval_[i] != utf8_Str[i])
			{
				return true;
			};

		};

		//c: 全部越えたら一致と判定
		return false;
	};

	//c:比較演算子
	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::Greater(const char* CompareArg2val)
	{
		return strcmp(this->str_c(), CompareArg2val) > 0;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::Less(const char* CompareArg2val)
	{
		return strcmp(this->str_c(), CompareArg2val) < 0;
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::GreaterEqual(const char* CompareArg2val)
	{
		return !(this->Less(CompareArg2val));
	};

	DEF_FORCE_INLINE bool SonikStringBase::SonikString_pImpl::LessEqual(const char* CompareArg2val)
	{
		return !(this->Greater(CompareArg2val));
	};
};

//======================================================================================================================
//
//				pImpl使用（外側)クラスの実装
//
//======================================================================================================================
//======================================================================================================================
//
//				SonikStringBase実装
//
//======================================================================================================================
namespace BASED_STRINGCLASS_SONIKLIB
{
	//コンストラクタ
	SonikStringBase::SonikStringBase(void)
	:pImpl(nullptr)
	{
		//no porcess;
	};

	SonikStringBase::~SonikStringBase(void)
	{
		//no process 
		//deleteは派生先で実行
		//Baseは関数提供のみ
	};

	//SJIS形式に変換して取得します。(バッファタイプも書き換わります。)
	const char* SonikStringBase::str_c(void)
	{
		return pImpl->str_c();
	};

	//wchar_t形式に変換して取得します。(バッファタイプも書き換わります。)
	const wchar_t* SonikStringBase::str_wchar(void)
	{
		return pImpl->str_wchar();
	};

	//UTF16形式に変換して取得します。(バッファタイプも書き換わります。)
	const char16_t* SonikStringBase::str_utf16(void)
	{
		return pImpl->str_utf16();
	};

	//UTF8形式に変換して取得します。(バッファタイプも書き換わります。)
	const utf8_t* SonikStringBase::str_utf8(void)
	{
		return pImpl->str_utf8();
	};

	//SJIS形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
	//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
	uint64_t SonikStringBase::GetCpy_str_c(char* dstBuffer)
	{
		return pImpl->GetCpy_str_c(dstBuffer);
	};

	//wchar_t形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
	//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
	uint64_t SonikStringBase::GetCpy_str_wcstr(wchar_t* dstBuffer)
	{
		return pImpl->GetCpy_str_wcstr(dstBuffer);
	};

	//UTF16形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
	//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
	uint64_t SonikStringBase::GetCpy_str_utf16(char16_t* dstBuffer)
	{
		return pImpl->GetCpy_str_utf16(dstBuffer);
	};

	//UTF8形式に変換して、バッファをdstBufferにコピーします。(バッファタイプも書き換わります。)
	//第１引数を省略してコールした場合はdstに必要なバッファサイズを取得することができます。(単位/1Byte)
	uint64_t SonikStringBase::GetCpy_str_utf8(utf8_t* dstBuffer)
	{
		return pImpl->GetCpy_str_utf8(dstBuffer);
	};

	//c:文字列のByte数を取得します。（Null終端文字をカウントに含まない)
	uint64_t SonikStringBase::Count_Byte_NotNull(void)
	{
		return pImpl->Count_Byte_NotNull();
	};

	//c:文字列数を取得します。（Null終端文字をカウントに含まない)
	uint64_t SonikStringBase::Count_Str_NotNull(void)
	{
		return pImpl->Count_Str_NotNull();
	};

	bool SonikStringBase::Split_Ascii(const char* delim, SonikLib::SonikStringSplitObject& _split_)
	{
		if (_split_.mp_split != nullptr)
		{
			m_allocator->memdel(_split_.mp_split);
			_split_.mp_split = nullptr;
		};

		char* nlp = nullptr;
		pImpl->Split_Ascii(delim, nlp, _split_.m_splitCnt, _split_.textbuffersize);

		void* l_allocbuffer = _split_.m_allocator->memal(_split_.textbuffersize);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		_split_.mp_split = new(l_allocbuffer) char[_split_.textbuffersize]{};

		if (!pImpl->Split_Ascii(delim, nlp, _split_.m_splitCnt, _split_.textbuffersize))
		{
			_split_.m_allocator->memdel(_split_.mp_split);
			_split_.mp_split = nullptr;
			return false;
		};

		return true;
	};

	//文字列中の全角英数字を半角英数字に変換します。
	bool SonikStringBase::ConvertFWANtoHWAN(void)
	{
		return pImpl->ConvertFWANtoHWAN();
	};

	//文字列中の全角カナを半角カナに変換します。
	bool SonikStringBase::ConvertFWKNtoHWKN(void)
	{
		return pImpl->ConvertFWKNtoHWKN();
	};

	//c: 指定位置の文字を削除します。
	void SonikStringBase::EraseChar(uint64_t ChrPoint)
	{
		pImpl->EraseChar(ChrPoint);
	};

	//c: 指定の開始位置から指定された文字数を削除します。
	void SonikStringBase::EraseStr(uint64_t ChrStartPoint, uint64_t ChrEndPoint)
	{
		pImpl->EraseStr(ChrStartPoint, ChrEndPoint);
	};

	bool SonikStringBase::operator !=(const char* Str)
	{
		return (*pImpl) != Str;
	};

	bool SonikStringBase::operator !=(const char16_t* w_Str)
	{
		return (*pImpl) != w_Str;
	};

	bool SonikStringBase::operator !=(const wchar_t* w_Str)
	{
		return (*pImpl) != w_Str;
	};

	bool SonikStringBase::operator !=(const utf8_t* utf8_Str)
	{
		return (*pImpl) != utf8_Str;
	};

	bool SonikStringBase::operator ==(const char* Str)
	{
		return (*pImpl) == Str;
	};

	bool SonikStringBase::operator ==(const char16_t* w_Str)
	{
		return (*pImpl) == w_Str;
	};

	bool SonikStringBase::operator ==(const wchar_t* w_Str)
	{
		return (*pImpl) == w_Str;
	};

	bool SonikStringBase::operator ==(const utf8_t* utf8_Str)
	{
		return (*pImpl) == utf8_Str;
	};
};

namespace SonikLib
{
	//======================================================================================================================
	//
	//				SonikStringSplitObject実装
	//
	//======================================================================================================================
	SonikStringSplitObject::SonikStringSplitObject(void)
	:mp_split(nullptr)
	,m_splitCnt(0)
	,textbuffersize(0)
	{
		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface();
			if(!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

		}catch (std::bad_alloc&)
		{
			throw;
		};


	};
	SonikStringSplitObject::SonikStringSplitObject(SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface> _allocator_)
	:mp_split(nullptr)
	,m_splitCnt(0)
	,textbuffersize(0)
	,m_allocator(_allocator_)
	{

	};

	SonikStringSplitObject::~SonikStringSplitObject(void)
	{
		if (mp_split != nullptr)
		{
			m_allocator->memdel(mp_split);
		};
	};

	bool SonikStringSplitObject::GetStr(uint64_t _splitnum_, SonikString& _getstr_)
	{
		if (_splitnum_ >= m_splitCnt || mp_split == nullptr)
		{
			return false;
		};

		uintptr_t* l_ptr = reinterpret_cast<uintptr_t*>(mp_split);
		_getstr_ = SonikString(reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]), m_allocator);

		return true;
	};

	bool SonikStringSplitObject::GetStr(uint64_t _splitnum_, SonikStringWIDE& _getstr_)
	{
		if (_splitnum_ >= m_splitCnt || mp_split == nullptr)
		{
			return false;
		};

		uintptr_t* l_ptr = reinterpret_cast<uintptr_t*>(mp_split);
		_getstr_ = SonikStringWIDE(reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]), m_allocator);

		return true;
	};

	bool SonikStringSplitObject::GetStr(uint64_t _splitnum_, SonikStringUTF8& _getstr_)
	{
		if (_splitnum_ >= m_splitCnt || mp_split == nullptr)
		{
			return false;
		};

		uintptr_t* l_ptr = reinterpret_cast<uintptr_t*>(mp_split);
		_getstr_ = SonikStringUTF8(reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]), m_allocator);

		return true;
	};

	bool SonikStringSplitObject::GetStr(uint64_t _splitnum_, SonikStringUTF16& _getstr_)
	{
		if (_splitnum_ >= m_splitCnt || mp_split == nullptr)
		{
			return false;
		};

		uintptr_t* l_ptr = reinterpret_cast<uintptr_t*>(mp_split);
		_getstr_ = SonikStringUTF16(reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]), m_allocator);

		return true;
	};

	bool SonikStringSplitObject::Split(uint64_t _splitnum_, const char* _delim_, SonikStringSplitObject& _out_)
	{
		if (_splitnum_ >= m_splitCnt || mp_split == nullptr)
		{
			return false;
		};

		uintptr_t* l_ptr = reinterpret_cast<uintptr_t*>(mp_split);
		wchar_t* str = reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]);

		uint64_t l_strcnt = 0;
		while ((*str) != 0x00)
		{
			++l_strcnt;
			++str;
		};


		SonikLib::SonikString l_delimstr = SonikLib::SonikString(_delim_, m_allocator);
		const wchar_t* l_delim = l_delimstr.str_wchar();
		uint64_t l_splitsize = 0;
		wchar_t* l_start = reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]);
		wchar_t* l_end = nullptr;

		while ((l_end = wcsstr(l_start, l_delim)) != nullptr)
		{
			++l_splitsize;
			l_start = (++l_end);
		};

		if (l_splitsize == 0)
		{
			//１個も見つからなければ終了
			return false;
		};

		//splitの数分、null文字と先頭メタ領域にアドレスが必要なのでその分のバッファを確保(split数が0より上なら文字列先頭も必要なのでカウント数 + 1個必要なので加算しておく。またnull文字も必要なので最後に+1)
		// sizeof(wchar_t) * MaxLength_ + (l_splitcnt + 1) = 文字数分のサイズ + スプリット数分のnull(\0)文字数。
		// sizeof(uintptr_t) * (l_splitcnt +1) = 先頭メタアドレスタグ領域のサイズ
		++l_splitsize;
		uint64_t l_bufsize = (sizeof(wchar_t) * (l_strcnt + 1)) + (sizeof(uintptr_t) * l_splitsize);

		void* l_allocbuffer = _out_.m_allocator->memal(l_bufsize);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char* l_buffer = new(l_allocbuffer) char[l_bufsize] {};

		//書き込み位置設定
		uintptr_t* l_meta_write = reinterpret_cast<uintptr_t*>(l_buffer);
		l_start = reinterpret_cast<wchar_t*>(l_meta_write + l_splitsize);
		l_end = nullptr;

		//まず文字列コピー
		memcpy(l_start, reinterpret_cast<wchar_t*>(l_ptr[_splitnum_]), l_strcnt * 2);

		//先頭ポインタを設定
		(*l_meta_write) = reinterpret_cast<uintptr_t>(l_start);
		++l_meta_write;

		//Split本番
		while ((l_end = wcsstr(l_start, l_delim)) != nullptr)
		{
			//スプリット対象の文字をnull文字に変える。
			(*l_end) = 0x00;
			l_start = (++l_end);

			//アドレス記述領域に地点を記載
			(*l_meta_write) = reinterpret_cast<uintptr_t>(l_start);
			++l_meta_write;
		};

		//最後にオブジェクトへ登録
		if (_out_.mp_split != nullptr)
		{
			_out_.m_allocator->memdel(_out_.mp_split);
		};

		_out_.mp_split = l_buffer;
		_out_.m_splitCnt = l_splitsize;
		_out_.textbuffersize = l_bufsize - (sizeof(uintptr_t) * l_splitsize);

		return true;
	};



	//======================================================================================================================
	//
	//				SonikString実装
	//
	//======================================================================================================================
	SonikString::SonikString(void)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		pImpl = nullptr;
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(_allocator_);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikString::SonikString(const SonikString& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikString::SonikString(const SonikStringWIDE& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikString::SonikString(const SonikStringUTF8& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};
	SonikString::SonikString(const SonikStringUTF16& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikString::SonikString(const char* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikString::SonikString(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const char16_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikString::SonikString(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const wchar_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikString::SonikString(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		
		}catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const utf8_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikString::SonikString(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{

			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const int8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const uint8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const int16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;
		
		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const uint16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const int32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const uint32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const int64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const uint64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const float SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikString::SonikString(const double SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikString::SonikString(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	//ムーヴコンストラクタ
	SonikString::SonikString(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};
	SonikString::SonikString(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikString::SonikString(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikString::SonikString(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};


	SonikString::~SonikString(void)
	{
		if (pImpl != 0)
		{
			pImpl->~SonikString_pImpl();
			m_allocator->memdel(pImpl);
			pImpl = nullptr;
		};

		m_allocator.~AllocatorSharedSmtPtr();
	};

	//define切り替えのstrゲット
	const char* SonikString::definition_str(void)
	{
		return pImpl->str_c();
	};

	//define切り替えのStrCopy
	uint64_t SonikString::GetCpy_str_definition(char* dstBuffer)
	{
		return pImpl->GetCpy_str_c(dstBuffer);
	};

	//各入力フォーマットから現在のバッファに変換して代入します。
	SonikString& SonikString::operator =(const SonikString& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator =(const SonikStringWIDE& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator =(const SonikStringUTF8& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator =(const SonikStringUTF16& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator =(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikString& SonikString::operator =(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikString& SonikString::operator =(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikString& SonikString::operator =(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};

	SonikString& SonikString::operator =(const char* Str)
	{
		(*pImpl) = Str;

		return (*this);
	};

	SonikString& SonikString::operator =(const char16_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikString& SonikString::operator =(const wchar_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikString& SonikString::operator =(const utf8_t* utf8_Str)
	{
		(*pImpl) = utf8_Str;

		return (*this);
	};

	SonikString& SonikString::operator =(const int8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const uint8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const int16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const uint16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const int32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const uint32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const int64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const uint64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const float SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikString& SonikString::operator =(const double SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	//各入力フォーマットから現在のバッファに変換して結合します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikString& SonikString::operator +=(const SonikString& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator +=(const SonikStringWIDE& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator +=(const SonikStringUTF8& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator +=(const SonikStringUTF16& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikString& SonikString::operator +=(const char* Str)
	{
		(*pImpl) += Str;

		return (*this);
	};

	SonikString& SonikString::operator +=(const char16_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikString& SonikString::operator +=(const wchar_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikString& SonikString::operator +=(const utf8_t* utf8_Str)
	{
		(*pImpl) += utf8_Str;

		return (*this);
	};

	SonikString& SonikString::operator +=(const int8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const uint8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const int16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const uint16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const int32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const uint32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const int64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const uint64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const float SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikString& SonikString::operator +=(const double SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikString SonikString::operator +(const SonikString& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikString SonikString::operator +(const SonikStringWIDE& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikString SonikString::operator +(const SonikStringUTF8& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};
	SonikString SonikString::operator +(const SonikStringUTF16& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikString SonikString::operator +(const char* Str)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + Str;

		return str_;
	};

	SonikString SonikString::operator +(const char16_t* w_Str)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikString SonikString::operator +(const wchar_t* w_Str)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikString SonikString::operator +(const utf8_t* utf8_Str)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + utf8_Str;

		return str_;
	};

	SonikString SonikString::operator +(const int8_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const uint8_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const int16_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const uint16_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const int32_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const uint32_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const int64_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const uint64_t SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const float SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikString SonikString::operator +(const double SetValue)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};


	//c: 文字列同士を比較します。(strcmp)
	//c: 一致の場合true 不一致の場合 falseを返却します。
	bool SonikString::operator ==(const SonikString& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikString::operator ==(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikString::operator ==(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikString::operator ==(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 不一致の場合true　一致の場合 falseを返却します。
	bool SonikString::operator !=(const SonikString& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikString::operator !=(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikString::operator !=(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikString::operator !=(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	//小なり
	bool SonikString::operator <(const SonikString& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};
	bool SonikString::operator <(const SonikStringWIDE& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};
	bool SonikString::operator <(const SonikStringUTF8& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};
	bool SonikString::operator <(const SonikStringUTF16& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	//大なり
	bool SonikString::operator >(const SonikString& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikString::operator >(const SonikStringWIDE& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikString::operator >(const SonikStringUTF8& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikString::operator >(const SonikStringUTF16& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	//小なりイコール
	bool SonikString::operator <=(const SonikString& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};
	bool SonikString::operator <=(const SonikStringWIDE& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};
	bool SonikString::operator <=(const SonikStringUTF8& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};
	bool SonikString::operator <=(const SonikStringUTF16& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	//大なりイコール
	bool SonikString::operator >=(const SonikString& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikString::operator >=(const SonikStringWIDE& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikString::operator >=(const SonikStringUTF8& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikString::operator >=(const SonikStringUTF16& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	//======================================================================================================================
	//
	//				SonikStringWIDE実装
	//
	//======================================================================================================================
	SonikStringWIDE::SonikStringWIDE(void)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		pImpl = nullptr;
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(_allocator_);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringWIDE::SonikStringWIDE(const SonikStringWIDE& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringWIDE::SonikStringWIDE(const SonikString& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringWIDE::SonikStringWIDE(const SonikStringUTF8& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const SonikStringUTF16& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringWIDE::SonikStringWIDE(const char* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringWIDE::SonikStringWIDE(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const char16_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringWIDE::SonikStringWIDE(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const wchar_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringWIDE::SonikStringWIDE(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);

		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const utf8_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringWIDE::SonikStringWIDE(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{

			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const int8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const uint8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const int16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const uint16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const int32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const uint32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const int64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const uint64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const float SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringWIDE::SonikStringWIDE(const double SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringWIDE::SonikStringWIDE(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	//ムーヴコンストラクタ
	SonikStringWIDE::SonikStringWIDE(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};
	SonikStringWIDE::SonikStringWIDE(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringWIDE::SonikStringWIDE(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringWIDE::SonikStringWIDE(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	//デストラクタ
	SonikStringWIDE::~SonikStringWIDE(void)
	{
		if (pImpl != 0)
		{
			pImpl->~SonikString_pImpl();
			m_allocator->memdel(pImpl);
			pImpl = nullptr;
		};

		m_allocator.~AllocatorSharedSmtPtr();
	};

	//define切り替えのstrゲット
	const wchar_t* SonikStringWIDE::definition_str(void)
	{
		return pImpl->str_wchar();
	};

	//define切り替えのStrCopy
	uint64_t SonikStringWIDE::GetCpy_str_definition(wchar_t* dstBuffer)
	{
		return pImpl->GetCpy_str_wcstr(dstBuffer);
	};

	//各入力フォーマットから現在のバッファに変換して代入します。
	SonikStringWIDE& SonikStringWIDE::operator =(const SonikStringWIDE& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const SonikString& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const SonikStringUTF8& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const SonikStringUTF16& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringWIDE& SonikStringWIDE::operator =(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringWIDE& SonikStringWIDE::operator =(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringWIDE& SonikStringWIDE::operator =(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const char* Str)
	{
		(*pImpl) = Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const char16_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const wchar_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const utf8_t* utf8_Str)
	{
		(*pImpl) = utf8_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const int8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const uint8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const int16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const uint16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const int32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const uint32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const int64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const uint64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const float SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator =(const double SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	//各入力フォーマットから現在のバッファに変換して結合します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringWIDE& SonikStringWIDE::operator +=(const SonikStringWIDE& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const SonikString& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const SonikStringUTF8& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const SonikStringUTF16& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const char* Str)
	{
		(*pImpl) += Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const char16_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const wchar_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const utf8_t* utf8_Str)
	{
		(*pImpl) += utf8_Str;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const int8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const uint8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const int16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const uint16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const int32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const uint32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const int64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const uint64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const float SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringWIDE& SonikStringWIDE::operator +=(const double SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringWIDE SonikStringWIDE::operator +(const SonikStringWIDE& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const SonikString& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const SonikStringUTF8& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};
	SonikStringWIDE SonikStringWIDE::operator +(const SonikStringUTF16& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const char* Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + Str;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const char16_t* w_Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const wchar_t* w_Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const utf8_t* utf8_Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + utf8_Str;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const int8_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const uint8_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const int16_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const uint16_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const int32_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const uint32_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const int64_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const uint64_t SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const float SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringWIDE SonikStringWIDE::operator +(const double SetValue)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};


	//c: 文字列同士を比較します。(strcmp)
	//c: 一致の場合true 不一致の場合 falseを返却します。
	bool SonikStringWIDE::operator ==(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator ==(const SonikString& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator ==(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator ==(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 不一致の場合true　一致の場合 falseを返却します。
	bool SonikStringWIDE::operator !=(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator !=(const SonikString& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator !=(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringWIDE::operator !=(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	//小なり
	bool SonikStringWIDE::operator <(const SonikStringWIDE& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringWIDE::operator <(const SonikString& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator <(const SonikStringUTF8& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator <(const SonikStringUTF16& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	//大なり
	bool SonikStringWIDE::operator >(const SonikStringWIDE& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >(const SonikString& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >(const SonikStringUTF8& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >(const SonikStringUTF16& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	//小なりイコール
	bool SonikStringWIDE::operator <=(const SonikStringWIDE& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringWIDE::operator <=(const SonikString& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator <=(const SonikStringUTF8& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator <=(const SonikStringUTF16& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	//大なりイコール
	bool SonikStringWIDE::operator >=(const SonikStringWIDE& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >=(const SonikString& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >=(const SonikStringUTF8& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};
	bool SonikStringWIDE::operator >=(const SonikStringUTF16& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	//======================================================================================================================
	//
	//				SonikStringUTF8実装
	//
	//======================================================================================================================
	SonikStringUTF8::SonikStringUTF8(void)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		pImpl = nullptr;
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(_allocator_);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF8::SonikStringUTF8(const SonikStringUTF8& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF8::SonikStringUTF8(const SonikString& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF8::SonikStringUTF8(const SonikStringWIDE& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const SonikStringUTF16& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF8::SonikStringUTF8(const char* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF8::SonikStringUTF8(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const char16_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF8::SonikStringUTF8(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const wchar_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF8::SonikStringUTF8(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);

		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const utf8_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF8::SonikStringUTF8(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{

			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const int8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const uint8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const int16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const uint16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const int32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const uint32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const int64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const uint64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const float SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF8::SonikStringUTF8(const double SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF8::SonikStringUTF8(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};
	
	//ムーヴコンストラクタ
	SonikStringUTF8::SonikStringUTF8(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};
	SonikStringUTF8::SonikStringUTF8(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringUTF8::SonikStringUTF8(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringUTF8::SonikStringUTF8(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	//デストラクタ
	SonikStringUTF8::~SonikStringUTF8(void)
	{
		if (pImpl != 0)
		{
			pImpl->~SonikString_pImpl();
			m_allocator->memdel(pImpl);
			pImpl = nullptr;
		};

		m_allocator.~AllocatorSharedSmtPtr();
	};

	//define切り替えのstrゲット
	const utf8_t* SonikStringUTF8::definition_str(void)
	{
		return pImpl->str_utf8();
	};

	//define切り替えのStrCopy
	uint64_t SonikStringUTF8::GetCpy_str_definition(utf8_t* dstBuffer)
	{
		return pImpl->GetCpy_str_utf8(dstBuffer);
	};

	//各入力フォーマットから現在のバッファに変換して代入します。
	SonikStringUTF8& SonikStringUTF8::operator =(const SonikStringUTF8& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const SonikString& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const SonikStringWIDE& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const SonikStringUTF16& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF8& SonikStringUTF8::operator =(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF8& SonikStringUTF8::operator =(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF8& SonikStringUTF8::operator =(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const char* Str)
	{
		(*pImpl) = Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const char16_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const wchar_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const utf8_t* utf8_Str)
	{
		(*pImpl) = utf8_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const int8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const uint8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const int16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const uint16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const int32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const uint32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const int64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const uint64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const float SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator =(const double SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	//各入力フォーマットから現在のバッファに変換して結合します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringUTF8& SonikStringUTF8::operator +=(const SonikStringUTF8& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const SonikString& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const SonikStringWIDE& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const SonikStringUTF16& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const char* Str)
	{
		(*pImpl) += Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const char16_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const wchar_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const utf8_t* utf8_Str)
	{
		(*pImpl) += utf8_Str;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const int8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const uint8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const int16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const uint16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const int32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const uint32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const int64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const uint64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const float SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF8& SonikStringUTF8::operator +=(const double SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringUTF8 SonikStringUTF8::operator +(const SonikStringUTF8& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const SonikString& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const SonikStringWIDE& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const SonikStringUTF16& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const char* Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + Str;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const char16_t* w_Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const wchar_t* w_Str)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const utf8_t* utf8_Str)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + utf8_Str;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const int8_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const uint8_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const int16_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const uint16_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const int32_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const uint32_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const int64_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const uint64_t SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const float SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF8 SonikStringUTF8::operator +(const double SetValue)
	{
		SonikStringUTF8 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 一致の場合true 不一致の場合 falseを返却します。
	bool SonikStringUTF8::operator ==(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator ==(const SonikString& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator ==(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator ==(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 不一致の場合true　一致の場合 falseを返却します。
	bool SonikStringUTF8::operator !=(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator !=(const SonikString& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator !=(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF8::operator !=(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	//小なり
	bool SonikStringUTF8::operator <(const SonikStringUTF8& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <(const SonikString& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <(const SonikStringWIDE& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <(const SonikStringUTF16& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	//大なり
	bool SonikStringUTF8::operator >(const SonikStringUTF8& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >(const SonikString& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >(const SonikStringWIDE& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >(const SonikStringUTF16& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	//小なりイコール
	bool SonikStringUTF8::operator <=(const SonikStringUTF8& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <=(const SonikString& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <=(const SonikStringWIDE& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator <=(const SonikStringUTF16& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	//大なりイコール
	bool SonikStringUTF8::operator >=(const SonikStringUTF8& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >=(const SonikString& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >=(const SonikStringWIDE& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF8::operator >=(const SonikStringUTF16& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	//======================================================================================================================
	//
	//				SonikStringUTF16実装
	//
	//======================================================================================================================
	SonikStringUTF16::SonikStringUTF16(void)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		pImpl = nullptr;
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(_allocator_);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF16::SonikStringUTF16(const SonikStringUTF8& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF16::SonikStringUTF16(const SonikString& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF16::SonikStringUTF16(const SonikStringWIDE& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const SonikStringUTF16& t_his)
	{
		m_allocator = t_his.m_allocator;
		pImpl = nullptr;

		void* l_allocbuffer = m_allocator->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl((*(t_his.pImpl)));
		}
		catch (std::bad_alloc&)
		{
			m_allocator->memdel(l_allocbuffer);
			throw;
		};
	};

	SonikStringUTF16::SonikStringUTF16(const char* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF16::SonikStringUTF16(const char* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const char16_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF16::SonikStringUTF16(const char16_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const wchar_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF16::SonikStringUTF16(const wchar_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);

		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const utf8_t* SetStr)
	{
		pImpl = nullptr;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};

	};
	SonikStringUTF16::SonikStringUTF16(const utf8_t* SetStr, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{

			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetStr, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const int8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const int8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const uint8_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const uint8_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const int16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const int16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const uint16_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const uint16_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const int32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const int32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const uint32_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const uint32_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const int64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const int64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const uint64_t SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const uint64_t SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const float SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const float SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	SonikStringUTF16::SonikStringUTF16(const double SetValue)
	{
		pImpl = 0;

		try
		{
			SonikLib::SLibAllocateInterface* l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	SonikStringUTF16::SonikStringUTF16(const double SetValue, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		m_allocator = _allocator_;
		pImpl = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl));
		try
		{
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			pImpl = new(l_allocbuffer) BASED_STRINGCLASS_SONIKLIB::SonikStringBase::SonikString_pImpl(SetValue, m_allocator);
		}
		catch (std::bad_alloc&)
		{
			_allocator_->memdel(l_allocbuffer);
			throw;
		};

	};

	//ムーヴコンストラクタ
	SonikStringUTF16::SonikStringUTF16(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};
	SonikStringUTF16::SonikStringUTF16(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringUTF16::SonikStringUTF16(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	SonikStringUTF16::SonikStringUTF16(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;
	};

	//デストラクタ
	SonikStringUTF16::~SonikStringUTF16(void)
	{
		if (pImpl != 0)
		{
			pImpl->~SonikString_pImpl();
			m_allocator->memdel(pImpl);
			pImpl = nullptr;
		};

		m_allocator.~AllocatorSharedSmtPtr();
	};

	//define切り替えのstrゲット
	const char16_t* SonikStringUTF16::definition_str(void)
	{
		return pImpl->str_utf16();
	};

	//define切り替えのStrCopy
	uint64_t SonikStringUTF16::GetCpy_str_definition(char16_t* dstBuffer)
	{
		return pImpl->GetCpy_str_utf16(dstBuffer);
	};

	//各入力フォーマットから現在のバッファに変換して代入します。
	SonikStringUTF16& SonikStringUTF16::operator =(const SonikStringUTF16& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const SonikString& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const SonikStringWIDE& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const SonikStringUTF8& t_his)
	{
		(*pImpl) = (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(SonikString&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF16& SonikStringUTF16::operator =(SonikStringWIDE&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF16& SonikStringUTF16::operator =(SonikStringUTF8&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};
	SonikStringUTF16& SonikStringUTF16::operator =(SonikStringUTF16&& _move_) noexcept
	{
		pImpl = _move_.pImpl;//ポインタコピー
		m_allocator = std::move(_move_.m_allocator); //アロケータムーヴ

		//ムーヴ元ポインタをnullptrに
		_move_.pImpl = nullptr;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const char* Str)
	{
		(*pImpl) = Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const char16_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const wchar_t* w_Str)
	{
		(*pImpl) = w_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const utf8_t* utf8_Str)
	{
		(*pImpl) = utf8_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const int8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const uint8_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const int16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const uint16_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const int32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const uint32_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const int64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const uint64_t SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const float SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator =(const double SetValue)
	{
		(*pImpl) = SetValue;

		return (*this);
	};

	//各入力フォーマットから現在のバッファに変換して結合します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringUTF16& SonikStringUTF16::operator +=(const SonikStringUTF16& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const SonikString& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const SonikStringWIDE& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const SonikStringUTF8& t_his)
	{
		(*pImpl) += (*(t_his.pImpl));

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const char* Str)
	{
		(*pImpl) += Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const char16_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const wchar_t* w_Str)
	{
		(*pImpl) += w_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const utf8_t* utf8_Str)
	{
		(*pImpl) += utf8_Str;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const int8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const uint8_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const int16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const uint16_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const int32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const uint32_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const int64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const uint64_t SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const float SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	SonikStringUTF16& SonikStringUTF16::operator +=(const double SetValue)
	{
		(*pImpl) += SetValue;

		return (*this);
	};

	//現在のバッファと入力バッファを結合し、別のオブジェクトとして返却します。
	//コピー元とコピー先が同じオブジェクトの場合、そのまま結合します。
	SonikStringUTF16 SonikStringUTF16::operator +(const SonikStringUTF16& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const SonikString& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const SonikStringWIDE& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const SonikStringUTF8& t_his)
	{
		SonikString str_;

		(*(str_.pImpl)) = (*pImpl) + (*(t_his.pImpl));

		return str_;
	};


	SonikStringUTF16 SonikStringUTF16::operator +(const char* Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + Str;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const char16_t* w_Str)
	{
		SonikStringWIDE str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const wchar_t* w_Str)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + w_Str;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const utf8_t* utf8_Str)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + utf8_Str;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const int8_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const uint8_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const int16_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const uint16_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const int32_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const uint32_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const int64_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const uint64_t SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const float SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	SonikStringUTF16 SonikStringUTF16::operator +(const double SetValue)
	{
		SonikStringUTF16 str_;

		(*(str_.pImpl)) = (*pImpl) + SetValue;

		return str_;
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 一致の場合true 不一致の場合 falseを返却します。
	bool SonikStringUTF16::operator ==(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator ==(const SonikString& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator ==(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator ==(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) == (*t_his.pImpl);
	};

	//c: 文字列同士を比較します。(strcmp)
	//c: 不一致の場合true　一致の場合 falseを返却します。
	bool SonikStringUTF16::operator !=(const SonikStringUTF16& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator !=(const SonikString& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator !=(const SonikStringWIDE& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	bool SonikStringUTF16::operator !=(const SonikStringUTF8& t_his) const
	{
		return (*pImpl) != (*t_his.pImpl);
	};

	//小なり
	bool SonikStringUTF16::operator <(const SonikStringUTF16& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <(const SonikString& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <(const SonikStringWIDE& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <(const SonikStringUTF8& _Greater_) const
	{
		return pImpl->Greater(_Greater_.pImpl->str_c());
	};

	//大なり
	bool SonikStringUTF16::operator >(const SonikStringUTF16& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >(const SonikString& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >(const SonikStringWIDE& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >(const SonikStringUTF8& _Less_) const
	{
		return pImpl->Less(_Less_.pImpl->str_c());
	};

	//小なりイコール
	bool SonikStringUTF16::operator <=(const SonikStringUTF16& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <=(const SonikString& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <=(const SonikStringWIDE& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator <=(const SonikStringUTF8& _GreaterEqual_) const
	{
		return pImpl->GreaterEqual(_GreaterEqual_.pImpl->str_c());
	};

	//大なりイコール
	bool SonikStringUTF16::operator >=(const SonikStringUTF16& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >=(const SonikString& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >=(const SonikStringWIDE& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

	bool SonikStringUTF16::operator >=(const SonikStringUTF8& _LessEqual_) const
	{
		return pImpl->LessEqual(_LessEqual_.pImpl->str_c());
	};

};