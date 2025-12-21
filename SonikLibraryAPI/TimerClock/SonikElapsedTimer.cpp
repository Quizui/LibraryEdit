
#include "SonikElapsedTimer.h"

#include <CPPGrammarDefines.h>
#include <PlatFormDefinitions.h>
#include <CompilersPreProcesser.h>

#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
//Windows環境
#include <windows.h>
#include <mmsystem.h> //timeBeginPeriod / timeEndPeriod

#elif defined(SLIB_PLATFORM_DEFS_POSIX)
//POSIC環境
#include <time.h>
#include <unistd.h>

#endif


namespace SonikLib
{

	// 内部共通関数：OS別の高精度カウント取得
	static DEF_FORCE_INLINE uint64_t GetNativeCount(bool isMicro)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		static double freq = 0;
		if (freq == 0)
		{
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);
			freq = static_cast<double>(li.QuadPart);
		};

		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		// 秒単位に変換してから必要単位へ(精度維持のためdouble計算)
		if (isMicro)
		{
			return static_cast<uint64_t>((count.QuadPart * 1000000.0) / freq);
		};

		return static_cast<uint64_t>((count.QuadPart * 1000.0) / freq);

#elif defined(SLIB_PLATFORM_DEFS_POSIX)
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		if (isMicro)
		{
			return (static_cast<uint64_t>(ts.tv_sec) * 1000000) + (ts.tv_nsec / 1000);
		};

		return (static_cast<uint64_t>(ts.tv_sec) * 1000) + (ts.tv_nsec / 1000000);
#endif
	};


	//コンストラクタ
	SonikElapsedTimer::SonikElapsedTimer(void)
		:StartTime(0)
		, IntervalTime(0)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		::timeBeginPeriod(1); // 精度向上
#endif
	};

	//デストラクタ
	SonikElapsedTimer::~SonikElapsedTimer(void)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		::timeEndPeriod(1); // 設定解除
#endif
	};

	//インターバル時間を設定します。
	//引数1: インターバルとして設定する時間をミリ秒で指定します。
	//補足: 本クラスにおける、インターバル時間は、その時間間隔が過ぎたかどうか。を判定するのに仕様します。
	//		0(Default値)を指定した場合、常に GetIntervalOverメソッドは、falseを返却します。
	void SonikElapsedTimer::SetIntervalTime(uint64_t Set_millisec)
	{
		IntervalTime = Set_millisec;
	};

	//計測開始時間を設定します。
	void SonikElapsedTimer::SetStartTime(void)
	{

		StartTime = GetNativeCount(false);
	};

	//計測開始時間からの差分を取得します。(ミリ秒)
	uint64_t SonikElapsedTimer::GetElapsedTime(void)
	{

		return GetNativeCount(false) - StartTime;
	};

	//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
	//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
	//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
	bool SonikElapsedTimer::GetIntervalOver(void)
	{

		if (IntervalTime == 0)
		{
			return false;
		};

		return GetElapsedTime() >= IntervalTime;
	};

	void SonikElapsedTimer::IntervalSleep(void)
	{
		uint64_t elapsed = GetElapsedTime();
		if (IntervalTime > elapsed)
		{
			SleepThis(IntervalTime - elapsed);
		};
	};

	//指定時間現在のスレッドをスリープします。(単位：ミリ秒)
	void SonikElapsedTimer::SleepThis(uint64_t _sleep_millisec_)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		::Sleep(static_cast<DWORD>(_sleep_millisec_));
#else
		usleep(static_cast<useconds_t>(_sleep_millisec_ * 1000));
#endif
	};


	//===============================================================================
	//Micro秒バージョン
	//===============================================================================
	//コンストラクタ
	SonikElapsedTimerMicro::SonikElapsedTimerMicro(void)
		:StartTime(0)
		, IntervalTime(0)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		::timeBeginPeriod(1);
#endif
	};

	//デストラクタ
	SonikElapsedTimerMicro::~SonikElapsedTimerMicro(void)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		::timeEndPeriod(1);
#endif
	};

	//インターバル時間を設定します。
	//引数1: インターバルとして設定する時間をミリ秒で指定します。
	//補足: 本クラスにおける、インターバル時間は、その時間間隔が過ぎたかどうか。を判定するのに仕様します。
	//		0(Default値)を指定した場合、常に GetIntervalOverメソッドは、falseを返却します。
	void SonikElapsedTimerMicro::SetIntervalTime(uint64_t Set_microsec)
	{
		IntervalTime = Set_microsec;
	};

	//計測開始時間を設定します。
	void SonikElapsedTimerMicro::SetStartTime(void)
	{
		StartTime = GetNativeCount(true);
	};

	//計測開始時間からの差分を取得します。(マイクロ秒)
	uint64_t SonikElapsedTimerMicro::GetElapsedTime(void)
	{
		return GetNativeCount(true) - StartTime;
	};

	//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
	//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
	//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
	bool SonikElapsedTimerMicro::GetIntervalOver(void)
	{
		if (IntervalTime == 0)
		{
			return false;
		};

		return GetElapsedTime() >= IntervalTime;
	};

	void SonikElapsedTimerMicro::IntervalSleep(void)
	{
		uint64_t elapsed = GetElapsedTime();
		if (IntervalTime > elapsed)
		{
			SleepThis(IntervalTime - elapsed);
		};

	};

	//指定時間現在のスレッドをスリープします。(単位：ミリ秒)
	void SonikElapsedTimerMicro::SleepThis(uint64_t _sleep_microsec_)
	{
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
		// Windowsのスリープは最小1ms単位のため切り捨て/四捨五入などで調整
		::Sleep(static_cast<DWORD>(_sleep_microsec_ / 1000));
#else
		usleep(static_cast<useconds_t>(_sleep_microsec_));
#endif
	};


};