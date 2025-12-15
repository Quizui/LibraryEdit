#pragma once

#ifndef __COMPILER_PREPROCCESSER_DEFINITIONS_H__
#define __COMPILER_PREPROCCESSER_DEFINITIONS_H__

//※ nodiscard はコンパイラのバージョン等によって前置きしないといけないので使う場合は以下のように挟む。
//DEF_PRE_NO_DISCARD func() DEF_POST_NO_DISCARD

//______________________________________
//									　	|
//		コンパイラ別キーワード定義               |
//______________________________________|
#if defined(__INTEL_COMPILER) //ICC/ICPC（Intel Compiler Classic）

	//______________________________________
	//									　	|
	//		ForceInlineキーワード		    |
	//______________________________________|
	#define DEF_FORCE_INLINE __forceinline

	//______________________________________
	//									　	|
	//		[[nodiscard]]置換定義		    |
	//______________________________________|
	#if !defined(_Check_return_)
		//_Check_return_が定義されていなければインクルードする必要があるためインクルード
		#include <specstrings.h>
	#endif
	
	//[[nodiscard]]代替定義
    #define DEF_PRE_NO_DISCARD _Check_return_
    #define DEF_POST_NO_DISCARD /*empty*/

	//______________________________________
	//									　	|
	//		assumeキーワード				|
	//______________________________________|
	#if __INTEL_COMPILER >= 1700
		//ICC 17.0以降は__builtin_assume
        #define SLIB_ASSUME(cond) __builtin_assume(cond)

	#else
		//古いICCは__builtin_unreachable で代用
        #define SLIB_ASSUME(cond) ((void)((cond) ? 0 : __builtin_unreachable()))

	#endif


#elif defined(__INTEL_LLVM_COMPILER) //oneAPI ICX/ICPX（LLVM ベース）
	
	//______________________________________
	//									　	|
	//		ForceInlineキーワード		    |
	//______________________________________|
	#define DEF_FORCE_INLINE __forceinline

	//______________________________________
	//									　	|
	//		[[nodiscard]]置換定義		    |
	//______________________________________|
	#if __cplusplus >= 201703L
		//CPP17以上なら[[nodiscard]]を定義
        #define DEF_PRE_NO_DISCARD [[nodiscard]]
        #define DEF_POST_NO_DISCARD /*empty*/

	#else
		//17以下ならコンパイラ固有定義
		#if !defined(_Check_return_)
			//_Check_return_が定義されていなければインクルードする必要があるためインクルード
			#include <specstrings.h>
		#endif

		//[[nodiscard]]代替定義
        #define DEF_PRE_NO_DISCARD _Check_return_
        #define DEF_POST_NO_DISCARD /*empty*/
	#endif

	//______________________________________
	//									　	|
	//		assumeキーワード				|
	//______________________________________|
	//__clang__区分でカバーされる。


#elif defined(_MSC_VER) // Microsoft Visual C++ (および Windows 向け Intel C++ も _MSC_VER が立つ)

	//______________________________________
	//									　	|
	//		ForceInlineキーワード		    |
	//______________________________________|
	#define DEF_FORCE_INLINE __forceinline

	//______________________________________
	//									　	|
	//		[[nodiscard]]置換定義		    |
	//______________________________________|
	#if __cplusplus >= 201703L
		//CPP17以上なら[[nodiscard]]を定義
        #define DEF_PRE_NO_DISCARD [[nodiscard]]
        #define DEF_POST_NO_DISCARD /*empty*/

	#else
		//17以下ならコンパイラ固有定義
		#if !defined(_Check_return_)
			//_Check_return_が定義されていなければインクルードする必要があるためインクルード
			#include <specstrings.h>
		#endif

		//[[nodiscard]]代替定義
        #define DEF_PRE_NO_DISCARD _Check_return_
        #define DEF_POST_NO_DISCARD /*empty*/

	#endif

	//______________________________________
	//									　	|
	//		assumeキーワード				|
	//______________________________________|
    #define SLIB_ASSUME(cond) __analysis_assume(cond)


#elif defined(__GNUC__) || defined(__clang__) //GCC or clang

	//______________________________________
	//									　	|
	//		ForceInlineキーワード		    |
	//______________________________________|
	//ForceInlineはClanggとGCCで再度細部分けが必要なため細部分け。
	#if defined(__clang__)
		#if __clang_major__ < 10 //Clang10 以下ならgnu_inline 属性のattributeが必要。
			#define DEF_FORCE_INLINE inline __attribute__((always_inline)) __attribute__((gnu_inline))

		#else //10以上はいらなくなった。
			#define DEF_FORCE_INLINE inline __attribute__((always_inline))

		#endif

	#elif  //GCCはデフォルトでgnu_inline付与
		#define DEF_FORCE_INLINE inline __attribute__((always_inline)) __attribute__((gnu_inline))

	#endif

	//______________________________________
	//									　	|
	//		[[nodiscard]]置換定義		    |
	//______________________________________|
	#if __cplusplus >= 201703L
		//CPP17以上なら[[nodiscard]]を定義
        #define DEF_PRE_NO_DISCARD [[nodiscard]]
        #define DEF_POST_NO_DISCARD /*empty*/

	#else
		//17以下ならコンパイラ固有定義
        #define DEF_PRE_NO_DISCARD /*empty*/
        #define DEF_POST_NO_DISCARD  __attribute__((warn_unused_result))

	#endif
	
	//______________________________________
	//									　	|
	//		assumeキーワード				|
	//______________________________________|
	//Clang
	#if defined (__clang__)
		#if __has_builtin(__builtin_assume)
			//Clang 15+なら
            #define SLIB_ASSUME(cond) __builtin_assume(cond)
		#else

            #define SLIB_ASSUME(cond) ((void)((cond) ? 0 : __builtin_unreachable()))

		#endif
	
	#elif defined(__GNUC__)
		//GCCは__builtin_unreachableが基本だがバージョン4.5以前であれば何もしない処理へ。
		#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) //GCCのメジャー/マイナーで4.5以上 であればIN
            #define SLIB_ASSUME(cond) ((void)((cond) ? 0 : __builtin_unreachable()))
		
		#else
			//何もしない処理へ。
            #define SLIB_ASSUME(cond) ((void)(cond))

		#endif

	#endif



#else	//どのコンパイラにも引っかからなかった場合

	//______________________________________
	//									　	|
	//		ForceInlineキーワード		    |
	//______________________________________|
	#define DEF_FORCE_INLINE inline

	//______________________________________
	//									　	|
	//		[[nodiscard]]置換定義		    |
	//______________________________________|
    #define DEF_PRE_NO_DISCARD /*empty*/
    #define DEF_POST_NO_DISCARD /*empty*/

#endif



#endif //end ifndef __COMPILER_PREPROCCESSER_DEFINITIONS_H__
