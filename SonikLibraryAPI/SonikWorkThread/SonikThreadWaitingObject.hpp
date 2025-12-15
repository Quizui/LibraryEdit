#pragma once

#ifndef SONIK_THREAD_WAITING_OBJECT_H_
#define SONIK_THREAD_WAITING_OBJECT_H_

#include "ThreadImplePreProcesser.h"
#include <CompilersPreProcesser.h>
#include <SmartPointer/SonikSmartPointer.hpp>
#include <Memory/AllocateInterface.h>

#if defined(SONIK_THREAD_IMPLE_TO_CPP_STD)
#include <semaphore>
#include <chrono>
#include <cstdint>

#elif defined(SONIK_THREAD_IMPLE_TO_WINDOWS_API)
#include <Windows.h>
#include <cstdint>

	#if defined(__cplusplus) && __cplusplus >= 201103L
		#include <chrono>
	#endif


#elif defined(SONIK_THREAD_IMPLE_TO_PTHREAD_API)

#endif


//呼び出し速度重視のため、hppで定義

namespace SonikLib
{

#if defined(SONIK_THREAD_IMPLE_TO_CPP_STD)
	//C++20以上
	template <int64_t semaphore_max = INT64_MAX>
	class WorkerThreadWaitingObject
	{
	private:
		std::counting_semaphore<semaphore_max> m_sem;
		
	private:
		//コピーとムーヴの禁止
		WorkerThreadWaitingObject(const WorkerThreadWaitingObject&) = delete;
		WorkerThreadWaitingObject(WorkerThreadWaitingObject&&) = delete;
		WorkerThreadWaitingObject& operator =(const WorkerThreadWaitingObject&) = delete;
		WorkerThreadWaitingObject& operator =(WorkerThreadWaitingObject&&) = delete;

		//コンストラクタ
		WorkerThreadWaitingObject(int64_t initial = 0) noexcept
		:m_sem(initial)
		{
			//none
		};

	public:
		//デストラクタ
		~WorkerThreadWaitingObject(void)
		{
			//none
		};

		//静的クリエイタ
		DEF_FORCE_INLINE static bool CreateWaitingObject(WorkerThreadWaitingObject*& out_, int64_t initial_ = 0)
		{
			WorkerThreadWaitingObject* l_obj = new(std::nothrow) WorkerThreadWaitingObject(initial_);
			if (l_obj == nullptr)
			{
				out_ = nullptr;
				return false;
			};

			out_ = l_obj;
			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(WorkerThreadWaitingObject*& out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> allocator_, int64_t initial_ = 0)
		{
			if (allocator_.IsNullptr())
			{
				out_ = nullptr;
				return false;
			};

			void* allocateptr = allocator_->memal(sizeof(WorkerThreadWaitingObject));
			if (allocateptr == nullptr)
			{
				out_ = nullptr;
				return false;
			};

			WorkerThreadWaitingObject* l_obj = new(allocateptr) WorkerThreadWaitingObject(initial_);
			out_ = l_obj;
			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>& out_, int64_t initial_ = 0)
		{
			WorkerThreadWaitingObject* l_obj = new(std::nothrow) WorkerThreadWaitingObject(initial_);
			if (l_obj == nullptr)
			{
				return false;
			};

			if (!SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>::SmartPointerCreate(l_obj, out_))
			{
				delete l_obj;
				return false;
			};

			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>& out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> allocator_, int64_t initial_ = 0)
		{
			if (allocator_.IsNullptr())
			{
				return false;
			};

			void* allocateptr = allocator_->memal(sizeof(WorkerThreadWaitingObject));
			if (allocateptr == nullptr)
			{
				return false;
			};
			WorkerThreadWaitingObject* l_obj = new(allocateptr) WorkerThreadWaitingObject(initial_);
			if (!SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>::SmartPointerCreate(l_obj, out_, allocator_))
			{
				allocator_->memdel(l_obj);
				return false;
			};

			return true;
		};

		// blocking
		DEF_FORCE_INLINE void acquire(void) noexcept
		{
			m_sem.acquire();
		};

		// immediate try
		DEF_PRE_NO_DISCARD DEF_FORCE_INLINE bool try_acquire(void) noexcept DEF_POST_NO_DISCARD
		{
			return m_sem.try_acquire();
		};

		// chrono relative
		template <class Rep, class Period>
		DEF_FORCE_INLINE bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time)
		{
			return m_sem.try_acquire(std::chrono::duration_cast<std::chrono::milliseconds>(rel_time));
		};

		// chrono absolute
		template<class Clock = std::chrono::steady_clock, class Duration = typename Clock::duration>
		DEF_FORCE_INLINE bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time)
		{
			static_assert(Clock::is_steady, "Clock must be steady (use std::chrono::steady_clock)");
			return m_sem.try_acquire_until(abs_time);
		};

		// millisecond relative helper (platform-agnostic friendly overload)
		DEF_FORCE_INLINE bool try_acquire_for_ms(uint32_t ms)
		{
			if (ms == UINT32_MAX)
			{
				m_sem.acquire();
				return true;
			};

			return m_sem.try_acquire_for(std::chrono::milliseconds(ms));
		};

		//release(increase count)
		DEF_FORCE_INLINE void release(int32_t update = 1) noexcept
		{
			for (int32_t i = 0; i < update; ++i)
			{
				m_sem.release();
			};
		};
	};


#elif defined(SONIK_THREAD_IMPLE_TO_WINDOWS_API)
	template <long long semaphore_max = LONG_MAX>
	class WorkerThreadWaitingObject
	{
	private:
		HANDLE m_sem;

	private:
		//コピーとムーヴの禁止
		WorkerThreadWaitingObject(const WorkerThreadWaitingObject&) = delete;
		WorkerThreadWaitingObject(WorkerThreadWaitingObject&&) = delete;
		WorkerThreadWaitingObject& operator =(const WorkerThreadWaitingObject&) = delete;
		WorkerThreadWaitingObject& operator =(WorkerThreadWaitingObject&&) = delete;

		//コンストラクタ
		WorkerThreadWaitingObject(void) noexcept
		: m_sem(nullptr)
		{
			//none
		};

	public:
		//デストラクタ
		~WorkerThreadWaitingObject(void)
		{
			if (m_sem != nullptr)
			{
				CloseHandle(m_sem);
			};
		};

		//静的クリエイタ
		DEF_FORCE_INLINE static bool CreateWaitingObject(WorkerThreadWaitingObject*& out_, unsigned long long initial_ = 0)
		{
			WorkerThreadWaitingObject* l_obj = new(std::nothrow) WorkerThreadWaitingObject(initial_);
			if (l_obj == nullptr)
			{
				out_ = nullptr;
				return false;
			};

			SetLastError(0);
			l_obj->m_sem = CreateSemaphore(nullptr, initial_, semaphore_max, nullptr);
			DWORD err = GetLastError();
			if (l_obj->m_sem == nullptr)
			{
				delete l_obj;
				out_ = nullptr;
				return false;
			};

			out_ = l_obj;
			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(WorkerThreadWaitingObject*& out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> allocator_, unsigned long long initial_ = 0)
		{
			if (allocator_.IsNullptr())
			{
				out_ = nullptr;
				return false;
			};

			void* allocateptr = allocator_->memal(sizeof(WorkerThreadWaitingObject));
			if (allocateptr == nullptr)
			{
				out_ = nullptr;
				return false;
			};

			WorkerThreadWaitingObject* l_obj = new(allocateptr) WorkerThreadWaitingObject(initial_);
			
			SetLastError(0);
			l_obj->m_sem = CreateSemaphore(nullptr, initial_, semaphore_max, nullptr);
			DWORD err = GetLastError();
			if (l_obj->m_sem == nullptr)
			{
				allocator_->memdel(l_obj);
				out_ = nullptr;
				return false;
			};

			out_ = l_obj;
			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>& out_, unsigned long long initial_ = 0)
		{
			WorkerThreadWaitingObject* l_obj = new(std::nothrow) WorkerThreadWaitingObject(initial_);
			if (l_obj == nullptr)
			{
				return false;
			};

			SetLastError(0);
			l_obj->m_sem = CreateSemaphore(nullptr, initial_, semaphore_max, nullptr);
			DWORD err = GetLastError();
			if (l_obj->m_sem == nullptr)
			{
				delete l_obj;
				return false;
			};

			if (!SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>::SmartPointerCreate(l_obj, out_))
			{
				delete l_obj;

				return false;
			};

			return true;
		};
		DEF_FORCE_INLINE static bool CreateWaitingObject(SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>& out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> allocator_, unsigned long long initial_ = 0)
		{
			if (allocator_.IsNullptr())
			{
				return false;
			};

			void* allocateptr = allocator_->memal(sizeof(WorkerThreadWaitingObject));
			if (allocateptr == nullptr)
			{
				return false;
			};
			WorkerThreadWaitingObject* l_obj = new(allocateptr) WorkerThreadWaitingObject(initial_);
			
			SetLastError(0);
			l_obj->m_sem = CreateSemaphore(nullptr, initial_, semaphore_max, nullptr);
			DWORD err = GetLastError();
			if (l_obj->m_sem == nullptr)
			{
				allocator_->memdel(l_obj);
				return false;
			};

			if (!SonikLib::SharedSmtPtr<WorkerThreadWaitingObject>::SmartPointerCreate(l_obj, out_, allocator_))
			{
				allocator_->memdel(l_obj);
				return false;
			};
		};

		// blocking
		DEF_FORCE_INLINE void acquire(void) noexcept
		{
			WaitForSingleObject(m_sem, INFINITE);
		};

		// immediate try
		DEF_FORCE_INLINE DEF_NO_DISCARD bool try_acquire(void) noexcept
		{
			return WaitForSingleObject(m_sem, 0) == WAIT_OBJECT_0;
		};

		// millisecond relative helper (platform-agnostic friendly overload)
		DEF_FORCE_INLINE bool try_acquire_for_ms(uint32_t ms)
		{
			DWORD wait_ms = (ms == UINT32_MAX)
				? INFINITE
				: static_cast<DWORD>(ms);

			DWORD r = WaitForSingleObject(m_sem, wait_ms);
			return r == WAIT_OBJECT_0;
		};

#if defined(__cplusplus) && __cplusplus >= 201103L
		//C++11 Chrono Used
		// chrono relative
		template <class Rep, class Period>
		DEF_FORCE_INLINE bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time)
		{
			uint32_t ms = 0;
			std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time);
			if (milli.count() < 0)
			{
				ms = 0;

			}
			else if (milli.count() > std::numeric_limits<uint32_t>::max())
			{
				ms = UINT32_MAX;
			}
			else
			{
				ms = static_cast<uint32_t>(milli.count());
			};
				
			return try_acquire_for_ms(ms);
		};

		// chrono absolute
		template<class Clock = std::chrono::steady_clock, class Duration = typename Clock::duration>
		DEF_FORCE_INLINE bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time)
		{
			static_assert(Clock::is_steady, "Clock must be steady (use std::chrono::steady_clock)");

			Clock::time_point now = Clock::now();
			Clock::time_point rel = abs_time - now;

			std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(rel);

			if(milli.count() <= 0)
			{
				return try_acquire_for_ms(0);
			};

			const int64_t cnt = milli.count();
			if (cnt >= static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))
			{

				return try_acquire_for_ms(UINT32_MAX);
			};

			return try_acquire_for_ms(static_cast<uint32_t>(cnt));
		};
#else

		//relative
		DEF_FORCE_INLINE bool try_acquire_for(uint32_t ms)
		{
			return try_acquire_for_ms(ms);
		};

		//absolute
		// abs_ms は GetTickCount() と同じ基準（DWORD）で表した目標時刻
		DEF_FORCE_INLINE bool try_acquire_until(uint32_t abs_time)
		{
			uint32_t now = static_cast<uint32_t>(GetTickCount());
			uint32_t diff = abs_time - now;

			if (diff == UINT32_MAX)
			{
				WaitForSingleObject(m_sem, INFINITE);
				return true;
			};

			return try_acquire_for_ms(diff);
		};

#endif

		//release(increase count)
		DEF_FORCE_INLINE void release(int32_t update = 1) noexcept
		{
			ReleaseSemaphore(m_sem, update, nullptr);
		};
	};

#elif defined(SONIK_THREAD_IMPLE_TO_PTHREAD_API)
	//まだ未実装

#endif

};




#endif //SONIK_THREAD_WAITING_OBJECT_H_