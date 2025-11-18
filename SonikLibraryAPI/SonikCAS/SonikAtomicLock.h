#pragma once

#ifndef SONIKATOMICBLOCK_H_
#define SONIKATOMICBLOCK_H_

//#include <atomic>
#include "./SonikAtomic.hpp"

namespace SonikLib
{
	namespace S_CAS
	{
		class SonikAtomicLock
		{
		private:
			//std::atomic<bool> _lock;
			SonikLib::SonikAtomic<bool> _lock;

		private:

#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
			//コピーと代入の禁止
			//コピーコンストラクタ
			SonikAtomicLock(const SonikAtomicLock& _copy_) = delete;
			SonikAtomicLock(SonikAtomicLock&& _move_) = delete;
			//代入演算子
			SonikAtomicLock& operator =(const SonikAtomicLock& _copy_) = delete;
			SonikAtomicLock& operator =(SonikAtomicLock&& _move_) = delete;

#else //C++ 11 以下
			//コピーと代入の禁止
			SonikAtomicLock(const SonikAtomicLock& _copy_);
			SonikAtomicLock& operator =(const SonikAtomicLock& _copy_);

			#if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
				//MSVC2010ならmove可能なので定義だけしておく。
				SonikAtomicLock(SonikAtomicLock&& _move_);
				SonikAtomicLock& operator =(SonikAtomicLock&& _move_);

			#endif
#endif
		public:
			//コンストラクタ
			SonikAtomicLock(void);

			//デストラクタ
			~SonikAtomicLock(void);

			//ロックの開始
			//ロックの権利が取得出来た場合に処理が返されます。
			void lock(void);

			//ロックが取得出来た場合はtrue
			//できなかった場合はfalseを返却します。
			bool try_lock(void);

			//ロックの終了
			void unlock(void);
		};

	}; // end namespace S_CAS
}; //end namespace SonikLib


#endif /* SONIKATOMICBLOCK_H_ */