/*
 * SonikElapsedTimer.h
 *
 *  Created on: 2016/03/03
 *      Author: SONIK
 */

#ifndef TIMERCLOCK_SONIKELAPSEDTIMER_H_
#define TIMERCLOCK_SONIKELAPSEDTIMER_H_

//C++11以降でしか使用できません。

#include <cstdint>

namespace SonikLib
{
	//ある時間からの経過時間を取得するクラスです。
	class SonikElapsedTimer
	{
	private:
		uint64_t StartTime;

		//インターバル時間
		uint64_t IntervalTime;

	private:
		//代入演算子
		SonikElapsedTimer& operator =(SonikElapsedTimer& t_his);

	public:
		//コンストラクタ
		SonikElapsedTimer(void);

		//デストラクタ
		~SonikElapsedTimer(void);

		//インターバル時間を設定します。
		//引数1: インターバルとして設定する時間をミリ秒で指定します。
		//補足: 本クラスにおける、インターバル時間は、その時間間隔が過ぎたかどうか。を判定するのに仕様します。
		//		0(Default値)を指定した場合、常に GetIntervalOverメソッドは、falseを返却します。
		void SetIntervalTime(uint64_t Set_millisec);

		//計測開始時間を設定します。
		void SetStartTime(void);

		//計測開始時間からの差分を取得します。(ミリ秒)
		uint64_t GetElapsedTime(void);

		//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
		//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
		//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
		bool GetIntervalOver(void);

		//インターバル時間が過ぎるまで現在のスレッドをスリープします。
		void IntervalSleep(void);

		//指定時間現在のスレッドをスリープします。(単位：ミリ秒)
		void SleepThis(uint64_t _sleep_millisec_);
	};

	//単位：マイクロ秒バージョン
	class SonikElapsedTimerMicro
	{
	private:
		uint64_t StartTime;

		//インターバル時間
		uint64_t IntervalTime;

	private:
		//代入演算子
		SonikElapsedTimerMicro& operator =(SonikElapsedTimerMicro& t_his);

	public:
		//コンストラクタ
		SonikElapsedTimerMicro(void);

		//デストラクタ
		~SonikElapsedTimerMicro(void);

		//インターバル時間を設定します。
		//引数1: インターバルとして設定する時間をミリ秒で指定します。
		//補足: 本クラスにおける、インターバル時間は、その時間間隔が過ぎたかどうか。を判定するのに仕様します。
		//		0(Default値)を指定した場合、常に GetIntervalOverメソッドは、falseを返却します。
		void SetIntervalTime(uint64_t Set_microsec);

		//計測開始時間を設定します。
		void SetStartTime(void);

		//計測開始時間からの差分を取得します。(マイクロ秒)
		uint64_t GetElapsedTime(void);

		//計測開始時間から、インターバル時間を過ぎているかどうかを確認します。
		//インターバル時間を設定した場合、そのインターバル時間を過ぎていれば trueを返却します。
		//default値( 0 ) が設定されていた場合は、本関数は常にfalseを返却します。
		bool GetIntervalOver(void);

		//インターバル時間が過ぎるまで現在のスレッドをスリープします。
		void IntervalSleep(void);

		//指定時間現在のスレッドをスリープします。(単位：マイクロ秒)
		void SleepThis(uint64_t _sleep_microsec_);
	};
};



#endif /* TIMERCLOCK_SONIKELAPSEDTIMER_H_ */
