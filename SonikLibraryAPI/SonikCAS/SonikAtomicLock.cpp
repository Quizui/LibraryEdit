
#include "SonikAtomicLock.h"
#include <cstdint>


//<thread>が使えるか？
#if defined(__has_include)
	//has_includeが使える
#if __has_include(<thread>)
#define AVAILABLE_STD_THREAD_HEADER
#endif

#else
	#if defined(__cplusplus) && __cplusplus >= 201103L
		#define AVAILABLE_STD_THREAD_HEADDER
	#endif
#endif

//stdのthreadが使えるならinclude
#if defined(AVAILABLE_STD_THREAD_HEADER)
	#include <thread>

#endif

//コンパイラの組み込みが利用できるならそっちもインクルードしておく。
#if defined(_MSC_VER)
	#include <intrin.h>

#elif defined(__GNUC__) || defined(__clang__)
	#include <immintrin.h>

#endif

//最後の砦はOS別インクルード
#if !(defined(AVAILABLE_STD_THREAD_HEADER)) //<thread>が使えない状態であれば。
	#if defined(_WIN32) || defined(_WIN64)
		//Windows環境が使えるならWindowsを使う
		#include <Windows.h>
	
	#else
		//Posix系と判定
		#include <sched.h>
		#include <unistd.h>
	
	#endif
#endif


namespace SonikLib
{
	namespace S_CAS
	{
		//コンストラクタ
		SonikAtomicLock::SonikAtomicLock(void)
			:_lock(false)
		{

		};

		//デストラクタ
		SonikAtomicLock::~SonikAtomicLock(void)
		{

		};

		//ロックの開始
		//ロックの権利が取得出来た場合に処理が返されます。
		void SonikAtomicLock::lock(void)
		{
			bool a_right = false;
			const int32_t l_spin_befor_backoff = 128;
			const int32_t l_max_backoff_shift = 7; //最大2^7 = 128回のpause
			int32_t l_spin_iters = 0;

			//権利が false になるまでビジーループ
			//while (!_lock.compare_exchange_strong(a_right, true, std::memory_order_acq_rel))
			while (1)
			{
				a_right = false;

				if (_lock.CompareExchange_Strong(a_right, true, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
				{
					//ロック取得成功。
					return; //処理返却
				};

				// スピンフェーズ: 短期間は busy-wait + PAUSE
				if (l_spin_iters < l_spin_befor_backoff)
				{
#if defined(_MSC_VER)
					YieldProcessor();
#elif defined(__GNUC__) || defined(__clang__)
					_mm_pause();
#else
					asm volatile("" ::: "memory");
#endif
					++l_spin_iters;
					continue;
				};

				// エクスポネンシャルバックオフ: pause を複数回実行
				{
					int shift = l_spin_iters - l_spin_befor_backoff;
					if (shift > l_max_backoff_shift)
					{
						shift = l_max_backoff_shift;
					};

					int backoff = 1 << shift;
					for (int i = 0; i < backoff; ++i)
					{
#if defined(_MSC_VER)
						YieldProcessor();

#elif defined(__GNUC__) || defined(__clang__)
						_mm_pause();

#else

						asm volatile("" ::: "memory");
#endif
					};

					++l_spin_iters;
				};


				// 長引く場合は OS に譲る
#if defined(AVAILABLE_STD_THREAD_HEADER)

				std::this_thread::yield();

#elif defined(_WIN32) || defined(_WIN64)
				if (!::SwitchToThread())
				{
					::Sleep(0);
				};
#else
				sched_yield();

#endif
				// 再試行
			};

		};

		//ロックが取得出来た場合はtrue
		//できなかった場合はfalseを返却します。
		bool SonikAtomicLock::try_lock(void)
		{
			bool a_right = false;

			//権利が取れなければfalseを返却
			if (_lock.CompareExchange_Strong(a_right, true, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				return true;
			};

			return false;
		};

		//ロックの終了
		void SonikAtomicLock::unlock(void)
		{
			_lock.store(false, SonikLib::SlibAtomicMemoryOrder::order_release);
		};

	}; //end namespace CAS

}; //end namespace SonikLib