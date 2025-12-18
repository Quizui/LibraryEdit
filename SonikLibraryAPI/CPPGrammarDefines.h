#pragma once

#ifndef __SONIKLIB_CPP_GRAMMAR_VERSION_DEFINCES_HEDDER__
#define __SONIKLIB_CPP_GRAMMAR_VERSION_DEFINCES_HEDDER__

//C++バージョンで使えないかもしれない装飾ワードのdefineを定義します。
//CVR = Cpp Varsion Replace
//CVRT = Cpp Varsion Replatce Template
#if defined(_MSC_VER) && _MSC_VER <= 1600
	//VisualStudio2010環境時
	#include <type_traits>
	#include <utility>
	#define SLIB_CVRT_ENABLE_IF(cond, Ret)	typename std::enable_if<(cond), Ret>::type
	#define SLIB_CVRT_UNDERLYING_TYPE(T)	typename std::underlying_type<T>::type
	#define SLIB_CVRT_IS_POINTER(T)			(std::is_pointer<T>::value)
	#define SLIB_CVRT_FWD(T, v)				v //怪しいので定義無しのほうで。
	#define SLIB_CVR_CONSTEXPR				/*empty*/
	#define SLIB_CVR_NOEXCEPT				/*empty*/
	#define SLIB_CVR_USING(name, ...)		typedef __VA_ARGS__ name
	#define SLIB_CVR_STDMOVE(moveobj)		std::move(moveobj)
	#define SLIB_CVR_OVERRIDE				override //確か使えたので定義

#elif defined(__cplusplus)
	#if __cplusplus >= 201703L //C++17以上
		#include <type_traits>
		#include <utility>
		#define SLIB_CVRT_ENABLE_IF(cond, Ret)	std::enable_if_t<(cond), Ret>
		#define SLIB_CVRT_UNDERLYING_TYPE(T)	std::underlying_type_t<T>
		#define SLIB_CVRT_IS_POINTER(T)			(std::is_pointer_v<T>)
		#define SLIB_CVRT_FWD(T, v)				std::forward<T>(v)
		#define SLIB_CVR_CONSTEXPR				constexpr
		#define SLIB_CVR_NOEXCEPT				noexcept
		#define SLIB_CVR_USING(name, ...)		using name = __VA_ARGS__
		#define SLIB_CVR_ENUMCLASS(name)		enum class name
		#define SLIB_CVR_STDMOVE(moveobj)		std::move(moveobj)
		#define SLIB_CVR_OVERRIDE				override


	#elif __cplusplus >= 201402L //C++14以上
		#include <type_traits>
		#include <utility>
		#define SLIB_CVRT_ENABLE_IF(cond, Ret)	std::enable_if_t<(cond), Ret>
		#define SLIB_CVRT_UNDERLYING_TYPE(T)	std::underlying_type_t<T>
		#define SLIB_CVRT_IS_POINTER(T)			(std::is_pointer<T>::value)
		#define SLIB_CVRT_FWD(T, v)				std::forward<T>(v)
		#define SLIB_CVR_CONSTEXPR				constexpr
		#define SLIB_CVR_NOEXCEPT				noexcept
		#define SLIB_CVR_USING(name, ...)		using name = __VA_ARGS__
		#define SLIB_CVR_ENUMCLASS(name)		enum class name
		#define SLIB_CVR_STDMOVE(moveobj)		std::move(moveobj)
		#define SLIB_CVR_OVERRIDE				override

	#elif __cplusplus >=201103L //C++11以上
		#include <type_traits>
		#include <utility>
		#define SLIB_CVRT_ENABLE_IF(cond, Ret)	typename std::enable_if<(cond), Ret>::type
		#define SLIB_CVRT_UNDERLYING_TYPE(T)	typename std::underlying_type<T>::type
		#define SLIB_CVRT_IS_POINTER(T)			(std::is_pointer<T>::value)
		#define SLIB_CVRT_FWD(T, v)				std::forward<T>(v)
		#define SLIB_CVR_CONSTEXPR				constexpr
		#define SLIB_CVR_NOEXCEPT				noexcept
		#define SLIB_CVR_USING(name, ...)		using name = __VA_ARGS__
		#define SLIB_CVR_ENUMCLASS(name)		enum class name
		#define SLIB_CVR_STDMOVE(moveobj)		std::move(moveobj)
		#define SLIB_CVR_OVERRIDE				override

	#else
		//C++11環境より下なため一旦最低限の表現定義にしておく。(vs2010と同等
		#include <type_traits>
		#define SLIB_CVRT_ENABLE_IF(cond, Ret)	typename std::enable_if<(cond), Ret>::type
		#define SLIB_CVRT_UNDERLYING_TYPE(T)	typename std::underlying_type<T>::type
		#define SLIB_CVRT_IS_POINTER(T)			(std::is_pointer<T>::value)
		#define SLIB_CVRT_FWD(T, v)				v
		#define SLIB_CVR_CONSTEXPR				/*empty*/
		#define SLIB_CVR_NOEXCEPT				/*empty*/
		#define SLIB_CVR_USING(name, ...)		typedef __VA_ARGS__ name
		#define SLIB_CVR_ENUMCLASS(name)		enum name
		#define SLIB_CVR_STDMOVE(moveobj)		moveobj
		#define SLIB_CVR_OVERRIDE				/*empty*/
	
	#endif

#else
	//不明環境なため空定義
	#define SLIB_CVRT_ENABLE_IF(cond, Ret)	/*empty*/
	#define SLIB_CVRT_UNDERLYING_TYPE(T)	/*empty*/
	#define SLIB_CVRT_IS_POINTER(T)			(0)
	#define SLIB_CVRT_FWD(T, v)				v
	#define SLIB_CVR_CONSTEXPR				/*empty*/
	#define SLIB_CVR_NOEXCEPT				/*empty*/
	#define SLIB_CVR_USING(name, ...)		/*empty*/
	#define SLIB_CVR_STDMOVE(moveobj)		moveobj
	#define SLIB_CVR_OVERRIDE				/*empty*/

#endif

#endif//__SONIKLIB_CPP_GRAMMAR_VERSION_DEFINCES_HEDDER__