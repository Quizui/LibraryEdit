
#ifndef TIMERCLOCK_SONIKELAPSEDTIMER_H_
#define TIMERCLOCK_SONIKELAPSEDTIMER_H_

//時間計測と一時的な処理中断を行えるクラスです。
//Windows環境の場合のみ winmm.lib を先にリンクさせる必要があります。(内部でtimeBeginPeriodを使っていてそれがwinmmに依存します。)

//timeBeginPeriodはWindows環境でのIntervalSleep関数を使うのに使用されています。
//これを使わないとスリープ時間の最低値が15ms~17msくらいになるため、スリープ時間を最低の1msにしています。
//逆にいえばWindows環境においてはIntervalSleep関数においてのみ精度が1ms以下にならず、マイクロ秒クラスを使っても一番の最高解像度が1msとなる点に注意してください。
//それ以外の関数については記載どおりマイクロ秒単位で計測可能です。

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
#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
		//コピーと代入の禁止
		SonikElapsedTimer(const SonikElapsedTimer& _copy_) = delete;
		SonikElapsedTimer(SonikElapsedTimer&& _move_) = delete;
		SonikElapsedTimer& operator =(const SonikElapsedTimer& _copy_) = delete;
		SonikElapsedTimer& operator =(SonikElapsedTimer&& _move_) = delete;

#else //C++ 11 以下
		//コピーと代入の禁止
		SonikElapsedTimer(const SonikElapsedTimer& _copy_);
		SonikElapsedTimer& operator =(const SonikElapsedTimer& _copy_);

	#if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
		//MSVC2010ならmove可能なので定義だけしておく。
		SonikElapsedTimer(SonikElapsedTimer&& _move_);
		SonikElapsedTimer& operator =(SonikElapsedTimer&& _move_);

	#endif
#endif

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
#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
		//コピーと代入の禁止
		SonikElapsedTimerMicro(const SonikElapsedTimerMicro& _copy_) = delete;
		SonikElapsedTimerMicro(SonikElapsedTimerMicro&& _move_) = delete;
		SonikElapsedTimerMicro& operator =(const SonikElapsedTimerMicro& _copy_) = delete;
		SonikElapsedTimerMicro& operator =(SonikElapsedTimerMicro&& _move_) = delete;

#else //C++ 11 以下
		//コピーと代入の禁止
		SonikElapsedTimerMicro(const SonikElapsedTimerMicro& _copy_);
		SonikElapsedTimerMicro& operator =(const SonikElapsedTimerMicro& _copy_);

	#if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
		//MSVC2010ならmove可能なので定義だけしておく。
		SonikElapsedTimerMicro(SonikElapsedTimerMicro&& _move_);
		SonikElapsedTimerMicro& operator =(SonikElapsedTimerMicro&& _move_);

	#endif
#endif

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