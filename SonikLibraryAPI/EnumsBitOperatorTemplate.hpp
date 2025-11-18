#pragma once

#ifndef __SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__
#define __SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__

#include "CPPGrammarDefines.h"
#include "CompilersPreProcesser.h"

namespace SLibEnumsBitTemplate
{
	//トレイト本体
	template<class E>
	struct EnableOperatorBitMask : std::false_type
	{
		//no implement
	};
};

//特殊化マクロ
#define SLIB_ENABLE_ENUMCLASS_BITMASK(E) template<> struct SLibEnumsBitTemplate::EnableOperatorBitMask<E> : std::true_type {};


//____________________________________________________
//									　				  |
//    各種ビット演算の有効/無効化フラグテンプレート	  	  |
//____________________________________________________|
//有効化するときは、有効化したいenum class を定義した後に、特殊化のdefineマクロを使う。
//ただちょっと使い方に工夫が必要。
//例えば以下は名前空間の問題でテンプレートとして認識してくれなくなる。
// namespace TEST
// {
//		namespace Enums
//		{
//			enum class TestEnumCls : uint32_t{};		
// 
//			SLIB_ENABLE_ENUMCLASS_BITMASK(TestEnumCls) //NG：template<> struct TEST::Enums::SLibEnumsBitTemplate::EnableOperatorBitMask<E> : std::true_type {}; として認識され定義がない状態になる。
//		};
// 
// };
// 
// 上記のような構造で使用したい場合は一回namespace を区切ってグローバルで宣言してあげる必要がある。
// ↓これなら動く
// 
// // namespace TEST
// {
//		namespace Enums
//		{
//			enum class TestEnumCls : uint32_t{};		
// 
//		};
// 
// };
// SLIB_ENABLE_ENUMCLASS_BITMASK(TEST::Enums::TestEnumCls) //OK
//
//


//ビット演算テンプレート本体
//operator |
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E) operator |(E left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	SLIB_CVR_USING(l_cast_type, SLIB_CVRT_UNDERLYING_TYPE(E));
	return static_cast<E>(static_cast<l_cast_type>(left_value) | static_cast<l_cast_type>(right_value));
};
//operator |=
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E&) operator |=(E& left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	left_value = left_value | right_value;
	return left_value;
};

//operator &
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E) operator &(E left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	SLIB_CVR_USING(l_cast_type, SLIB_CVRT_UNDERLYING_TYPE(E));
	return static_cast<E>(static_cast<l_cast_type>(left_value) & static_cast<l_cast_type>(right_value));
};
//operator &=
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E&) operator &=(E& left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	left_value = left_value & right_value;
	return left_value;
};

//operator ^
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E) operator ^(E left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	SLIB_CVR_USING(l_cast_type, SLIB_CVRT_UNDERLYING_TYPE(E));
	return static_cast<E>(static_cast<l_cast_type>(left_value) ^ static_cast<l_cast_type>(right_value));
};
//operator ^=
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E&) operator ^=(E& left_value, E right_value) SLIB_CVR_NOEXCEPT
{
	left_value = left_value ^ right_value;
	return left_value;
};
//operator ~
template<class E>
DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SLIB_CVRT_ENABLE_IF(SLibEnumsBitTemplate::EnableOperatorBitMask<E>::value, E) operator ~(E _value_) SLIB_CVR_NOEXCEPT
{
	SLIB_CVR_USING(l_cast_type, SLIB_CVRT_UNDERLYING_TYPE(E));
	return static_cast<E>(~static_cast<l_cast_type>(_value_));
};

#endif //__SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__