#pragma once

#ifndef SONIK_THREAD_IMPLE_PREPROCESSERS_DEFINE_TYPE_HEDDERS_
#define SONIK_THREAD_IMPLE_PREPROCESSERS_DEFINE_TYPE_HEDDERS_

//本ヘッダはコンパイル時にThreadの実装をコンパイル環境別に切り替えるためのdefine条件を定義するファイルであり、
//外部(Used)には公開(配布)しない、する必要がない。

//C++20ならセマフォベースのstdのthread実装を使用
#if defined(__cplusplus) && __cplusplus >= 202002L
	//C++20以上ならチェックの上使う
	//__has_include が使えるなら<thread>使用可能かで判別
	#if defined(__has_include)
		#if __has_include(<thread>)
			//C++11threadが使えるなら使う
			#define SONIK_THREAD_IMPLE_TO_CPP_STD
			//#define SONIK_THREAD_IMPLE_TO_WINDOWS_API //実装用に無理やり定義
		#endif

	#else //コンパイラ？IDE?に__has_include が定義されていない場合は推定で使用可能かどうかを判別

		//MSVCの場合、2012 以降なら使える事が多いので最低必須バージョンとする
		#if defined(_MSC_VER) && _MSC_VER >= 1930
			#define SONIK_THREAD_IMPLE_TO_CPP_STD

		//clang はバージョン3.3からいけるっぽいのでそこを最低必須バージョンとする。
		#elif defined(__clang__) && (__clang_major__ >= 11)
			#define SONIK_THREAD_IMPLE_TO_CPP_STD

		//GCCなら4.8以降で使えるっぽいのでそこを最低必須バージョンとする。
		#elif defined(__GNUC__) && (__GNUC__ >= 10)
			#define SONIK_THREAD_IMPLE_TO_CPP_STD

		#endif

	#endif

#else //C++20未満ならOS実装を使う方向にする
	#if defined(_WIN64) || defined_(_WIN32) //基本は64bitベースに作成しているため32bitで動くかはしらん！
		//C++11未満でWindowsならWindowsAPIThreaadの使用をする。
		#define SONIK_THREAD_IMPLE_TO_WINDOWS_API

	#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
		//C++11未満でLinuxorUnixならpthreadの使用をする(macもPOSIXらしいので追加)
		#define SONIK_THREAD_IMPLE_TO_PTHREAD_API

	#endif

#endif


#endif //SONIK_THREAD_IMPLE_PREPROCESSERS_DEFINE_TYPE_HEDDERS_