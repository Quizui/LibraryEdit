#pragma once

#ifndef __SONIKLIB_STRING_FUNCTION_VARIABLES_TYPE_PERMISSIBLE_LIMIT_TEMPLATE_H__
#define __SONIKLIB_STRING_FUNCTION_VARIABLES_TYPE_PERMISSIBLE_LIMIT_TEMPLATE_H__

#include <type_traits>

//C++20 以前はchar8_t が無いので。
#if __cplusplus < 202002L
	//C++20 以前であれば　char8_t は uint8_t
#include <stdint.h>
using utf8_t = uint8_t;
#else
using utf8_t = char8_t;

#endif

//C++14互換で、const char*, const wchar_t*, const char16_t*, const utf8_t* に制限するテンプレートを定義します。
namespace SonikLibStringPermissibleType
{
	//C++14 互換のremove_cvref_t
	template<class T>
	struct remove_cvref
	{
		using type = std::remove_cv_t<std::remove_reference<T>>;
	};
	template<class T>
	using remove_cvref_t = remove_cvref<T>::type;

	//トレイト本体
	//使用するときは以下のように使います。
	//template<class T>
	//std::enable_if_t<SonikLibStringPermissibleType::is_string_ptr_t<T>::value, void> function(T str){ /* T is only const char*, const wchar_t*, const char16_t*, const utf8_t* */}
	template<class T>
	struct is_string_ptr : std::false_type {};
	template<>
	struct is_string_ptr<const char*> : std::true_type {};
	template<>
	struct is_string_ptr<const wchar_t*> : std::true_type {};
	template<>
	struct is_string_ptr<const char16_t*> : std::true_type {};
	template<>
	struct is_string_ptr<const utf8_t*> : std::true_type {};

	//C++14互換
	//C++14 の場合は「static constexpr」で内部リンクにすることで
    //複数翻訳単位 include 時の多重定義を回避します。
	template<class T>
	static constexpr bool is_string_ptr_t = is_string_ptr< remove_cvref_t<T> >::value;


};

#endif