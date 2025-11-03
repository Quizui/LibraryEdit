#pragma once

#ifndef __SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__
#define __SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__

#include <type_traits>
#include "CompilersPreProcesser.h"

namespace SLibEnumsBitTemplate
{

//____________________________________________________
//									　				  |
//    各種ビット演算の有効/無効化フラグテンプレート	  	  |
//____________________________________________________|
//有効化するときは、有効化したいenum class を定義した後に、EnableOperatorBitMask<enum class型>を宣言する。

//トレイト本体
	template<class E>
	struct EnableOperatorBitMask : std::false_type
	{
		//no implement
	};
	//ビット演算テンプレート本体
	//operator |
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E>::type
#endif
 	operator |(E left_value, E right_value) noexcept
	{
		using l_cast_type = std::underlying_type_t<E>;
		return static_cast<E>(static_cast<l_cast_type>(left_value) | static_cast<l_cast_type>(right_value));
	};
	//operator |=
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E&>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E&>::type
#endif
	operator |=(E& left_value, E right_value) noexcept
	{
		left_value = left_value | right_value;
		return left_value;
	};

	//operator &
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E>::type
#endif
	operator &(E left_value, E right_value) noexcept
	{
		using l_cast_type = std::underlying_type_t<E>;
		return static_cast<E>(static_cast<l_cast_type>(left_value) & static_cast<l_cast_type>(right_value));
	};
	//operator &=
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E&>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E&>::type
#endif
	operator &=(E& left_value, E right_value) noexcept
	{
		left_value = left_value & right_value;
		return left_value;
	};

	//operator ^
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E>::type
#endif
	operator ^(E left_value, E right_value) noexcept
	{
		using l_cast_type = std::underlying_type_t<E>;
		return static_cast<E>(static_cast<l_cast_type>(left_value) ^ static_cast<l_cast_type>(right_value));
	};
	//operator ^=
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E&>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E&>::type
#endif
	operator ^=(E& left_value, E right_value) noexcept
	{
		left_value = left_value ^ right_value;
		return left_value;
	};

	//operator ~
	template<class E>
#if defined(__cplusplus) && __cplusplus >= 201402L
	//C++14
	DEF_FORCE_INLINE constexpr std::enable_if_t<EnableOperatorBitMask<E>::value, E>
#else
	//C++14未満(C++11)
	DEF_FORCE_INLINE typename std::enable_if<EnableOperatorBitMask<E>::value, E>::type
#endif
	operator ~(E _value_) noexcept
	{
		using l_cast_type = std::underlying_type_t<E>;
		return static_cast<E>(~static_cast<l_cast_type>(_value_));
	};

}; //end namespace 


#endif //__SONIKLIB_ENUM_BITOPERATOR_ENABLED_TEMPLATE_HPP__