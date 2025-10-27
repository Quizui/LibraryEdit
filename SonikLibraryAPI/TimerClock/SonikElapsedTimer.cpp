/*
 * SonikElapsedTimer.cpp
 *
 *  Created on: 2016/03/03
 *      Author: SONIK
 */

#ifdef __SONIK_x86_64__

#include <chrono>
#include <thread>
#include "SonikElapsedTimer.h"

namespace SonikLib
{
	//コンストラクタ
	SonikElapsedTimer::SonikElapsedTimer(void)
	:StartTime(0)
	,IntervalTime(0)
	{

	};

	//デストラクタ
	SonikElapsedTimer::~SonikElapsedTimer(void)
	{
		//no process;
	};

	//代入演算子
	SonikElapsedTimer& SonikElapsedTimer::operator =(SonikElapsedTimer& t_his)
	{
		//no called faunction
		//コールされないので、実装なし。
		return (*this);
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
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		StartTime = mill.count();
	};

	//計測開始時間からの差分を取得します。(ミリ秒)
	uint64_t SonikElapsedTimer::GetElapsedTime(void)
	{
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		return mill.count() - StartTime;
	};

	//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
	//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
	//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
	bool SonikElapsedTimer::GetIntervalOver(void)
	{
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		if( IntervalTime == 0 )
		{
			return false;
		};

		if( IntervalTime < (mill.count() - StartTime) )
		{
			return true;
		};

		return false;
	};

	void SonikElapsedTimer::IntervalSleep(void)
	{
		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		if( IntervalTime == 0 )
		{
			return;
		};

		unsigned long long now_interval = (mill.count() - StartTime);

		if( IntervalTime > now_interval )
		{
			std::this_thread::sleep_for(std::chrono::milliseconds( IntervalTime - now_interval));
		};

	};

	//指定時間現在のスレッドをスリープします。(単位：ミリ秒)
	void SonikElapsedTimer::SleepThis(uint64_t _sleep_millisec_)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(_sleep_millisec_));
	};


	//===============================================================================
	//Micro秒バージョン
	//===============================================================================
	//コンストラクタ
	SonikElapsedTimerMicro::SonikElapsedTimerMicro(void)
	:StartTime(0)
	,IntervalTime(0)
	{

	};

	//デストラクタ
	SonikElapsedTimerMicro::~SonikElapsedTimerMicro(void)
	{
		//no process;
	};

	//代入演算子
	SonikElapsedTimerMicro& SonikElapsedTimerMicro::operator =(SonikElapsedTimerMicro& t_his)
	{
		//no called faunction
		//コールされないので、実装なし。
		return (*this);
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
		std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		StartTime = mic.count();
	};

	//計測開始時間からの差分を取得します。(マイクロ秒)
	uint64_t SonikElapsedTimerMicro::GetElapsedTime(void)
	{
		std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		return mic.count() - StartTime;
	};

	//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
	//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
	//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
	bool SonikElapsedTimerMicro::GetIntervalOver(void)
	{
		std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		if( IntervalTime == 0 )
		{
			return false;
		};

		if( IntervalTime < (mic.count() - StartTime) )
		{
			return true;
		};

		return false;
	};

	void SonikElapsedTimerMicro::IntervalSleep(void)
	{
		std::chrono::microseconds mic = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );

		if( IntervalTime == 0 )
		{
			return;
		};

		unsigned long long now_interval = (mic.count() - StartTime);

		if( IntervalTime > now_interval )
		{
			std::this_thread::sleep_for(std::chrono::microseconds( IntervalTime - now_interval));
		};

	};

	//指定時間現在のスレッドをスリープします。(単位：ミリ秒)
	void SonikElapsedTimerMicro::SleepThis(uint64_t _sleep_microsec_)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(_sleep_microsec_));
	};


};

#endif
