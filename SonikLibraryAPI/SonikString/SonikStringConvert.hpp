#pragma once

#ifndef SONIKString_SONIKStringCONVERT_H_
#define SONIKString_SONIKStringCONVERT_H_



#if defined(__GNUC__) || defined(__clang__)
	#define __SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__

	#if defined(__BMI__)
		#include <bmiintrin.h> //BMI bitfunction supported
		#undef ___SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__
		#define __SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__
	#endif

#elif defined(_MSC_VER) // MSVC でビットカウントを行うための関数
	#define __SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__
		#include <intrin.h>
		#include <immintrin.h>

	#if defined(__BMI__)
		#include <bmiintrin.h> //BMI bitfunction supported
		#undef __SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__
		#define __SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__
	#endif

#endif

#include <cstdlib>
#include <new>
#include <algorithm>
#include <clocale>

#include "./SonikStringEnums.h"
#include "../SmartPointer/SonikSmartPointer.hpp"
#include "../MathBit/MathBit.h"
#include "../SonikCAS/SonikAtomicLock.h"



namespace SonikConvStaticTable
{
	//UTF-8 FULLWIDKATAKANA => HALFWIDTH KATAKANA
	static const uint32_t HW_KANA_OFFSET_STARTPOINT[] = { 0xE382A1, 0xE38380 };
	//c:形式 => 左3Byte -> 実文字コード、 右1Byte -> 最上位bit = ポインタ進行数(0 or 1)  残り7bit = 下記配列の濁点位置。
	static const uint32_t HW_KANA[] = { 0xEFBDA700, 0xEFBDB100, 0xEFBDA800, 0xEFBDB200, 0xEFBDA900, 0xEFBDB300, 0xEFBDAA00, 0xEFBDB400, 0xEFBDAB00, 0xEFBDB500,
											0xEFBDB600, 0xEFBDB69F, 0xEFBDB700, 0xEFBDB79F, 0xEFBDB800, 0xEFBDB89F, 0xEFBDB900, 0xEFBDB99F, 0xEFBDBA00, 0xEFBDBA9F,
											0xEFBDBB00, 0xEFBDBB9F, 0xEFBDBC00, 0xEFBDBC9F, 0xEFBDBD00, 0xEFBDBD9F, 0xEFBDBE00, 0xEFBDBE9F, 0xEFBDBF00, 0xEFBDBF9F,
											0xEFBE8000,
											0xEFBE9E00, 0xEFBE9F00, //c: <- ". ゜ 31 32
											0xEFBE809F, 0xEFBE8100, 0xEFBE819F, 0xEFBDAF00, 0xEFBE8200, 0xEFBE829F, 0xEFBE8300, 0xEFBE839F, 0xEFBE8400, 0xEFBE849F,
											0xEFBE8500, 0xEFBE8600, 0xEFBE8700, 0xEFBE8800, 0xEFBE8900,
											0xEFBE8A00, 0xEFBE8A9F, 0xEFBE8AA0, 0xEFBE8B00, 0xEFBE8B9F, 0xEFBE8BA0, 0xEFBE8C00, 0xEFBE8C9F, 0xEFBE8CA0, 0xEFBE8D00, 0xEFBE8D9F, 0xEFBE8DA0, 0xEFBE8E00, 0xEFBE8E9F, 0xEFBE8EA0,
											0xEFBE8F00, 0xEFBE9000, 0xEFBE9100, 0xEFBE9200, 0xEFBE9300,
											0xEFBDAC00, 0xEFBE9400, 0xEFBDAD00, 0xEFBE9500, 0xEFBDAE00, 0xEFBE9600,
											0xEFBE9700, 0xEFBE9800, 0xEFBE9900, 0xEFBE9A00, 0xEFBE9B00,
											0xE383AE00, 0xEFBE9C00, 0xE383B000, 0xE383B100, 0xEFBDA600, 0xEFBE9D00,
											0xEFB9F39F, 0xE383B500, 0xE383B600, 0xEFBE9C9F, 0xE383B800, 0xE383B900, 0xEFBDA69F,
	};

};

namespace SonikConvStaticLocale
{
	class LocaleManagerSingleton
	{
	private:
		char* m_locale;
		SonikLib::S_CAS::SonikAtomicLock m_lock;

	private:
		//コンストラクタ
		LocaleManagerSingleton(void)
			:m_locale(nullptr)
		{
			const char* l_locale = std::setlocale(LC_CTYPE, "");

			uint64_t l_size = 0;

			if (l_locale != nullptr)
			{
				l_size = strlen(l_locale) + 1;//null文字分

				try
				{
					m_locale = new char[l_size];
					std::fill_n(m_locale, l_size, 0);

					memcpy(m_locale, l_locale, l_size);

				}
				catch (std::bad_alloc&)
				{
					//確保失敗。
					throw;
				};
			};
		};

		//コピー、ムーヴ禁止
		LocaleManagerSingleton(const LocaleManagerSingleton& _copy_) = delete;
		LocaleManagerSingleton(LocaleManagerSingleton&& _move_) = delete;
		LocaleManagerSingleton& operator =(const LocaleManagerSingleton& _copy_) = delete;
		LocaleManagerSingleton& operator = (LocaleManagerSingleton&& _move_) = delete;

	public:
		//デストラクタ
		~LocaleManagerSingleton(void)
		{
			if (m_locale != nullptr)
			{
				delete[] m_locale;
			};
		};

		//singleton accesser
		inline static LocaleManagerSingleton& instance(void)
		{
			static LocaleManagerSingleton __lms_obj__;

			return __lms_obj__;
		};

		inline const char* get_initialize_locale(void)
		{
			return m_locale;
		};

		inline void Lock_LocaleControl(void)
		{
			m_lock.lock();
		};

		inline void UnLock_LocaleControl(void)
		{
			m_lock.unlock();
		};

	};// end class LocaleManagerSingleton

	//グローバルでコールすることで初期化をする。
	//LocaleManagerSingleton& _global_ = LocaleManagerSingleton::instance();

};// end namespace SonikConvStaticLocale


namespace SonikLibStringConvert
{
	//ロケール定数からロケール名を取得します。
	inline void ConvertLocaleCharacter(uint32_t& _out_size_, char* _buffer_, SonikLibConvertLocale _locale_)
	{
		switch (_locale_)
		{
		case SonikLibConvertLocale::LC_DEFAULT_C:
			_out_size_ = static_cast<uint32_t>(strlen("C") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "C", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_JPN:
			_out_size_ = static_cast<uint32_t>(strlen("japanese_Japan.932") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "japanese_Japan.932", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_JPNUTF8:
			_out_size_ = static_cast<uint32_t>(strlen("japanese_Japan.UTF-8") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "japanese_Japan.UTF-8", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_ENGUS:
			_out_size_ = static_cast<uint32_t>(strlen("en_US") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "en_US", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_ENGGB:
			_out_size_ = static_cast<uint32_t>(strlen("en_GB") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "en_GB", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_FRNCE:
			_out_size_ = static_cast<uint32_t>(strlen("fr_FR") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "fr_FR", _out_size_);
			return;
			break;
		case SonikLibConvertLocale::LC_GERMANY:
			_out_size_ = static_cast<uint32_t>(strlen("de_DE") + 1);
			if (_buffer_ == nullptr)
			{
				return;
			};

			memcpy(_buffer_, "de_DE", _out_size_);
			return;
			break;

		default:
			_out_size_ = 0;
			return;
			break;
		};

	};

	//文字列のタイプをチェックします。
	//完全な判別は不可能（らしい）なので可能性の範囲でチェックされた結果を返却します。
	//2019/01/14 SJISの場合、utf-8が後ろに入っている場合もあるので、utf-8が出るまですべて読み込みます。
	inline SonikLibConvertType CheckConvertType(const char* pCheckSrc)
	{

		if (pCheckSrc == nullptr)
		{
			return SCHTYPE_UNKNOWN;
		};

		uint32_t Len = static_cast<uint32_t>(strlen(pCheckSrc));
		uint64_t lop = 0;
		unsigned char val;

		if (Len >= 3)
		{
			if (pCheckSrc[0] == 0xEF && pCheckSrc[1] == 0xBB && pCheckSrc[2] == 0xBF)
			{
				return SCHTYPE_BOMUTF8;
			};
		};

		SonikLibConvertType conv_t = SCHTYPE_UNKNOWN;
		unsigned char sjis_check_val;
		for (lop = 0; lop < Len; ++lop)
		{
			val = (*(pCheckSrc + lop));

			if (0x01 <= val && val <= 0x7E)
			{
				//ASCIIなのでSJIS判定
				conv_t = SCHTYPE_SJIS;
				continue;
				//			break;
			};

			if (0x80 <= val && val <= 0x9F)
			{
				//SJIS判定確定
				conv_t = SCHTYPE_SJIS;
				//			continue;
				break;
			};

			if (0xC2 <= val && val <= 0xDF)
			{

				//半角文字かもしれないので設定した上で読み飛ばし  0xa0(0xa1)～0xdf
				if (0xA0 <= val && val <= 0xDF)
				{
					conv_t = SCHTYPE_SJIS;
					continue;
				};

				if ((Len - (lop + 1)) == 0)
				{
					//ANSIなのでSJISともUTF8とも取れるので、UTF8として解釈
					conv_t = SCHTYPE_UTF8;
					break;
				};

				if ((Len - (lop + 1)) < 2)
				{
					//第２バイトにSJISの第１範囲が来ても最終文字なのでSJISの可能性は無い。UTF-8
					conv_t = SCHTYPE_UTF8;
					break;
				};

				sjis_check_val = static_cast<unsigned char>((*(pCheckSrc + (lop + 1))));
				if (0x80 <= sjis_check_val && sjis_check_val <= 0x9F)
				{

					sjis_check_val = static_cast<unsigned char>((*(pCheckSrc + (lop + 2))));
					if (0xE0 <= sjis_check_val && sjis_check_val <= 0xEF)
					{
						//SJIS確定
						conv_t = SCHTYPE_SJIS;
						break;
					};

					++lop;
				};

				//全部違えばUTF-8
				conv_t = SCHTYPE_UTF8;
				break;
			};


			if (0xE0 <= val && val <= 0xEF)
			{
				if (lop > 0)
				{
					sjis_check_val = static_cast<unsigned char>((*(pCheckSrc + (lop - 1))));
					if (sjis_check_val >= 0x80 && sjis_check_val <= 0x9F)
					{
						//SJISの組み合わせ
						conv_t = SCHTYPE_SJIS;
						continue;
					};
				};

				//可能性的にはUTF8の確率が高いため、UTF8と判断
				conv_t = SCHTYPE_UTF8;
				break;
			};



			if (0xA0 <= val && val <= 0xC1)
			{
				//SJIS判定
				conv_t = SCHTYPE_SJIS;
				continue;
			};


			if (0xF0 <= val && val <= 0xFD)
			{
				//UTF8確定
				conv_t = SCHTYPE_UTF8;
				break;
			};


			//その他よくわからない部分は読み飛ばし。
		};

		//返却
		return conv_t;
	};

	//文字列のタイプをチェックし、その文字列で使用されているバイト数を返却します。
	//文字列のタイプチェックは可能性の範囲のチェックとなります。
	//SJIS, UTF-8のみに対応。
	inline uint64_t GetStringLengthByte(const char* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		SonikLibConvertType Type_ = CheckConvertType(pCheckSrc);
		uint64_t cnt = 0;

		if (Type_ == SCHTYPE_SJIS || Type_ == SCHTYPE_UTF8)
		{
			while ((*pCheckSrc) != 0)
			{
				++cnt;
				++pCheckSrc;
			};

			return cnt;
		};

		//判定できなかったのでカウントできない。
		return 0;

	};
	//unicode文字の文字列バイト数を返却します。
	inline uint64_t GetStringLengthByte(const char16_t* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		uint64_t cnt = 0;

		while ((*pCheckSrc) != 0)
		{
			cnt += 2;
			++pCheckSrc;
		};

		return cnt;
	};
	//unicode(UTF-32)文字の文字列バイト数を返却します。
	//ただのループ。
	inline uint64_t GetStringLengthByte(const char32_t* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		uint64_t cnt_ = 0;

		while ((*pCheckSrc) != 0)
		{
			cnt_ += 4;
			++pCheckSrc;
		};

		return cnt_;
	};


	//文字列のタイプをチェックし、その文字列で使用されている文字数を返却します。
	//文字列のタイプチェックは可能性の範囲のチェックとなります。
	//SJIS, UTF-8のみに対応。
	inline uint64_t GetStringCount(const char* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		SonikLibConvertType Type_ = CheckConvertType(pCheckSrc);
		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		unsigned char swapbit = 0;
		const unsigned char* pchecsrc_val = reinterpret_cast<const unsigned char*>(pCheckSrc);

		if (Type_ == SCHTYPE_SJIS)
		{
			while ((*pchecsrc_val) != 0x00)
			{

				if ((0x80 <= (*pchecsrc_val) && (*pchecsrc_val) <= 0x9F) || (0xE0 <= (*pchecsrc_val) && (*pchecsrc_val) <= 0xFC))
				{

					++pchecsrc_val;
				};

				++cnt;
				++pchecsrc_val;
			};

		}
		else if (Type_ == SCHTYPE_UTF8)
		{
			while ((*pchecsrc_val) != 0x00)
			{
				/*
				swapbit = SonikMathBit::BitSwap((*pchecsrc_val));

				bitcnt = ~(swapbit);

				bitcnt |= (bitcnt << 1);
				bitcnt |= (bitcnt << 2);
				bitcnt |= (bitcnt << 4);
				bitcnt |= (bitcnt << 8);
				bitcnt |= (bitcnt << 16);
				bitcnt |= (bitcnt << 32);

				bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
				bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
				bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
				bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
				bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

				bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);
				bitcnt = (64 - bitcnt);
				*/
				swapbit = (*pchecsrc_val) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
				bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
				bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
				bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
				//黒魔術ビット操作
				swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
				swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

				bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

				if (bitcnt == 0)
				{
					++bitcnt;
				};

				++cnt;
				pchecsrc_val += bitcnt;

			};

		}
		else
		{
			return 0;
		};

		return cnt;
	};

	//第２引数に指定したタイプで強制的にカウントを行います。(SJIS混じりUTF8の対処)
	inline uint64_t GetStringCount(const char* pCheckSrc, SonikLibConvertType CountStrType)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		if (CountStrType != SCHTYPE_SJIS && CountStrType != SCHTYPE_UTF8)
		{
			return 0;
		};

		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		unsigned char swapbit = 0;
		const unsigned char* pchecsrc_val = reinterpret_cast<const unsigned char*>(pCheckSrc);

		if (CountStrType == SCHTYPE_SJIS)
		{
			while ((*pchecsrc_val) != 0x00)
			{
				if ((0x80 <= (*pchecsrc_val) && (*pchecsrc_val) <= 0x9F) || (0xE0 <= (*pchecsrc_val) && (*pchecsrc_val) <= 0xFC))
				{

					++pchecsrc_val;
				};

				++cnt;
				++pchecsrc_val;
			};

		}
		else if (CountStrType == SCHTYPE_UTF8)
		{
			while ((*pchecsrc_val) != 0x00)
			{
				/*
				swapbit = SonikMathBit::BitSwap((*pchecsrc_val));

				bitcnt = ~(swapbit);

				bitcnt |= (bitcnt << 1);
				bitcnt |= (bitcnt << 2);
				bitcnt |= (bitcnt << 4);
				bitcnt |= (bitcnt << 8);
				bitcnt |= (bitcnt << 16);
				bitcnt |= (bitcnt << 32);

				bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
				bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
				bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
				bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
				bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

				bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);

				bitcnt = (64 - bitcnt);
				*/

				swapbit = (*pchecsrc_val) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
				bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
				bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
				bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
				//黒魔術ビット操作
				swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
				swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

				bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif


				if (bitcnt == 0)
				{
					++bitcnt;
				};

				++cnt;
				pchecsrc_val += bitcnt;

			};

		}
		else
		{
			return 0;
		};

		return cnt;
	};

	//unicode(UTF-16)文字の文字数を返却します。
	inline uint64_t GetStringCount(const char16_t* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		uint64_t cnt = 0;

		while ((*pCheckSrc) != 0)
		{
			if ((*pCheckSrc & 0xFC00) == 0xD800) //サロゲートペアチェック
			{
				if ((pCheckSrc[1] & 0xFC00) == 0xDC00)
				{
					//１文字分読み飛ばし。
					++pCheckSrc;
				};
			};
			++cnt;
			++pCheckSrc;
		};

		return cnt;
	};

	//unicode(UTF-32)文字の文字数を返却します。
	inline uint64_t GetStringCount(const char32_t* pCheckSrc)
	{
		if (pCheckSrc == nullptr)
		{
			return 0;
		};

		uint64_t cnt_ = 0;

		while ((*pCheckSrc) != 0)
		{
			++cnt_;
			++pCheckSrc;
		};

		return cnt_;
	};


	//UTF-8をUNICODE(UTF-32)に変換します。
	inline bool ConvertUTF8ToUTF32(const utf8_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント。(NULL文字は含まない。)
		uint64_t SrcLen = GetStringCount(reinterpret_cast<const char*>(pSrc)) + 1; //カウントにNULL文字が含まれないため、NULL文字分として+1文字追加

		//UTF32 は4Byte固定長なので、文字数 = 必要サイズ。
		//Byte数で返すので. １文字当たり4Byte で 文字数 * 4 する。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = ((SrcLen * 1) << 2); //Byte数で返却
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		char32_t* pUtf32_ = 0;

		pUtf32_ = new(std::nothrow) char32_t[SrcLen] {}; //初期化時0クリア
		if (pUtf32_ == nullptr)
		{
			return false;
		}

		uint64_t utf8_i = 0;
		uint64_t utf32_i = 0;
		unsigned char val = 0;
		uint64_t numBytes = 0;
		uint64_t offset_ = 0;
		const unsigned char* control_src = reinterpret_cast<const unsigned char*>(pSrc);
		uint64_t lopsize = SrcLen - 1; 	//オフセットにNULL文字はいらないため、NULL文字分サイズを調整

		for (offset_ = 0; offset_ < lopsize;/*省略*/)
		{

			control_src += utf8_i;

			val = (*control_src);
			numBytes = 0;

			//utfのバイトセットチェック。
			if (0 <= val && val < 0x80)
			{
				numBytes = 1;

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src));

			}
			else if (0xC2 <= val && val < 0xE0)
			{
				numBytes = 2;

				if (!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0))
				{
					delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x1E) == 0)
				{
					delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = (static_cast<uint32_t>((*control_src) & 0x1F)) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F);

			}
			else if (0xE0 <= val && val < 0xF0)
			{
				numBytes = 3;

				if ((!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 2)) && (*(control_src + 2)) < 0xC0))
					)
				{
					delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x0F) == 0 &&
					((*(control_src + 1)) & 0x20) == 0)
				{
					delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src) & 0x0F) << 12;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 2)) & 0x3F);

			}
			else if (0xF0 <= val && val < 0xF8)
			{
				numBytes = 4;

				if ((!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 2)) && (*(control_src + 2)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 3)) && (*(control_src + 3)) < 0xC0)))
				{
					delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x07) == 0 &&
					((*(control_src + 1)) & 0x30) == 0)
				{
					delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src) & 0x07) << 18;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F) << 12;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 2)) & 0x3F) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 3)) & 0x3F);

			};

			if (numBytes == 0)
			{
				delete[] pUtf32_;
				return false;
			};

			control_src -= utf8_i;

			utf8_i += numBytes;
			++offset_;
			++utf32_i;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pUtf32_, SrcLen, pDest);
#else
			std::memcpy(pDest, pUtf32_, (SrcLen << 2));
#endif

		}
		catch (...)
		{
			delete[] pUtf32_;
			return false;
		};

		delete[] pUtf32_;
		return true;
	};
	inline bool ConvertUTF8ToUTF32(const utf8_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント。(NULL文字は含まない。)
		uint64_t SrcLen = GetStringCount(reinterpret_cast<const char*>(pSrc)) + 1; //カウントにNULL文字が含まれないため、NULL文字分として+1文字追加

		//UTF32 は4Byte固定長なので、文字数 = 必要サイズ。
		//Byte数で返すので. １文字当たり4Byte で 文字数 * 4 する。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = ((SrcLen * 1) << 2); //Byte数で返却
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		char32_t* pUtf32_ = nullptr;

		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * SrcLen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		pUtf32_ = new(l_allocbuffer) char32_t[SrcLen] {}; //初期化時0クリア


		uint64_t utf8_i = 0;
		uint64_t utf32_i = 0;
		unsigned char val = 0;
		uint64_t numBytes = 0;
		uint64_t offset_ = 0;
		const unsigned char* control_src = reinterpret_cast<const unsigned char*>(pSrc);
		uint64_t lopsize = SrcLen - 1; 	//オフセットにNULL文字はいらないため、NULL文字分サイズを調整

		for (offset_ = 0; offset_ < lopsize;/*省略*/)
		{

			control_src += utf8_i;

			val = (*control_src);
			numBytes = 0;

			//utfのバイトセットチェック。
			if (0 <= val && val < 0x80)
			{
				numBytes = 1;

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src));

			}
			else if (0xC2 <= val && val < 0xE0)
			{
				numBytes = 2;

				if (!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0))
				{
					_allocator_->memdel(pUtf32_);
					//delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x1E) == 0)
				{
					_allocator_->memdel(pUtf32_);
					//delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = (static_cast<uint32_t>((*control_src) & 0x1F)) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F);

			}
			else if (0xE0 <= val && val < 0xF0)
			{
				numBytes = 3;

				if ((!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 2)) && (*(control_src + 2)) < 0xC0))
					)
				{
					_allocator_->memdel(pUtf32_);
					//				delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x0F) == 0 &&
					((*(control_src + 1)) & 0x20) == 0)
				{
					_allocator_->memdel(pUtf32_);
					//				delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src) & 0x0F) << 12;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 2)) & 0x3F);

			}
			else if (0xF0 <= val && val < 0xF8)
			{
				numBytes = 4;

				if ((!(0x80 <= (*(control_src + 1)) && (*(control_src + 1)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 2)) && (*(control_src + 2)) < 0xC0)) ||
					(!(0x80 <= (*(control_src + 3)) && (*(control_src + 3)) < 0xC0)))
				{
					_allocator_->memdel(pUtf32_);
					//delete[] pUtf32_;
					return false;
				};
				if (((*control_src) & 0x07) == 0 &&
					((*(control_src + 1)) & 0x30) == 0)
				{
					_allocator_->memdel(pUtf32_);
					//delete[] pUtf32_;
					return false;
				};

				pUtf32_[utf32_i] = static_cast<uint32_t>((*control_src) & 0x07) << 18;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 1)) & 0x3F) << 12;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 2)) & 0x3F) << 6;
				pUtf32_[utf32_i] |= static_cast<uint32_t>((*(control_src + 3)) & 0x3F);

			};

			if (numBytes == 0)
			{
				_allocator_->memdel(pUtf32_);
				//delete[] pUtf32_;
				return false;
			};

			control_src -= utf8_i;

			utf8_i += numBytes;
			++offset_;
			++utf32_i;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pUtf32_, SrcLen, pDest);
#else
			std::memcpy(pDest, pUtf32_, (SrcLen << 2));
#endif

		}
		catch (...)
		{
			_allocator_->memdel(pUtf32_);
			//delete[] pUtf32_;
			return false;
		};

		_allocator_->memdel(pUtf32_);
		//delete[] pUtf32_;
		return true;
	};

	//UNICODE(UTF-32)をUTF-8に変換します。
	inline bool ConvertUTF32ToUTF8(const char32_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t utf32_strcnt_ = 0;
		uint64_t utf8_strcnt_ = 0;
		//文字数カウント
		const char32_t* tmpsrc = pSrc;
		while ((*tmpsrc) != 0)
		{
			++tmpsrc;
			++utf32_strcnt_;
		};

		// x * 4 = x << 2; 終端文字は１Byteでいいので +1 Byte
		utf8_strcnt_ = (utf32_strcnt_ << 2) + 1;

		utf8_t* utf8buffer = new(std::nothrow) utf8_t[utf8_strcnt_]{}; //初期化時0クリア
		if (utf8buffer == nullptr)
		{
			return false;
		};

		unsigned char* p_utf8offset = reinterpret_cast<unsigned char*>(utf8buffer);

		tmpsrc = pSrc;
		uint64_t CopySize = 0;

		for (uint32_t i = 0; i < utf32_strcnt_; ++i)
		{
			if ((*tmpsrc) > 0x10FFFF)
			{
				delete[] utf8buffer;
				return false;
			};

			if ((*tmpsrc) < 128)
			{
				(*p_utf8offset) = static_cast<unsigned char>((*tmpsrc));
				++p_utf8offset;

				++CopySize;
			}
			else if ((*tmpsrc) < 2048)
			{
				(*p_utf8offset) = 0xC0 | static_cast<unsigned char>(((*tmpsrc) >> 6));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 2;
			}
			else if ((*tmpsrc) < 65536)
			{
				(*p_utf8offset) = 0xE0 | static_cast<unsigned char>(((*tmpsrc) >> 12));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>((((*tmpsrc) >> 6) & 0x3F));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 3;
			}
			else
			{
				(*p_utf8offset) = 0xF0 | static_cast<unsigned char>(((*tmpsrc) >> 18));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) >> 12) & 0x3F);
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) >> 6) & 0x3F);
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 4;
			};

			++tmpsrc;
		};

		//BestFitサイズ返却(Byte数単位
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize + 1; //Null文字分追加
			delete[] utf8buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] utf8buffer;
			return false;
		};


		try
		{
#if defined(_S_STRING_COPY_STL)
			//d::copy_n(utf8buffer, CopySize, pDest);
#else
			std::memcpy(pDest, utf8buffer, CopySize);
#endif

		}
		catch (...)
		{
			delete[] utf8buffer;
			return false;
		};

		delete[] utf8buffer;
		return true;
	};
	inline bool ConvertUTF32ToUTF8(const char32_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t utf32_strcnt_ = 0;
		uint64_t utf8_strcnt_ = 0;
		//文字数カウント
		const char32_t* tmpsrc = pSrc;
		while ((*tmpsrc) != 0)
		{
			++tmpsrc;
			++utf32_strcnt_;
		};

		// x * 4 = x << 2; 終端文字は１Byteでいいので +1 Byte
		utf8_strcnt_ = (utf32_strcnt_ << 2) + 1;

		void* l_allocbuffer = _allocator_->memal(sizeof(utf8_t) * utf8_strcnt_);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		utf8_t* utf8buffer = new(l_allocbuffer) utf8_t[utf8_strcnt_]{}; //初期化時0クリア


		unsigned char* p_utf8offset = reinterpret_cast<unsigned char*>(utf8buffer);

		tmpsrc = pSrc;
		uint64_t CopySize = 0;

		for (uint32_t i = 0; i < utf32_strcnt_; ++i)
		{
			if ((*tmpsrc) > 0x10FFFF)
			{
				_allocator_->memdel(utf8buffer);
				//delete[] utf8buffer;
				return false;
			};

			if ((*tmpsrc) < 128)
			{
				(*p_utf8offset) = static_cast<unsigned char>((*tmpsrc));
				++p_utf8offset;

				++CopySize;
			}
			else if ((*tmpsrc) < 2048)
			{
				(*p_utf8offset) = 0xC0 | static_cast<unsigned char>(((*tmpsrc) >> 6));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 2;
			}
			else if ((*tmpsrc) < 65536)
			{
				(*p_utf8offset) = 0xE0 | static_cast<unsigned char>(((*tmpsrc) >> 12));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>((((*tmpsrc) >> 6) & 0x3F));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 3;
			}
			else
			{
				(*p_utf8offset) = 0xF0 | static_cast<unsigned char>(((*tmpsrc) >> 18));
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) >> 12) & 0x3F);
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) >> 6) & 0x3F);
				++p_utf8offset;
				(*p_utf8offset) = 0x80 | static_cast<unsigned char>(((*tmpsrc) & 0x3F));
				++p_utf8offset;

				CopySize += 4;
			};

			++tmpsrc;
		};

		//BestFitサイズ返却(Byte数単位
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize + 1; //Null文字分追加

			_allocator_->memdel(utf8buffer);
			//delete[] utf8buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(utf8buffer);
			//delete[] utf8buffer;
			return false;
		};


		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf8buffer, CopySize, pDest);
#else
			std::memcpy(pDest, utf8buffer, CopySize);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(utf8buffer);
			//delete[] utf8buffer;
			return false;
		};

		_allocator_->memdel(utf8buffer);
		//delete[] utf8buffer;
		return true;

	};

	//UNICODE(UTF-32)をUNICODE(UTF-16)に変換します。
	inline bool ConvertUTF32ToUTF16(const char32_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント
		uint64_t srclen_ = GetStringCount(pSrc);

		//事前に確保する設定上1文字=4Byte必要で計算する。UTF16は最大2Byte x 2Byteの4Byteで構成される。
		uint64_t wsrclen_ = (srclen_ + 1) << 1;  // ( x + null文字 ) * 2 -> char16_t型で配列を作るので、これで2Byte文字列 * 2の配列ができる = １文字最大4Byteで計算している。

		char16_t* utf16buffer = new(std::nothrow) char16_t[wsrclen_] {}; //初期化時0クリア
		if (utf16buffer == nullptr)
		{
			return false;
		};

		const char32_t* utf32_Src = pSrc;
		char16_t* p_cont_utf16buffer = utf16buffer;
		uint64_t CopySize = 0; //UTF16で使う要素数
		for (uint64_t i = 0; i < srclen_; ++i)
		{
			if (utf32_Src[i] == 0 || utf32_Src[i] > 0x10FFFF)
			{
				delete[] utf16buffer;
				return false;
			};

			if (utf32_Src[i] < 0x10000)
			{
				(*p_cont_utf16buffer) = static_cast<uint16_t>(utf32_Src[i]);
				++p_cont_utf16buffer;

				++CopySize;
			}
			else
			{

				(*p_cont_utf16buffer) = static_cast<uint16_t>(((utf32_Src[i] - 0x10000) >> 10) + 0xD800); // x / 0x400 = x / 1024 = x >> 10
				++p_cont_utf16buffer;
				(*p_cont_utf16buffer) = static_cast<uint16_t>(((utf32_Src[i] - 0x10000) & 0x3FF) + 0xDC00); // x % 0x400 = x % 1024 = x & 1023 = x & (0x400 - 1) = x & 0x3FF
				++p_cont_utf16buffer;

				CopySize += 2;
			};

		};

		//サイズ返却(Byte数で返却)
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = ((CopySize + 1) << 1); //(要素数 + Null文字用の要素) * 2 で使用Byte数
			delete[] utf16buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] utf16buffer;
			return false;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf16buffer, CopySize, pDest);
#else
			std::memcpy(pDest, utf16buffer, (CopySize << 1));
#endif

		}
		catch (...)
		{
			delete[] utf16buffer;
			return false;
		};

		delete[] utf16buffer;
		return true;
	};
	inline bool ConvertUTF32ToUTF16(const char32_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント
		uint64_t srclen_ = GetStringCount(pSrc);

		//事前に確保する設定上1文字=4Byte必要で計算する。UTF16は最大2Byte x 2Byteの4Byteで構成される。
		uint64_t wsrclen_ = (srclen_ + 1) << 1;  // ( x + null文字 ) * 2 -> char16_t型で配列を作るので、これで2Byte文字列 * 2の配列ができる = １文字最大4Byteで計算している。

		void* l_allocbuffer = _allocator_->memal(sizeof(char16_t) * wsrclen_);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char16_t* utf16buffer = new(l_allocbuffer) char16_t[wsrclen_] {}; //初期化時0クリア


		const char32_t* utf32_Src = pSrc;
		char16_t* p_cont_utf16buffer = utf16buffer;
		uint64_t CopySize = 0; //UTF16で使う要素数
		for (uint64_t i = 0; i < srclen_; ++i)
		{
			if (utf32_Src[i] == 0 || utf32_Src[i] > 0x10FFFF)
			{
				_allocator_->memdel(utf16buffer);
				//delete[] utf16buffer;
				return false;
			};

			if (utf32_Src[i] < 0x10000)
			{
				(*p_cont_utf16buffer) = static_cast<uint16_t>(utf32_Src[i]);
				++p_cont_utf16buffer;

				++CopySize;
			}
			else
			{

				(*p_cont_utf16buffer) = static_cast<uint16_t>(((utf32_Src[i] - 0x10000) >> 10) + 0xD800); // x / 0x400 = x / 1024 = x >> 10
				++p_cont_utf16buffer;
				(*p_cont_utf16buffer) = static_cast<uint16_t>(((utf32_Src[i] - 0x10000) & 0x3FF) + 0xDC00); // x % 0x400 = x % 1024 = x & 1023 = x & (0x400 - 1) = x & 0x3FF
				++p_cont_utf16buffer;

				CopySize += 2;
			};

		};

		//サイズ返却(Byte数で返却)
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = ((CopySize + 1) << 1); //(要素数 + Null文字用の要素) * 2 で使用Byte数

			_allocator_->memdel(utf16buffer);
			//delete[] utf16buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(utf16buffer);
			//delete[] utf16buffer;
			return false;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf16buffer, CopySize, pDest);
#else
			std::memcpy(pDest, utf16buffer, (CopySize << 1));
#endif

		}
		catch (...)
		{
			_allocator_->memdel(utf16buffer);
			//delete[] utf16buffer;
			return false;
		};

		_allocator_->memdel(utf16buffer);
		//delete[] utf16buffer;
		return true;
	};

	//UNICODE(UTF-16)をUNICODE(UTF-32)に変換します。
	inline bool ConvertUTF16ToUTF32(const char16_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント
		uint64_t srclen_ = GetStringCount(pSrc);

		//サイズ返却。Byte数で返却
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = (srclen_ + 1) << 2; //(要素数＋Null文字要素) * 4(１要素当たりのByte数) = 総使用Byte数
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		char32_t* utf32buffer = new(std::nothrow) char32_t[srclen_ + 1]{}; //初期化時0クリア
		if (utf32buffer == nullptr)
		{
			return false;
		};

		//配列組み換え
		const char16_t* utf16buffer = pSrc;

		for (uint64_t i = 0; i < srclen_; ++i)
		{
			if (0xD800 <= (*utf16buffer) && (*utf16buffer) < 0xDC00)
			{
				if (0xDC00 <= (*(utf16buffer + 1)))
				{
					utf32buffer[i] = 0x10000 + ((*utf16buffer) - 0xD800) * 0x400 + ((*(utf16buffer + 1)) - 0xDC00);

					utf16buffer += 2;

				}
				else if ((*(utf16buffer + 1)) == 0)
				{
					utf32buffer[i] = (*utf16buffer);

					utf16buffer += 1;

				}
				else
				{
					delete[] utf32buffer;
					return false;
				};

			}
			else if (0xDC00 <= (*utf16buffer) && (*utf16buffer) < 0xE000)
			{
				if ((*(utf16buffer + 1)) == 0)
				{
					utf32buffer[i] = (*utf16buffer);

					utf16buffer += 1;

				}
				else
				{
					delete[] utf32buffer;
					return false;
				};

			}
			else
			{
				utf32buffer[i] = (*utf16buffer);

				utf16buffer += 1;

			};

		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf32buffer, srclen_, pDest);
#else
			std::memcpy(pDest, utf32buffer, (srclen_ << 2));
#endif

		}
		catch (...)
		{
			delete[] utf32buffer;
			return false;
		};


		delete[] utf32buffer;
		return true;
	};
	inline bool ConvertUTF16ToUTF32(const char16_t* pSrc, char32_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		//文字数カウント
		uint64_t srclen_ = GetStringCount(pSrc);

		//サイズ返却。Byte数で返却
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = (srclen_ + 1) << 2; //(要素数＋Null文字要素) * 4(１要素当たりのByte数) = 総使用Byte数
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		void* l_allocbuffer = _allocator_->memal((sizeof(char32_t) * (srclen_ + 1)));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* utf32buffer = new(l_allocbuffer) char32_t[srclen_ + 1]{}; //初期化時0クリア


		//配列組み換え
		const char16_t* utf16buffer = pSrc;

		for (uint64_t i = 0; i < srclen_; ++i)
		{
			if (0xD800 <= (*utf16buffer) && (*utf16buffer) < 0xDC00)
			{
				if (0xDC00 <= (*(utf16buffer + 1)))
				{
					utf32buffer[i] = 0x10000 + ((*utf16buffer) - 0xD800) * 0x400 + ((*(utf16buffer + 1)) - 0xDC00);

					utf16buffer += 2;

				}
				else if ((*(utf16buffer + 1)) == 0)
				{
					utf32buffer[i] = (*utf16buffer);

					utf16buffer += 1;

				}
				else
				{
					_allocator_->memdel(utf32buffer);
					//delete[] utf32buffer;
					return false;
				};

			}
			else if (0xDC00 <= (*utf16buffer) && (*utf16buffer) < 0xE000)
			{
				if ((*(utf16buffer + 1)) == 0)
				{
					utf32buffer[i] = (*utf16buffer);

					utf16buffer += 1;

				}
				else
				{
					_allocator_->memdel(utf32buffer);
					//delete[] utf32buffer;
					return false;
				};

			}
			else
			{
				utf32buffer[i] = (*utf16buffer);

				utf16buffer += 1;

			};

		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(utf32buffer, srclen_, pDest);
#else
			std::memcpy(pDest, utf32buffer, (srclen_ << 2));
#endif

		}
		catch (...)
		{
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		_allocator_->memdel(utf32buffer);
		//delete[] utf32buffer;
		return true;
	};

	//UNICODE(UTF-16)をUTF8に変換します。
	inline bool ConvertUTF16ToUTF8(const char16_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t buffersize = 0;

		//null文字を含めたByte数で帰って来る
		ConvertUTF16ToUTF32(pSrc, nullptr, &buffersize);

		//Byte数から要素数を算出
		buffersize >>= 2; //UTF32は4Byteブロックのため、使用Byte数 / 4 = 要素数
		char32_t* utf32buffer = new(std::nothrow) char32_t[buffersize] {}; //初期化時0クリア
		if (utf32buffer == nullptr)
		{
			return false;
		};

		if (!ConvertUTF16ToUTF32(pSrc, utf32buffer, nullptr))
		{
			delete[] utf32buffer;
			return false;
		};

		//null文字を含めたByte数で帰って来る
		ConvertUTF32ToUTF8(utf32buffer, nullptr, &buffersize);

		//UTF8は1ByteブロックのためByte数 = 要素数。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = buffersize;
			delete[] utf32buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] utf32buffer;
			return false;
		};

		if (!ConvertUTF32ToUTF8(utf32buffer, pDest, nullptr))
		{
			delete[] utf32buffer;
			return false;
		};

		delete[] utf32buffer;
		return true;
	};
	inline bool ConvertUTF16ToUTF8(const char16_t* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t buffersize = 0;

		//null文字を含めたByte数で帰って来る
		ConvertUTF16ToUTF32(pSrc, nullptr, &buffersize, _allocator_);

		//Byte数から要素数を算出
		buffersize >>= 2; //UTF32は4Byteブロックのため、使用Byte数 / 4 = 要素数

		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * buffersize);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* utf32buffer = new(l_allocbuffer) char32_t[buffersize] {}; //初期化時0クリア

		if (!ConvertUTF16ToUTF32(pSrc, utf32buffer, nullptr, _allocator_))
		{
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		//null文字を含めたByte数で帰って来る
		ConvertUTF32ToUTF8(utf32buffer, nullptr, &buffersize, _allocator_);

		//UTF8は1ByteブロックのためByte数 = 要素数。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = buffersize;

			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		if (!ConvertUTF32ToUTF8(utf32buffer, pDest, nullptr, _allocator_))
		{
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		_allocator_->memdel(utf32buffer);
		//delete[] utf32buffer;
		return true;
	};

	//UTF8をUNICODE(UTF-16)に変換します。
	inline bool ConvertUTF8ToUTF16(const utf8_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t buffersize = 0;

		//null文字を含めたByte数で帰って来る
		ConvertUTF8ToUTF32(pSrc, nullptr, &buffersize);
		//Byte数から要素数を算出
		buffersize >>= 2; //UTF32は4Byteブロックのため、使用Byte数 / 4 = 要素数

		char32_t* utf32buffer = new(std::nothrow) char32_t[buffersize] {}; //初期化時0クリア
		if (utf32buffer == nullptr)
		{
			return false;
		};

		if (!ConvertUTF8ToUTF32(pSrc, utf32buffer, nullptr))
		{
			delete[] utf32buffer;
			return false;
		};

		//null文字を含めたByte数で帰って来る
		ConvertUTF32ToUTF16(utf32buffer, nullptr, &buffersize);

		//Byte数から要素数を算出
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = buffersize; //Byte数で返却
			delete[] utf32buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] utf32buffer;
			return false;
		};

		if (!ConvertUTF32ToUTF16(utf32buffer, pDest, nullptr))
		{
			delete[] utf32buffer;
			return false;
		};

		delete[] utf32buffer;
		return true;
	};
	inline bool ConvertUTF8ToUTF16(const utf8_t* pSrc, char16_t* pDest, uint64_t* DestBufferSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t buffersize = 0;

		//null文字を含めたByte数で帰って来る
		ConvertUTF8ToUTF32(pSrc, nullptr, &buffersize, _allocator_);
		//Byte数から要素数を算出
		buffersize >>= 2; //UTF32は4Byteブロックのため、使用Byte数 / 4 = 要素数

		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * buffersize);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* utf32buffer = new(l_allocbuffer) char32_t[buffersize] {}; //初期化時0クリア


		if (!ConvertUTF8ToUTF32(pSrc, utf32buffer, nullptr, _allocator_))
		{
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		//null文字を含めたByte数で帰って来る
		ConvertUTF32ToUTF16(utf32buffer, nullptr, &buffersize, _allocator_);

		//Byte数から要素数を算出
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = buffersize; //Byte数で返却

			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		if (!ConvertUTF32ToUTF16(utf32buffer, pDest, nullptr, _allocator_))
		{
			_allocator_->memdel(utf32buffer);
			//delete[] utf32buffer;
			return false;
		};

		_allocator_->memdel(utf32buffer);
		//delete[] utf32buffer;
		return true;
	};

	//マルチバイト文字列をUTF8文字列に変換します。
	//第１引数の文字列は、可能性の判定として、SJIS判定であれば処理を行います。
	inline bool ConvertMBSToUTF8(const char* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcsLen = GetStringCount(pSrc) + 1; //null文字分で+1文字追加

		//utf16でも32でもサロゲートペアを考慮した場合4Byteの領域で取らないと十分な量とならないので、
		//char32_tで領域確保し、wchar_tのサイズに応じて処理を変更する。
		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcsLen] {}; //初期化時0クリア
		if (wcsbuffer == nullptr)
		{
			SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
			return false;
		};

		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcsLen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcsLen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//wchar_tのサイズによって変換経路を変える。
#if WCHAR_MAX <= 0xffffU
	//wchar_t = 2Byte
	//UTF16 -> UTF8
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			//utf16->utf8 サイズチェック
			ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(wcsbuffer), nullptr, DestBufferSize); // バイト数で返却
			delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] wcsbuffer;
			return false;
		};

		//utf16->utf8 本番
		if (!ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(wcsbuffer), pDest, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#else
	//wchar_t = 4Byte
	//UTF32->UTF8
		if (pDest == nullptr && DestBufferSize != nullptr)
		{
			//utf16->utf8 サイズチェック
			ConvertUTF32ToUTF8(wcsbuffer, nullptr, DestBufferSize); // バイト数で返却
			delete[] wcsbuffer;
			return false;
		};

		//utf32->utf8本番
		if (!ConvertUTF32ToUTF8(wcsbuffer, pDest, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#endif

		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertMBSToUTF8(const char* pSrc, utf8_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcsLen = GetStringCount(pSrc) + 1; //null文字分で+1文字追加

		//utf16でも32でもサロゲートペアを考慮した場合4Byteの領域で取らないと十分な量とならないので、
		//char32_tで領域確保し、wchar_tのサイズに応じて処理を変更する。
		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcsLen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcsLen] {}; //初期化時0クリア


		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcsLen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcsLen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				_allocator_->memdel(wcsbuffer);
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//wchar_tのサイズによって変換経路を変える。
#if WCHAR_MAX <= 0xffffU
	//wchar_t = 2Byte
	//UTF16 -> UTF8
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			//utf16->utf8 サイズチェック
			ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(wcsbuffer), nullptr, DestBufferSize, _allocator_); // バイト数で返却

			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		//utf16->utf8 本番
		if (!ConvertUTF16ToUTF8(reinterpret_cast<char16_t*>(wcsbuffer), pDest, nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#else
	//wchar_t = 4Byte
	//UTF32->UTF8
		if (pDest == nullptr && DestBufferSize != nullptr)
		{
			//utf16->utf8 サイズチェック
			ConvertUTF32ToUTF8(wcsbuffer, nullptr, DestBufferSize, _allocator_); // バイト数で返却

			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		//utf32->utf8本番
		if (!ConvertUTF32ToUTF8(wcsbuffer, pDest, nullptr, _allocaor_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#endif

		_allocator_->memdel(wcsbuffer);
		//delete[] wcsbuffer;
		return true;
	};

	//UTF8文字列をマルチバイト文字列に変換します。
	//第１引数の文字列は、可能性の判定として、SJIS判定であれば処理を行います。
	//第１引数の文字列に対して、Null終端がない場合の動作は、strlenと同様にバッファオーバーランを起こします。
	inline bool ConvertUTF8ToMBS(const utf8_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(reinterpret_cast<const char*>(pSrc)) != SCHTYPE_UTF8)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(reinterpret_cast<const char*>(pSrc)) + 1;

		//途中のwchar_tが2Byteでも4Byteでも最大4Byte(UTF16のサロゲートペアを考慮するとUTF16も最大4Byte使う)
		//そのため、ベストフィットを計算する処理をパフォーマンス優先にして無くして、グッドフィットにする。
		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcslen] {}; //初期化時0クリア
		if (wcsbuffer == nullptr)
		{
			return false;
		}

#if WCHAR_MAX <= 0xffffU
		//wchar_t = 2byte
		if (!ConvertUTF8ToUTF16(pSrc, reinterpret_cast<char16_t*>(wcsbuffer), nullptr))
		{
			delete[] wcsbuffer;
			return false;
		}

#else
		//wchar_t = 4byte
		if (!ConvertUTF8ToUTF32(pSrc, wcsbuffer, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		}

#endif

		uint64_t mbslen = wcslen << 1;
		char* mbsbuffer = new(std::nothrow) char[(mbslen)]{}; //初期化時0クリア
		if (mbsbuffer == nullptr)
		{
			delete[] wcsbuffer;
			return false;
		};

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(pDest, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] mbsbuffer;
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//バイト数で必要サイズを返却
		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize;//null文字分追加でByte数で返却
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		}

		delete[] mbsbuffer;
		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertUTF8ToMBS(const utf8_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(reinterpret_cast<const char*>(pSrc)) != SCHTYPE_UTF8)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(reinterpret_cast<const char*>(pSrc)) + 1;

		//途中のwchar_tが2Byteでも4Byteでも最大4Byte(UTF16のサロゲートペアを考慮するとUTF16も最大4Byte使う)
		//そのため、ベストフィットを計算する処理をパフォーマンス優先にして無くして、グッドフィットにする。
		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcslen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcslen] {}; //初期化時0クリア


#if WCHAR_MAX <= 0xffffU
	//wchar_t = 2byte
		if (!ConvertUTF8ToUTF16(pSrc, reinterpret_cast<char16_t*>(wcsbuffer), nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		}

#else
	//wchar_t = 4byte
		if (!ConvertUTF8ToUTF32(pSrc, wcsbuffer, nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		}

#endif

		uint64_t mbslen = wcslen << 1;

		l_allocbuffer = _allocator_->memal(sizeof(char) * mbslen);
		if (l_allocbuffer == nullptr)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		char* mbsbuffer = new(l_allocbuffer) char[(mbslen)]{}; //初期化時0クリア

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(pDest, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

				_allocator_->memdel(mbsbuffer);
				_allocator_->memdel(wcsbuffer);
				//delete[] mbsbuffer;
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//バイト数で必要サイズを返却
		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize;//null文字分追加でByte数で返却
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		};

		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		}

		_allocator_->memdel(mbsbuffer);
		_allocator_->memdel(wcsbuffer);
		//delete[] mbsbuffer;
		//delete[] wcsbuffer;
		return true;
	};

	//マルチバイト文字列をUTF16文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	inline bool ConvertMBStoUTF16(const char* pSrc, char16_t* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1; //null文字分

		//2ByteUTF16でも最大サロゲートペアで4Byte使うので4Byte領域で文字数分配列確保
		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcslen] {}; //初期化時0クリア
		if (wcsbuffer == nullptr)
		{
			return false;
		};

		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		//使用ロケールでまず変換試行
		setlocale(LC_CTYPE, locale);

		size_t  convstrcnt = 0;
		convstrcnt = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
		if (convstrcnt == (-1))
		{
			//セットしてあるロケールで失敗したのプログラム開始時の初期化値でやってみる。
			std::setlocale(LC_CTYPE, _default_);
			convstrcnt = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
			if (convstrcnt == (-1))
			{
				//だめだったのでエラー
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//wchar_tのサイズによって経路変更
#if WCHAR_MAX <= 0xffffU
		uint64_t CopySize = GetStringLengthByte(reinterpret_cast<char16_t*>(wcsbuffer)) + 2;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //そのままサイズ返却
			delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] wcsbuffer;
			return false;
		};

		//wchar_t = 2ByteならUTF16でmbstowcsされてるのでそのままmemcpyして終了
		try
		{
#if defined(_S_STRING_COPY_STL)
			CopySize >>= 1; //要素数へ変換
			std::copy_n(reinterpret_cast<char16_t*>(wcsbuffer), CopySize, pDest);
#else
			std::memcpy(pDest, wcsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			delete[] wcsbuffer;
			return false;
		};

#else
	//wchar_t = 4ByteならUTF32toUTF16で変換して終了
		if (pDest == nullptr && DestBufferSize != nullptr)
		{
			ConvertUTF32ToUTF16(wcsbuffer, nullptr, DestBufferSize); //Byte単位で取得
			delete[] wcsbuffer;
			return false;
		};

		if (!ConvertUTF32ToUTF16(wcsbuffer, pDest, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#endif

		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertMBStoUTF16(const char* pSrc, char16_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1; //null文字分

		//2ByteUTF16でも最大サロゲートペアで4Byte使うので4Byte領域で文字数分配列確保
		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcslen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcslen] {}; //初期化時0クリア

		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		//使用ロケールでまず変換試行
		setlocale(LC_CTYPE, locale);

		size_t  convstrcnt = 0;
		convstrcnt = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
		if (convstrcnt == (-1))
		{
			//セットしてあるロケールで失敗したのプログラム開始時の初期化値でやってみる。
			std::setlocale(LC_CTYPE, _default_);
			convstrcnt = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
			if (convstrcnt == (-1))
			{
				//だめだったのでエラー
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

				_allocator_->memdel(wcsbuffer);
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		//wchar_tのサイズによって経路変更
#if WCHAR_MAX <= 0xffffU
		uint64_t CopySize = GetStringLengthByte(reinterpret_cast<char16_t*>(wcsbuffer)) + 2;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //そのままサイズ返却

			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		//wchar_t = 2ByteならUTF16でmbstowcsされてるのでそのままmemcpyして終了
		try
		{
#if defined(_S_STRING_COPY_STL)
			CopySize >>= 1; //要素数へ変換
			std::copy_n(reinterpret_cast<char16_t*>(wcsbuffer), CopySize, pDest);
#else
			std::memcpy(pDest, wcsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#else
	//wchar_t = 4ByteならUTF32toUTF16で変換して終了
		if (pDest == nullptr && DestBufferSize != nullptr)
		{
			ConvertUTF32ToUTF16(wcsbuffer, nullptr, DestBufferSize, _allocator_); //Byte単位で取得

			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		if (!ConvertUTF32ToUTF16(wcsbuffer, pDest, nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#endif

		_allocator_->memdel(wcsbuffer);
		//delete[] wcsbuffer;
		return true;
	};

	//UTF16文字列をマルチバイト文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	inline bool ConvertUTF16toMBS(const char16_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t wcslen = GetStringLengthByte(pSrc) + 2; //Byte数で取得。
		uint64_t mbslen = wcslen; //変換後mbsが格納できる最大要素数は UTF16の使用バイト数で収まる範囲なのでそのまま渡す。

		wcslen >>= 1; //要素数へ置き換え
		//char32_tとして4Byteブロックで要素数分確保。これでwchar_tがUTf16, 32どちらであろうと格納できる。
		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcslen] {}; //初期化時0クリア
		if (wcsbuffer == nullptr)
		{
			return false;
		}

#if WCHAR_MAX <= 0xffffU
		//wchar_t = 2Byte
		//wcstombs はwcs部分を2Byteブロックとして扱うのでUTF16として渡す。
		//引数をそのままコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pSrc, wcslen, reinterpret_cast<char16_t*>(wcsbuffer));
#else
			std::memcpy(wcsbuffer, pSrc, (wcslen << 1)); //wcslen は一旦要素数に変換しているので再度Byte数へ
#endif

		}
		catch (...)
		{
			delete[] wcsbuffer;
			return false;
		};


#else
		//wchar_t = 4Byte
		//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として渡す。
		//UTF16 を 32に変換。バッファはもともと32として取っているのいでサイズ取得のためのコール省略し、直変換
		if (!ConvertUTF16ToUTF32(pSrc, wcsbuffer, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#endif

		char* mbsbuffer = new(std::nothrow) char[mbslen] {}; //初期化時0クリア
		if (mbsbuffer == nullptr)
		{
			delete[] wcsbuffer;
			return false;
		};

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] mbsbuffer;
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //Byte数で返却
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		};

		//コピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		};

		delete[] mbsbuffer;
		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertUTF16toMBS(const char16_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t wcslen = GetStringLengthByte(pSrc) + 2; //Byte数で取得。
		uint64_t mbslen = wcslen; //変換後mbsが格納できる最大要素数は UTF16の使用バイト数で収まる範囲なのでそのまま渡す。

		wcslen >>= 1; //要素数へ置き換え
		//char32_tとして4Byteブロックで要素数分確保。これでwchar_tがUTf16, 32どちらであろうと格納できる。
		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcslen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcslen] {}; //初期化時0クリア

#if WCHAR_MAX <= 0xffffU
	//wchar_t = 2Byte
	//wcstombs はwcs部分を2Byteブロックとして扱うのでUTF16として渡す。
	//引数をそのままコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pSrc, wcslen, reinterpret_cast<char16_t*>(wcsbuffer));
#else
			std::memcpy(wcsbuffer, pSrc, (wcslen << 1)); //wcslen は一旦要素数に変換しているので再度Byte数へ
#endif

		}
		catch (...)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};


#else
	//wchar_t = 4Byte
	//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として渡す。
	//UTF16 を 32に変換。バッファはもともと32として取っているのいでサイズ取得のためのコール省略し、直変換
		if (!ConvertUTF16ToUTF32(pSrc, wcsbuffer, nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#endif

		l_allocbuffer = _allocator_->memal(sizeof(char) * mbslen);
		if (l_allocbuffer == nullptr)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		char* mbsbuffer = new(l_allocbuffer) char[mbslen] {}; //初期化時0クリア

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

				_allocator_->memdel(mbsbuffer);
				_allocator_->memdel(wcsbuffer);
				//delete[] mbsbuffer;
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //Byte数で返却

			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		};

		//コピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		};

		_allocator_->memdel(mbsbuffer);
		_allocator_->memdel(wcsbuffer);
		//delete[] mbsbuffer;
		//delete[] wcsbuffer;
		return true;
	};

	//マルチバイト文字列をUTF32文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	inline bool ConvertMBStoUTF32(const char* pSrc, char32_t* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1;
		//utf32はバイト長なので..。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = wcslen << 2; //Byte数で返却
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcslen];
		if (wcsbuffer == nullptr)
		{
			return false;
		};

		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		//使用ロケールでまず変換試行
		setlocale(LC_CTYPE, locale);

		size_t  conv = 0;
		conv = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
		if (conv == (-1))
		{
			//セットしてあるロケールで失敗したのプログラム開始時の初期化値でやってみる。
			std::setlocale(LC_CTYPE, _default_);
			conv = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
			if (conv == (-1))
			{
				//だめだったのでエラー
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

#if WCHAR_MAX <= 0xffffU
		//wchar_t = 2Byte
		//mbstowcs はwcs部分を2Byteブロックとして扱うのでUTF16として返ってくる
		//pDest(返却先)に対して直接UTF32に変換。
		if (!ConvertUTF16ToUTF32(reinterpret_cast<char16_t*>(wcsbuffer), pDest, nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#else
		//wchar_t = 4Byte
		//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として返ってくる
		//そのままpDest(返却先)に対してコピー。
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(wcsbuffer, wcslen, pDest);
#else
			std::memcpy(pDest, wcsbuffer, wcslen << 2);
#endif

		}
		catch (...)
		{
			delete[] wcsbuffer;
			return false;
		};

#endif

		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertMBStoUTF32(const char* pSrc, char32_t* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		if (CheckConvertType(pSrc) != SCHTYPE_SJIS)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1;
		//utf32はバイト長なので..。
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = wcslen << 2; //Byte数で返却
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			return false;
		};

		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcslen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcslen];


		//ロケール操作ロック
		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		//使用ロケールでまず変換試行
		setlocale(LC_CTYPE, locale);

		size_t  conv = 0;
		conv = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
		if (conv == (-1))
		{
			//セットしてあるロケールで失敗したのプログラム開始時の初期化値でやってみる。
			std::setlocale(LC_CTYPE, _default_);
			conv = std::mbstowcs(reinterpret_cast<wchar_t*>(wcsbuffer), pSrc, wcslen);
			if (conv == (-1))
			{
				//だめだったのでエラー
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

				_allocator_->memdel(wcsbuffer);
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

#if WCHAR_MAX <= 0xffffU
		//wchar_t = 2Byte
		//mbstowcs はwcs部分を2Byteブロックとして扱うのでUTF16として返ってくる
		//pDest(返却先)に対して直接UTF32に変換。
		if (!ConvertUTF16ToUTF32(reinterpret_cast<char16_t*>(wcsbuffer), pDest, nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#else
		//wchar_t = 4Byte
		//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として返ってくる
		//そのままpDest(返却先)に対してコピー。
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(wcsbuffer, wcslen, pDest);
#else
			std::memcpy(pDest, wcsbuffer, wcslen << 2);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#endif

		_allocator_->memdel(wcsbuffer);
		//delete[] wcsbuffer;
		return true;
	};

	//UTF32文字列をマルチバイト文字列に変換します。
	//内部ではmbstowcs_s関数を使用しますが、一時領域を確保し、コピーして処理を行うため、コピー元領域、コピー先領域が重なっていても正常にコピーされます。
	inline bool ConvertUTF32toMBS(const char32_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1;

		//バッファ確保
		char32_t* wcsbuffer = new(std::nothrow) char32_t[wcslen] {}; //初期化時0クリア
		if (wcsbuffer == nullptr)
		{
			return false;
		}

#if WCHAR_MAX <= 0xffffU
		//wchar_t = 2Byte
		//wcstombs はwcs部分を2Byteブロックとして扱うのでUTF16として渡す。
		//UTF16へ変換
		if (!ConvertUTF32ToUTF16(pSrc, reinterpret_cast<char16_t*>(wcsbuffer), nullptr))
		{
			delete[] wcsbuffer;
			return false;
		};

#else
		//wchar_t = 4Byte
		//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として渡す。
		//そのままコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pSrc, wcslen, wcsbuffer);
#else
			std::memcpy(wcsbuffer, pSrc, (wcslen << 2));
#endif

		}
		catch (...)
		{
			delete[] wcsbuffer;
			return false;
		};

#endif

		uint64_t mbslen = wcslen;
		//MBS必要バッファ数は文字数 *2;
		char* mbsbuffer = new(std::nothrow) char[(mbslen << 1)]{}; //初期化時0クリア
		if (mbsbuffer == nullptr)
		{
			delete[] wcsbuffer;
			return false;
		};

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();
				delete[] mbsbuffer;
				delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //Byte数で返却
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		};

		//コピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			delete[] mbsbuffer;
			delete[] wcsbuffer;
			return false;
		};

		delete[] mbsbuffer;
		delete[] wcsbuffer;
		return true;
	};
	inline bool ConvertUTF32toMBS(const char32_t* pSrc, char* pDest, uint64_t* DestBufferSize, const char* locale, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (pSrc == nullptr || (*pSrc) == 0)
		{
			return false;
		};

		uint64_t wcslen = GetStringCount(pSrc) + 1;

		//バッファ確保
		void* l_allocbuffer = _allocator_->memal(sizeof(char32_t) * wcslen);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char32_t* wcsbuffer = new(l_allocbuffer) char32_t[wcslen] {}; //初期化時0クリア

#if WCHAR_MAX <= 0xffffU
	//wchar_t = 2Byte
	//wcstombs はwcs部分を2Byteブロックとして扱うのでUTF16として渡す。
	//UTF16へ変換
		if (!ConvertUTF32ToUTF16(pSrc, reinterpret_cast<char16_t*>(wcsbuffer), nullptr, _allocator_))
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#else
	//wchar_t = 4Byte
	//wcstombs は wcs部分を4Byteブロックとして扱うのでUTF32として渡す。
	//そのままコピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(pSrc, wcslen, wcsbuffer);
#else
			std::memcpy(wcsbuffer, pSrc, (wcslen << 2));
#endif

		}
		catch (...)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

#endif

		uint64_t mbslen = wcslen;
		//MBS必要バッファ数は文字数 *2;
		l_allocbuffer = _allocator_->memal(sizeof(char) * (mbslen << 1));
		if (l_allocbuffer == nullptr)
		{
			_allocator_->memdel(wcsbuffer);
			//delete[] wcsbuffer;
			return false;
		};

		char* mbsbuffer = new(l_allocbuffer) char[(mbslen << 1)]{}; //初期化時0クリア

		SonikConvStaticLocale::LocaleManagerSingleton::instance().Lock_LocaleControl();
		const char* _default_ = SonikConvStaticLocale::LocaleManagerSingleton::instance().get_initialize_locale();

		std::setlocale(LC_CTYPE, locale);

		size_t err = 0;
		err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
		if (err == (-1))
		{
			//セットしてあるロケールで失敗したのデフォルトでやってみる。
			std::setlocale(LC_CTYPE, _default_);
			err = std::wcstombs(mbsbuffer, reinterpret_cast<wchar_t*>(wcsbuffer), mbslen);
			if (err == (-1))
			{
				SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

				_allocator_->memdel(mbsbuffer);
				_allocator_->memdel(wcsbuffer);
				//delete[] mbsbuffer;
				//delete[] wcsbuffer;
				return false;
			};
		};

		std::setlocale(LC_CTYPE, _default_);
		SonikConvStaticLocale::LocaleManagerSingleton::instance().UnLock_LocaleControl();

		uint64_t CopySize = GetStringLengthByte(mbsbuffer) + 1;
		if (DestBufferSize != nullptr && pDest == nullptr)
		{
			(*DestBufferSize) = CopySize; //Byte数で返却

			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return true;

		}
		else if (pDest == nullptr)
		{
			//DestBufferSize がnullptr でpDestもnullptrのときは出力できないので処理終了
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		};

		//コピー
		try
		{
#if defined(_S_STRING_COPY_STL)
			std::copy_n(mbsbuffer, CopySize, pDest);
#else
			std::memcpy(pDest, mbsbuffer, CopySize);
#endif

		}
		catch (...)
		{
			_allocator_->memdel(mbsbuffer);
			_allocator_->memdel(wcsbuffer);
			//delete[] mbsbuffer;
			//delete[] wcsbuffer;
			return false;
		};

		_allocator_->memdel(mbsbuffer);
		_allocator_->memdel(wcsbuffer);
		//delete[] mbsbuffer;
		//delete[] wcsbuffer;
		return true;
	};

	//UTF8文字列における、全角英数字及び、感嘆符(！(エクスクラメーションマーク))等の記号を半角英数字に変換します。
	//UTF-8における全角から半角変換において、領域オーバーをすることがないはずなのでそのまま領域を変換し、終端コードを最後に付与します。
	//第３引数は終端コードを含めた配列の使用要素数です。
	//FWC = Full Width Charcter
	//HWC = Half Width Charcter
	//AN = Alphabet and Number
	inline bool ConvertUTF8FWCToHWCForAN(const char* ConvertStr, char* dst, uint64_t& UsedSize)
	{
		if (ConvertStr == nullptr || (*ConvertStr) == 0)
		{
			return false;
		};

		SonikLibConvertType type_ = CheckConvertType(ConvertStr);
		if (SonikLibConvertType::SCHTYPE_UTF8 != type_)
		{
			//UTF8じゃない模様。
			return false;
		};

		uint64_t StrByte = GetStringLengthByte(ConvertStr);
		if ((StrByte & 0x01) != 0)
		{
			++StrByte;
		};
		char16_t* buffer = new(std::nothrow) char16_t[(StrByte >> 1)]{};
		if (buffer == nullptr)
		{
			//確保失敗
			return false;
		};

		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		uint64_t PlusValue = 0;
		uint64_t Offset = 0;
		unsigned char swapbit = 0;
		char* InsertPoint = reinterpret_cast<char*>(buffer);
		const unsigned char* l_convstr = reinterpret_cast<const unsigned char*>(ConvertStr);

		while ((*l_convstr) != 0x00)
		{
			/*
			swapbit = SonikMathBit::BitSwap((*l_convstr));

			bitcnt = ~(swapbit);

			bitcnt |= (bitcnt << 1);
			bitcnt |= (bitcnt << 2);
			bitcnt |= (bitcnt << 4);
			bitcnt |= (bitcnt << 8);
			bitcnt |= (bitcnt << 16);
			bitcnt |= (bitcnt << 32);

			bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
			bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
			bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
			bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
			bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

			bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);

			bitcnt = (64 - bitcnt);
			*/

			swapbit = (*l_convstr) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
			bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
			bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
			bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
			//黒魔術ビット操作
			swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
			swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

			bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

			if (bitcnt == 0)
			{
				++bitcnt;
			};

			PlusValue = 0;
			Offset = 0;
			if (bitcnt == 3)
			{
				PlusValue |= (*l_convstr);
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 1)));
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 2)));

				if (0xEFBC81 <= PlusValue && PlusValue <= 0xEFBD9E)
				{
					//c:範囲内なら処理
					PlusValue -= 0xEFBC60;
					//BCとBD間は0x40(64)ずらすとぴったしになる。
					Offset = ((*(l_convstr + 1)) - 0xBC) * 64;
					PlusValue += Offset;

					(*InsertPoint) = static_cast<char>(PlusValue);
					++InsertPoint;
					++cnt;

					l_convstr += bitcnt;

					continue;
				};

			};

			std::memmove(InsertPoint, l_convstr, bitcnt);

			InsertPoint += bitcnt;
			l_convstr += bitcnt;
			cnt += bitcnt;


		};//end while

		(*InsertPoint) = 0x00;
		++cnt;

		if (dst == nullptr)
		{
			//使用サイズを要素数を返却。
			UsedSize = cnt;
			delete[] buffer;
			return true;
		};

		if (UsedSize < cnt)
		{
			//領域不足
			delete[] buffer;
			return false;
		};

		std::memcpy(dst, buffer, cnt);
		UsedSize = cnt;
		delete[] buffer;
		return true;
	};
	inline bool ConvertUTF8FWCToHWCForAN(const char* ConvertStr, char* dst, uint64_t& UsedSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (ConvertStr == nullptr || (*ConvertStr) == 0)
		{
			return false;
		};

		SonikLibConvertType type_ = CheckConvertType(ConvertStr);
		if (SonikLibConvertType::SCHTYPE_UTF8 != type_)
		{
			//UTF8じゃない模様。
			return false;
		};

		uint64_t StrByte = GetStringLengthByte(ConvertStr);
		if ((StrByte & 0x01) != 0)
		{
			++StrByte;
		};

		void* l_allocbuffer = _allocator_->memal(sizeof(char16_t) * (StrByte >> 1));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char16_t* buffer = new(l_allocbuffer) char16_t[(StrByte >> 1)]{};

		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		uint64_t PlusValue = 0;
		uint64_t Offset = 0;
		unsigned char swapbit = 0;
		char* InsertPoint = reinterpret_cast<char*>(buffer);
		const unsigned char* l_convstr = reinterpret_cast<const unsigned char*>(ConvertStr);

		while ((*l_convstr) != 0x00)
		{
			/*
			swapbit = SonikMathBit::BitSwap((*l_convstr));

			bitcnt = ~(swapbit);

			bitcnt |= (bitcnt << 1);
			bitcnt |= (bitcnt << 2);
			bitcnt |= (bitcnt << 4);
			bitcnt |= (bitcnt << 8);
			bitcnt |= (bitcnt << 16);
			bitcnt |= (bitcnt << 32);

			bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
			bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
			bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
			bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
			bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

			bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);

			bitcnt = (64 - bitcnt);
			*/

			swapbit = (*l_convstr) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
			bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
			bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
			bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
			//黒魔術ビット操作
			swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
			swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

			bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

			if (bitcnt == 0)
			{
				++bitcnt;
			};

			PlusValue = 0;
			Offset = 0;
			if (bitcnt == 3)
			{
				PlusValue |= (*l_convstr);
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 1)));
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 2)));

				if (0xEFBC81 <= PlusValue && PlusValue <= 0xEFBD9E)
				{
					//c:範囲内なら処理
					PlusValue -= 0xEFBC60;
					//BCとBD間は0x40(64)ずらすとぴったしになる。
					Offset = ((*(l_convstr + 1)) - 0xBC) * 64;
					PlusValue += Offset;

					(*InsertPoint) = static_cast<char>(PlusValue);
					++InsertPoint;
					++cnt;

					l_convstr += bitcnt;

					continue;
				};

			};

			std::memmove(InsertPoint, l_convstr, bitcnt);

			InsertPoint += bitcnt;
			l_convstr += bitcnt;
			cnt += bitcnt;


		};//end while

		(*InsertPoint) = 0x00;
		++cnt;

		if (dst == nullptr)
		{
			//使用サイズを要素数を返却。
			UsedSize = cnt;

			_allocator_->memdel(buffer);
			//delete[] buffer;
			return true;
		};

		if (UsedSize < cnt)
		{
			//領域不足
			delete[] buffer;
			return false;
		};

		std::memcpy(dst, buffer, cnt);
		UsedSize = cnt;

		_allocator_->memdel(buffer);
		//delete[] buffer;
		return true;

	};

	//UTF8文字列のおける、全角カナを半角カナに変換します。
	inline bool ConvertUTF8FWCToHWCForKANA(const char* ConvertStr, char* dst, uint64_t& UsedSize)
	{
		if (ConvertStr == nullptr || (*ConvertStr) == 0)
		{
			return false;
		};

		SonikLibConvertType type_ = CheckConvertType(ConvertStr);
		if (SonikLibConvertType::SCHTYPE_UTF8 != type_)
		{
			//UTF8じゃない模様。
			return false;
		};

		//バイト数取得。
		uint64_t StrSize = GetStringLengthByte(ConvertStr);
		//NULL文字分プラスして一時的にバッファサイズも２倍確保
		StrSize = ((StrSize + 1) << 1);

		//一時的に2倍のサイズが必要。
		char* buffer = new(std::nothrow) char[StrSize] {};
		if (buffer == nullptr)
		{
			return false;
		};

		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		uint64_t PlusValue = 0;
		uint64_t Offset_first = 0;
		uint64_t Offset_second = 0;
		unsigned char swapbit = 0;
		char* InsertPoint = reinterpret_cast<char*>(buffer);
		char* InsertTopPoint = InsertPoint;
		unsigned char* PlusValueAccess = nullptr;
		const unsigned char* l_convstr = reinterpret_cast<const unsigned char*>(ConvertStr);


		while ((*l_convstr) != 0x00)
		{
			/*
			swapbit = SonikMathBit::BitSwap((*l_convstr));

			bitcnt = ~(swapbit);

			bitcnt |= (bitcnt << 1);
			bitcnt |= (bitcnt << 2);
			bitcnt |= (bitcnt << 4);
			bitcnt |= (bitcnt << 8);
			bitcnt |= (bitcnt << 16);
			bitcnt |= (bitcnt << 32);

			bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
			bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
			bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
			bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
			bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

			bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);

			bitcnt = (64 - bitcnt);
			*/

			swapbit = (*l_convstr) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
			bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
			bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
			bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
			//黒魔術ビット操作
			swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
			swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

			bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

			if (bitcnt == 0)
			{
				++bitcnt;
			};

			PlusValue = 0;
			Offset_first = 0;
			Offset_second = 0;
			if (bitcnt == 3)
			{
				PlusValue |= (*l_convstr);
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 1)));
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 2)));

				if (0xE382A1 <= PlusValue && PlusValue <= 0xE383BA)
				{
					//c:範囲内なら処理
					Offset_first = (*(l_convstr + 1)) - 0x82;
					Offset_second = PlusValue - SonikConvStaticTable::HW_KANA_OFFSET_STARTPOINT[((*(l_convstr + 1)) - 0x82)] + (Offset_first * 33);


					//c:配列から値検索
					PlusValue = SonikConvStaticTable::HW_KANA[Offset_second];
					PlusValueAccess = reinterpret_cast<unsigned char*>(&PlusValue);
					Offset_first = ((((*PlusValueAccess) & 0x80) >> 7) * 3) + 3;	//InsertPointをずらす値を取得
					Offset_second = ((*PlusValueAccess) & 0x7F);		//c: 濁点位置の取得

					//c:変更前のポイントの取得
					InsertTopPoint = InsertPoint;
					//c:代入処理
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 3)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 2)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 1)));
					++InsertPoint;

					//c:濁点位置に代入。0ならｧが入るが、ポインタは３つしか動かないため、次回代入時に消えるか１Byte目に終点文字である0が入る
					PlusValue = SonikConvStaticTable::HW_KANA[Offset_second];
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 3)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 2)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 1)));

					//c;オフセット適用
					InsertPoint = InsertTopPoint + Offset_first;

					//c:入力位置を進める。
					l_convstr += bitcnt;
					//c:使用サイズカウント
					cnt += Offset_first;
					continue;
				};

			};

			InsertPoint += bitcnt;
			l_convstr += bitcnt;
			cnt += bitcnt;
		};//end while

		(*InsertPoint) = 0x00;
		++cnt;

		if (dst == nullptr)
		{
			//必要サイズ返却
			UsedSize = cnt;
			delete[] buffer;
			return true;
		};

		//c:再度適切な配列サイズを作り直してもう一度コピー
		if (UsedSize < cnt)
		{
			//c:容量足りない。
			delete[] buffer;
			return false;
		};

		memcpy(dst, buffer, UsedSize);
		delete[] buffer;

		UsedSize = cnt;
		return true;
	};
	inline bool ConvertUTF8FWCToHWCForKANA(const char* ConvertStr, char* dst, uint64_t& UsedSize, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		if (ConvertStr == nullptr || (*ConvertStr) == 0)
		{
			return false;
		};

		SonikLibConvertType type_ = CheckConvertType(ConvertStr);
		if (SonikLibConvertType::SCHTYPE_UTF8 != type_)
		{
			//UTF8じゃない模様。
			return false;
		};

		//バイト数取得。
		uint64_t StrSize = GetStringLengthByte(ConvertStr);
		//NULL文字分プラスして一時的にバッファサイズも２倍確保
		StrSize = ((StrSize + 1) << 1);

		//一時的に2倍のサイズが必要。
		void* l_allocbuffer = _allocator_->memal(sizeof(char) * StrSize);
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		char* buffer = new(l_allocbuffer) char[StrSize] {};


		uint64_t cnt = 0;
		uint64_t bitcnt = 0;
		uint64_t PlusValue = 0;
		uint64_t Offset_first = 0;
		uint64_t Offset_second = 0;
		unsigned char swapbit = 0;
		char* InsertPoint = reinterpret_cast<char*>(buffer);
		char* InsertTopPoint = InsertPoint;
		unsigned char* PlusValueAccess = nullptr;
		const unsigned char* l_convstr = reinterpret_cast<const unsigned char*>(ConvertStr);


		while ((*l_convstr) != 0x00)
		{
			/*
			swapbit = SonikMathBit::BitSwap((*l_convstr));

			bitcnt = ~(swapbit);

			bitcnt |= (bitcnt << 1);
			bitcnt |= (bitcnt << 2);
			bitcnt |= (bitcnt << 4);
			bitcnt |= (bitcnt << 8);
			bitcnt |= (bitcnt << 16);
			bitcnt |= (bitcnt << 32);

			bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
			bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
			bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
			bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
			bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

			bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);

			bitcnt = (64 - bitcnt);
			*/

			swapbit = (*l_convstr) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
			bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
			bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
			bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
			//黒魔術ビット操作
			swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
			swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

			bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

			if (bitcnt == 0)
			{
				++bitcnt;
			};

			PlusValue = 0;
			Offset_first = 0;
			Offset_second = 0;
			if (bitcnt == 3)
			{
				PlusValue |= (*l_convstr);
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 1)));
				PlusValue = ((PlusValue << 8) | (*(l_convstr + 2)));

				if (0xE382A1 <= PlusValue && PlusValue <= 0xE383BA)
				{
					//c:範囲内なら処理
					Offset_first = (*(l_convstr + 1)) - 0x82;
					Offset_second = PlusValue - SonikConvStaticTable::HW_KANA_OFFSET_STARTPOINT[((*(l_convstr + 1)) - 0x82)] + (Offset_first * 33);


					//c:配列から値検索
					PlusValue = SonikConvStaticTable::HW_KANA[Offset_second];
					PlusValueAccess = reinterpret_cast<unsigned char*>(&PlusValue);
					Offset_first = ((((*PlusValueAccess) & 0x80) >> 7) * 3) + 3;	//InsertPointをずらす値を取得
					Offset_second = ((*PlusValueAccess) & 0x7F);		//c: 濁点位置の取得

					//c:変更前のポイントの取得
					InsertTopPoint = InsertPoint;
					//c:代入処理
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 3)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 2)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 1)));
					++InsertPoint;

					//c:濁点位置に代入。0ならｧが入るが、ポインタは３つしか動かないため、次回代入時に消えるか１Byte目に終点文字である0が入る
					PlusValue = SonikConvStaticTable::HW_KANA[Offset_second];
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 3)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 2)));
					++InsertPoint;
					(*InsertPoint) = static_cast<char>((*(PlusValueAccess + 1)));

					//c;オフセット適用
					InsertPoint = InsertTopPoint + Offset_first;

					//c:入力位置を進める。
					l_convstr += bitcnt;
					//c:使用サイズカウント
					cnt += Offset_first;
					continue;
				};

			};

			InsertPoint += bitcnt;
			l_convstr += bitcnt;
			cnt += bitcnt;
		};//end while

		(*InsertPoint) = 0x00;
		++cnt;

		if (dst == nullptr)
		{
			//必要サイズ返却
			UsedSize = cnt;

			_allocator_->memdel(buffer);
			//delete[] buffer;
			return true;
		};

		//c:再度適切な配列サイズを作り直してもう一度コピー
		if (UsedSize < cnt)
		{
			//c:容量足りない。
			_allocator_->memdel(buffer);
			//delete[] buffer;
			return false;
		};

		memcpy(dst, buffer, UsedSize);
		_allocator_->memdel(buffer);
		//delete[] buffer;

		UsedSize = cnt;
		return true;
	};

};

namespace SonikLibStringControl
{
	//c:指定された位置から指定された文字数を削除します。
	inline bool StringPointEraser(char* ControlStr, uint64_t StartPoint, uint64_t EndPoint, uint64_t MaxStrCnt)
	{
		if ((ControlStr == nullptr) || (*ControlStr) == 0 || (EndPoint > MaxStrCnt) || (StartPoint > MaxStrCnt))
		{
			return false;
		};

		if (StartPoint == 0)
		{
			StartPoint = 1;
		};

		if (EndPoint == 0)
		{
			EndPoint = MaxStrCnt;
		};

		//c:ポイントの捜索
		SonikLibConvertType Type_ = SonikLibStringConvert::CheckConvertType(ControlStr);
		uint64_t cnt = 0;
		uint64_t endcnt = MaxStrCnt + 1;
		uint64_t OffsetCnt = 1;
		uint64_t bitcnt = 0;
		unsigned char swapbit = 0;
		unsigned char* pCheckSrc = reinterpret_cast<unsigned char*>(ControlStr);

		uintptr_t StartPointAddress = 0;
		uintptr_t EndPointAddress = 0;

		if (Type_ == SCHTYPE_SJIS)
		{
			while ((*pCheckSrc) != 0x00)
			{
				OffsetCnt = 1;

				if ((0x80 <= (*pCheckSrc) && (*pCheckSrc) <= 0x9F) || (0xE0 <= (*pCheckSrc) && (*pCheckSrc) <= 0xFC))
				{
					++OffsetCnt;
				};

				++cnt;

				if (cnt == StartPoint)
				{
					StartPointAddress = reinterpret_cast<uintptr_t>(pCheckSrc);
					endcnt = cnt + EndPoint - 1;
				};

				if (cnt == endcnt)
				{
					EndPointAddress = reinterpret_cast<uintptr_t>(pCheckSrc);
				};

				pCheckSrc += OffsetCnt;

			};

		}
		else if (Type_ == SCHTYPE_UTF8)
		{
			while ((*pCheckSrc) != 0x00)
			{
				/*
				swapbit = SonikMathBit::BitSwap((*pCheckSrc));

				bitcnt = ~(swapbit);

				bitcnt |= (bitcnt << 1);
				bitcnt |= (bitcnt << 2);
				bitcnt |= (bitcnt << 4);
				bitcnt |= (bitcnt << 8);
				bitcnt |= (bitcnt << 16);
				bitcnt |= (bitcnt << 32);

				bitcnt = (bitcnt & 0x5555555555555555) + ((bitcnt >> 1) & 0x5555555555555555);
				bitcnt = (bitcnt & 0x3333333333333333) + ((bitcnt >> 2) & 0x3333333333333333);
				bitcnt = (bitcnt & 0x0F0F0F0F0F0F0F0F) + ((bitcnt >> 4) & 0x0F0F0F0F0F0F0F0F);
				bitcnt = (bitcnt & 0x00FF00FF00FF00FF) + ((bitcnt >> 8) & 0x00FF00FF00FF00FF);
				bitcnt = (bitcnt & 0x0000FFFF0000FFFF) + ((bitcnt >> 16) & 0x0000FFFF0000FFFF);

				bitcnt = (bitcnt & 0x00000000FFFFFFFF) + ((bitcnt >> 32) & 0x00000000FFFFFFFF);
				bitcnt = (64 - bitcnt);
				*/

				swapbit = (*pCheckSrc) & 0xF0;

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
				bitcnt = static_cast<uint64_t>(__builtin_popcount(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
				bitcnt = static_cast<uint64_t>(__popcnt(static_cast<uint32_t>(swapbit)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
				bitcnt = static_cast<uint64_t>(__popcnt_u32(static_cast<uint32_t>(swapbit)));
#else
				//黒魔術ビット操作
				swapbit = (swapbit & 0x55) + ((swapbit >> 1) & 0x55);
				swapbit = (swapbit & 0x33) + ((swapbit >> 2) & 0x33);

				bitcnt = static_cast<uint64_t>((swapbit & 0x0F) + ((swapbit >> 4) & 0x0F));
#endif

				if (bitcnt == 0)
				{
					++bitcnt;
				};

				++cnt;

				if (cnt == StartPoint)
				{
					StartPointAddress = reinterpret_cast<uintptr_t>(pCheckSrc);
					endcnt = cnt + EndPoint - 1;
				};

				pCheckSrc += bitcnt;

				if (cnt == endcnt)
				{
					EndPointAddress = reinterpret_cast<uintptr_t>(pCheckSrc);
				};

			};

		}
		else
		{
			return false;
		};

		if (EndPointAddress == 0 || StartPointAddress == 0)
		{
			return false;
		};

		uint64_t movesize = reinterpret_cast<uintptr_t>(pCheckSrc) - EndPointAddress;
		memmove(reinterpret_cast<void*>(StartPointAddress), reinterpret_cast<void*>(EndPointAddress), movesize);

		movesize = EndPointAddress - StartPointAddress; //c:変数使い回し。
		EndPointAddress = reinterpret_cast<uintptr_t>(pCheckSrc) - (EndPointAddress - StartPointAddress);
		memset(reinterpret_cast<void*>(EndPointAddress), 0, movesize);

		return true;
	};
};


#endif /* SONIKString_SONIKStringCSonikERT_H_ */