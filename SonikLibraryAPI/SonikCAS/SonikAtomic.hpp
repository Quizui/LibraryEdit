#ifndef SONIKLIB_SONIKATOMIC_DEFINITION_HPP_
#define SONIKLIB_SONIKATOMIC_DEFINITION_HPP_

#include <cstdint>
#include <PlatFormDefinitions.h>
#include <CPPGrammarDefines.h>

#if defined(_MSC_VER)
    //MSVC環境
    #include <Windows.h>
    #include <winnt.h> //Interlockedに必要
#endif

namespace SonikLib
{
#if defined(SLIB_COMPILER_DEF_MSVC)
    //定義はあれど無視されます(互換性用)
    enum SlibAtomicMemoryOrder
    {
        order_relax = 0,
        order_acquire,
        order_release,
        order_seq_cst,
        order_acq_rel,
    };
#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
    enum SlibAtomicMemoryOrder
    {
        order_relax     = __ATOMIC_RELAXED,
        order_acquire   = __ATOMIC_ACQUIRE,
        order_release   = __ATOMIC_RELEASE,
        order_seq_cst   = __ATOMIC_SEQ_CST,
        order_acq_rel   = __ATOMIC_ACQ_REL,
    };
#endif

    //通常タイプのテンプレート
    //uint64_t版とポインタ版、bool版はこれより下で特殊化実装します。
    template <class atomic_type>
    class SonikAtomic
    {
    private:
#if defined(SLIB_COMPILER_DEF_MSVC)
       __declspec(align(8)) volatile int64_t m_value;
#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
       volatile int64_t m_value __attribute__((aligned(8)));
#endif

    private:
    #if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_) = delete;
		SonikAtomic(SonikAtomic&& _move_) = delete;
		SonikAtomic& operator =(const SonikAtomic& _copy_) = delete;
		SonikAtomic& operator =(SonikAtomic&& _move_) = delete;

    #else //C++ 11 以下
        //コピーと代入の禁止
		SonikAtomic(const SonikAtomic& _copy_);
		SonikAtomic& operator =(const SonikAtomic& _copy_);

        #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
            //MSVC2010ならmove可能なので定義だけしておく。
            SonikAtomic(SonikAtomic&& _copy_);
            SonikAtomic& operator =(SonikAtomic&& _copy_);

        #endif
    #endif

    public:
        //コンストラクタ
        SonikAtomic(void) SLIB_CVR_NOEXCEPT
        :m_value(0)
        {/*none*/};
        SonikAtomic(atomic_type values) SLIB_CVR_NOEXCEPT
        :m_value(static_cast<int64_t>(values))
        {/*none*/};

        //デストラクタ
        ~SonikAtomic(void) SLIB_CVR_NOEXCEPT
        {/*none*/};

        atomic_type load(SlibAtomicMemoryOrder memory_order) const SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            int64_t retvalue = InterlockedCompareExchange64(const_cast<volatile int64_t*>(reinterpret_cast<volatile int64_t const*>(&m_value)),0 , 0);
            return static_cast<atomic_type>(retvalue);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            return __atomic_load_n(&m_value, memory_order);
#endif
        };

        bool CompareExchange_Weak(atomic_type& expected, atomic_type desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile int64_t* cast_m_value = const_cast<volatile int64_t*>(reinterpret_cast<volatile int64_t const*>(&m_value));
            int64_t cast_expected = static_cast<int64_t>(expected);
            int64_t cast_desired = static_cast<int64_t>(desired);

            int64_t prev = InterlockedCompareExchange64(cast_m_value, cast_desired, cast_expected);
            if(prev == static_cast<int64_t>(cast_expected))
            {
                return true;
            };

            expected = static_cast<atomic_type>(prev);
            return false;

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            int64_t failer_order = memory_order;
            if(memory_order == SlibAtomicMemoryOrder::order_release)
            {
                failer_order = __ATOMIC_RELAXED;

            }else if(memory_order == SlibAtomicMemoryOrder::order_acq_rel)
            {
                failer_order = __ATOMIC_ACQUIRE;
            };

            atomic_type exp = expected;
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, desired, true, memory_order, failer_order);
            if(!ret)
            {
                expected = exp;
            };

            return ret;
#endif
        };

        bool CompareExchange_Strong(atomic_type& expected, atomic_type desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            return CompareExchange_Weak(expected, desired, memory_order);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            int64_t failer_order = memory_order;
            if(memory_order == SlibAtomicMemoryOrder::order_release)
            {
                failer_order = __ATOMIC_RELAXED;

            }else if(memory_order == SlibAtomicMemoryOrder::order_acq_rel)
            {
                failer_order = __ATOMIC_ACQUIRE;
            };

            int64_t exp = static_cast<int64_t>(expected);
            int64_t des = static_cast<int64_t>(desired);
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, false, memory_order, failer_order);
            if(!ret)
            {
                expected = static_cast<atomic_type>(exp);
            };

            return ret;
#endif
        };

        atomic_type Fetch_Add(atomic_type values, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
            if(values < 0)
            {
                //負の値なら処理しない。
                return load(memory_order);
            };

#if defined(SLIB_COMPILER_DEF_MSVC)
            //WindowsImplement
            volatile int64_t* cast_m_value = const_cast<volatile int64_t*>(&m_value);
            int64_t cast_values = static_cast<int64_t>(values);
            int64_t prev = InterlockedExchangeAdd64(cast_m_value, cast_values);
            
            return static_cast<atomic_type>(prev);


#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            return __atomic_fetch_add(&m_value, values, memory_order);
#endif
        };

        atomic_type Fetch_Sub(atomic_type values, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
            if(values < 0)
            {
                //負の値なら処理しない。
                return load(memory_order);
            };

#if defined(SLIB_COMPILER_DEF_MSVC)
            //Windows Implement
            //uint64_t u = (~static_cast<uint64_t>(values)) + 1ULL;

            volatile int64_t* cast_m_value = const_cast<volatile int64_t*>(&m_value);
            int64_t  cast_values = static_cast<int64_t>(values) * (-1); //符号反転
            int64_t prev = InterlockedExchangeAdd64(cast_m_value, cast_values); //マイナス方向へ加算

            return static_cast<atomic_type>(prev);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            return __atomic_fetch_sub(&m_value, values, memory_order);
#endif
        };

    };//end class
    
    //ポインタ版 の特殊化================================
    template <class atomic_type>
    class SonikAtomic<atomic_type*>
    {
    private:
#if defined(SLIB_COMPILER_DEF_MSVC)
        __declspec(align(8)) volatile uint64_t m_value;
#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
        volatile uint64_t m_value __attribute__((aligned(8)));
#endif

    private:
    #if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_) = delete;
		SonikAtomic(SonikAtomic&& _move_) = delete;
		SonikAtomic& operator =(const SonikAtomic& _copy_) = delete;
		SonikAtomic& operator =(SonikAtomic&& _move_) = delete;

    #else //C++ 11 以下
        //コピーと代入の禁止
		SonikAtomic(const SonikAtomic& _copy_);
		SonikAtomic& operator =(const SonikAtomic& _copy_);

    #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
            //MSVC2010ならmove可能なので定義だけしておく。
            SonikAtomic(SonikAtomic&& _copy_);
            SonikAtomic& operator =(SonikAtomic&& _copy_);

        #endif
    #endif

    public:
        //コンストラクタ
        SonikAtomic(void) SLIB_CVR_NOEXCEPT
        :m_value(0)
        {/*none*/};
        SonikAtomic(atomic_type* values) SLIB_CVR_NOEXCEPT
        :m_value(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(values)))
        {/*none*/};

        //デストラクタ
        ~SonikAtomic(void) SLIB_CVR_NOEXCEPT
        {/*none*/};

        atomic_type* load(SlibAtomicMemoryOrder memory_order) const SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
            int64_t retvalue = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(pv),0 , 0);
            return reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(retvalue));

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            uint64_t ret = __atomic_load_n(&m_value, memory_order);
            return reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(ret));
#endif
        };

        bool CompareExchange_Weak(atomic_type*& expected, atomic_type* desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
            volatile int64_t* cast_m_value = reinterpret_cast<volatile int64_t*>(pv);
            int64_t cast_expected = static_cast<int64_t>(reinterpret_cast<uintptr_t>(expected));
            int64_t cast_desired = static_cast<int64_t>(reinterpret_cast<uintptr_t>(desired));

            int64_t prev = InterlockedCompareExchange64(cast_m_value, cast_desired, cast_expected);
            if(prev == cast_expected)
            {
                return true;
            };

            expected = reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(prev));
            return false;

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            uint64_t exp = reinterpret_cast<uintptr_t>(expected);
            uint64_t des = reinterpret_cast<uintptr_t>(desired);
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, true, memory_order, __ATOMIC_RELAXED);
            if(!ret)
            {
                expected = reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(exp));
            };

            return ret;
#endif
        };

        bool CompareExchange_Strong(atomic_type*& expected, atomic_type* desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            return CompareExchange_Weak(expected, desired, memory_order);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            uint64_t exp = reinterpret_cast<uintptr_t>(expected);
            uint64_t des = reinterpret_cast<uintptr_t>(desired);
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, false, memory_order, __ATOMIC_RELAXED);
            if (!ret)
            {
                expected = reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(exp));
            };

            return ret;
#endif
        };

        atomic_type* Fetch_Add(uint64_t elem_size_cnt, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            //WindowsImplement
            while (true)
            {
                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
                uint64_t desired = old + static_cast<uint64_t>((sizeof(atomic_type) * elem_size_cnt));                  // unsigned wrap
                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
                if (static_cast<uint64_t>(prev) == old)
                {
                    return reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(old));
                };

                // else retry
            };

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            while (true)
            {
                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
                uint64_t desired = old + static_cast<uintptr_t>((sizeof(atomic_type) * elem_size_cnt));
                uint64_t expected = old;
                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
                {
                    return reinterpret_cast<atomic_type*>(old);
                };
            };
            // expected は新しい値に更新されるので再試行
#endif
        };

        atomic_type* Fetch_Sub(uint64_t elem_size_cnt, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            //WindowsImplement
            while (true)
            {
                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
                uint64_t desired = old - static_cast<uint64_t>((sizeof(atomic_type) * elem_size_cnt));                  // unsigned wrap
                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
                if (static_cast<uint64_t>(prev) == old)
                {
                    return reinterpret_cast<atomic_type*>(static_cast<uintptr_t>(old));
                };

                // else retry
            };

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            while (true)
            {
                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
                uint64_t desired = old - static_cast<uintptr_t>((sizeof(atomic_type) * elem_size_cnt));
                uint64_t expected = old;
                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
                {
                    return reinterpret_cast<atomic_type*>(old);
                };
            };
            // expected は新しい値に更新されるので再試行
#endif
        };

    };//end class

    //uintptr_t版の特殊化================================
//    template <>
//    class SonikAtomic<uintptr_t>
//    {
//    private:
//        alignas(8) volatile uint64_t m_value;
//
//    private:
//#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
//        //コピーと代入の禁止
//        SonikAtomic(const SonikAtomic& _copy_) = delete;
//        SonikAtomic(SonikAtomic&& _move_) = delete;
//        SonikAtomic& operator =(const SonikAtomic& _copy_) = delete;
//        SonikAtomic& operator =(SonikAtomic&& _move_) = delete;
//
//#else //C++ 11 以下
//        //コピーと代入の禁止
//        SonikAtomic(const SonikAtomic& _copy_);
//        SonikAtomic& operator =(const SonikAtomic& _copy_);
//#endif
//
//    public:
//        //コンストラクタ
//        SonikAtomic(void) SLIB_CVR_NOEXCEPT
//            : m_value(0)
//        {/*none*/
//        };
//        SonikAtomic(uintptr_t values) SLIB_CVR_NOEXCEPT
//            :m_value(static_cast<uint64_t>(values))
//        {/*none*/
//        };
//
//        //デストラクタ
//        ~SonikAtomic(void) SLIB_CVR_NOEXCEPT
//        {/*none*/
//        };
//
//        uintptr_t load(SlibAtomicMemoryOrder memory_order) const SLIB_CVR_NOEXCEPT
//        {
//#if defined(SLIB_COMPILER_DEF_MSVC)
//            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
//            int64_t retvalue = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(pv), 0, 0);
//            return static_cast<uintptr_t>(retvalue);
//
//#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
//            uint64_t ret = __atomic_load_n(&m_value, memory_order);
//            return static_cast<uintptr_t>(ret);
//#endif
//        };
//
//        bool CompareExchange_Weak(uintptr_t& expected, uintptr_t desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
//        {
//#if defined(SLIB_COMPILER_DEF_MSVC)
//            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
//            volatile int64_t* cast_m_value = reinterpret_cast<volatile int64_t*>(pv);
//            int64_t cast_expected = static_cast<int64_t>(expected);
//            int64_t cast_desired = static_cast<int64_t>(desired);
//
//            int64_t prev = InterlockedCompareExchange64(cast_m_value, cast_desired, cast_expected);
//            if (prev == cast_expected)
//            {
//                return true;
//            };
//
//            expected = static_cast<uintptr_t>(prev);
//            return false;
//
//#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
//            //posix implement
//            uint64_t exp = static_cast<uint64_t>(expected);
//            uint64_t des = static_cast<uint64_t>(desired);
//            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, true, memory_order, __ATOMIC_RELAXED);
//            if (!ret)
//            {
//                expected = static_cast<uintptr_t>(exp);
//            };
//
//            return ret;
//#endif
//        };
//
//        bool CompareExchange_Strong(uintptr_t& expected, uintptr_t desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
//        {
//#if defined(SLIB_COMPILER_DEF_MSVC)
//            return CompareExchange_Weak(expected, desired, memory_order);
//
//#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
//            //posix implement
//            uint64_t exp = static_cast<uint64_t>(expected);
//            uint64_t des = static_cast<uint64_t>(desired);
//            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, false, memory_order, __ATOMIC_RELAXED);
//            if (!ret)
//            {
//                expected = static_cast<uintptr_t>(exp);
//            };
//
//            return ret;
//#endif
//        };
//
//        uintptr_t Fetch_Add(uint64_t byte_offset, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
//        {
//#if defined(SLIB_COMPILER_DEF_MSVC)
//            //WindowsImplement
//            while (true)
//            {
//                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
//                uint64_t desired = old + byte_offset;                  // unsigned wrap
//                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
//                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
//                if (static_cast<uint64_t>(prev) == old)
//                {
//                    return static_cast<uintptr_t>(old);
//                };
//
//                // else retry
//            };
//
//#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
//            //posix implement
//            while (true)
//            {
//                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
//                uint64_t desired = old + byte_offset;
//                uint64_t expected = old;
//                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
//                {
//                    return  static_cast<uintptr_t>(old);
//                };
//            };
//            // expected は新しい値に更新されるので再試行
//#endif
//        };
//
//       uintptr_t Fetch_Sub(uint64_t byte_offset, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
//        {
//#if defined(SLIB_COMPILER_DEF_MSVC)
//            //WindowsImplement
//            while (true)
//            {
//                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
//                uint64_t desired = old - byte_offset;                  // unsigned wrap
//                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
//                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
//                if (static_cast<uint64_t>(prev) == old)
//                {
//                    return static_cast<uintptr_t>(old);
//                };
//
//                // else retry
//            };
//
//#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
//            //posix implement
//            while (true)
//            {
//                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
//                uint64_t desired = old - byte_offset;
//                uint64_t expected = old;
//                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
//                {
//                    return  static_cast<uintptr_t>(old);
//                };
//            };
//            // expected は新しい値に更新されるので再試行
//#endif
//        };
//
//    };//end class


    //uint64_t の特殊化==================================
    template <>
    class SonikAtomic<uint64_t>
    {
    private:
#if defined(SLIB_COMPILER_DEF_MSVC)
        __declspec(align(8)) volatile uint64_t m_value;
#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
        volatile uint64_t m_value __attribute__((aligned(8)));
#endif

    private:
#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_) = delete;
        SonikAtomic(SonikAtomic&& _move_) = delete;
        SonikAtomic& operator =(const SonikAtomic& _copy_) = delete;
        SonikAtomic& operator =(SonikAtomic&& _move_) = delete;

#else //C++ 11 以下
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_);
        SonikAtomic& operator =(const SonikAtomic& _copy_);

        #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
            //MSVC2010ならmove可能なので定義だけしておく。
            SonikAtomic(SonikAtomic&& _copy_);
            SonikAtomic& operator =(SonikAtomic&& _copy_);

        #endif
#endif

    public:
        //コンストラクタ
        SonikAtomic(void) SLIB_CVR_NOEXCEPT
            : m_value(0)
        {/*none*/
        };
        SonikAtomic(uint64_t values) SLIB_CVR_NOEXCEPT
            :m_value(values)
        {/*none*/
        };

        //デストラクタ
        ~SonikAtomic(void) SLIB_CVR_NOEXCEPT
        {/*none*/
        };

        uint64_t load(SlibAtomicMemoryOrder memory_order) const SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
            int64_t retvalue = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(pv), 0, 0);
            return static_cast<uint64_t>(retvalue);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            uint64_t ret = __atomic_load_n(&m_value, memory_order);
            return ret;
#endif
        };

        bool CompareExchange_Weak(uint64_t& expected, uint64_t desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile void* pv = const_cast<volatile void*>(static_cast<volatile const void*>(&m_value));
            volatile int64_t* cast_m_value = reinterpret_cast<volatile int64_t*>(pv);
            int64_t cast_expected = static_cast<int64_t>(expected);
            int64_t cast_desired = static_cast<int64_t>(desired);

            int64_t prev = InterlockedCompareExchange64(cast_m_value, cast_desired, cast_expected);
            if (prev == cast_expected)
            {
                return true;
            };

            expected = static_cast<uint64_t>(prev);
            return false;

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            uint64_t exp = expected;
            uint64_t des = desired;
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, true, memory_order, __ATOMIC_RELAXED);
            if (!ret)
            {
                expected = exp;
            };

            return ret;
#endif
        };

        bool CompareExchange_Strong(uint64_t& expected, uint64_t desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            return CompareExchange_Weak(expected, desired, memory_order);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            uint64_t exp = expected;
            uint64_t des = desired;
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, false, memory_order, __ATOMIC_RELAXED);
            if (!ret)
            {
                expected = exp;
            };

            return ret;
#endif
        };

        uint64_t Fetch_Add(uint64_t values, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            //WindowsImplement
            while (true)
            {
                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
                uint64_t desired = old + values;                  // unsigned wrap
                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
                if (static_cast<uint64_t>(prev) == old)
                {
                    return static_cast<uint64_t>(old);
                };

                // else retry
        };

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            while (true)
            {
                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
                uint64_t desired = old + values;
                uint64_t expected = old;
                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
                {
                    return  static_cast<uint64_t>(old);
                };
            };
            // expected は新しい値に更新されるので再試行
#endif
        };

        uintptr_t Fetch_Sub(uint64_t values, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            //WindowsImplement
            while (true)
            {
                uint64_t old = static_cast<uint64_t>(InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), 0, 0));
                uint64_t desired = old - values;                  // unsigned wrap
                // InterlockedCompareExchange64 の引数は LONG64 (符号付き) なので cast
                int64_t prev = InterlockedCompareExchange64(reinterpret_cast<volatile int64_t*>(&m_value), static_cast<int64_t>(desired), static_cast<int64_t>(old));
                if (static_cast<uint64_t>(prev) == old)
                {
                    return static_cast<uint64_t>(old);
                };

                // else retry
            };

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            while (true)
            {
                uint64_t old = __atomic_load_n(&m_value, __ATOMIC_RELAXED);
                uint64_t desired = old - values;
                uint64_t expected = old;
                if (__atomic_compare_exchange_n(&m_value, &expected, desired, false, /*成功時の order*/memory_order, /*失敗時の order*/__ATOMIC_RELAXED))
                {
                    return  static_cast<uint64_t>(old);
                };
            };
            // expected は新しい値に更新されるので再試行
#endif
        };

    };//end class

    //bool の特殊化======================================
    template<>
    class SonikAtomic<bool>
    {
    private:
#if defined(SLIB_COMPILER_DEF_MSVC)
        __declspec(align(4)) volatile int32_t m_value; //低位バイトを0/1で使う
#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
        volatile int32_t m_value __attribute__((aligned(4))); //低位バイトを0/1で使う
#endif

    private:
#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_) = delete;
        SonikAtomic(SonikAtomic&& _move_) = delete;
        SonikAtomic& operator =(const SonikAtomic& _copy_) = delete;
        SonikAtomic& operator =(SonikAtomic&& _move_) = delete;

#else //C++ 11 以下
        //コピーと代入の禁止
        SonikAtomic(const SonikAtomic& _copy_);
        SonikAtomic& operator =(const SonikAtomic& _copy_);

        #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600 
            //MSVC2010ならmove可能なので定義だけしておく。
            SonikAtomic(SonikAtomic&& _copy_);
            SonikAtomic& operator =(SonikAtomic&& _copy_);

        #endif
#endif

    public:
        //コンストラクタ
        SonikAtomic(void) SLIB_CVR_NOEXCEPT
            : m_value(0)
        {/*none*/
        };
        SonikAtomic(bool values) SLIB_CVR_NOEXCEPT
            :m_value(static_cast<uint8_t>(values))
        {/*none*/
        };

        //デストラクタ
        ~SonikAtomic(void) SLIB_CVR_NOEXCEPT
        {/*none*/
        };

        bool load(SlibAtomicMemoryOrder memory_order) const SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            int32_t retvalue = InterlockedCompareExchange(const_cast<volatile LONG*>(reinterpret_cast<const volatile LONG*>(&m_value)), 0, 0);
            return static_cast<bool>(retvalue & 0xFF);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
           int32_t retvalue = __atomic_load_n(&m_value, memory_order);
            return static_cast<bool>(retvalue & 0xFF);
#endif
        };

        void store(bool write_value, SlibAtomicMemoryOrder memory_order)
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            const long newLow = write_value ? 1L : 0L;
            volatile long* cast_mvalue = const_cast<volatile LONG*>(reinterpret_cast<const volatile LONG*>(&m_value));
            int32_t old = (*cast_mvalue);

            for (;;)//永続ループ
            {
                long nw = (old & ~0xFF) | newLow;
                int32_t prev = InterlockedCompareExchange(cast_mvalue, nw, old);
                if ( prev == old)
                {
                    return;
                };

                old = prev;
            };

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            const uint32_t newLow = write_value ? 1L : 0L;
            int32_t old =0;

            for (;;)//永続ループ
            {
                uint32_t nw = (old & ~0xFFu) | newLow;
                if (__atomic_compare_exchange_n(&m_value, &old, nw, false, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED))
                {
                    return;
                };
            };
#endif
        };

        bool CompareExchange_Weak(bool& expected, bool desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            volatile long* cast_m_value = const_cast<volatile LONG*>(reinterpret_cast<const volatile LONG*>(&m_value));
            int32_t cast_expected = static_cast<int32_t>(expected);
            int32_t cast_desired = static_cast<int32_t>(desired);

            int64_t prev = InterlockedCompareExchange(cast_m_value, cast_desired, cast_expected);
            if (prev == cast_expected)
            {
                return true;
            };

            expected = static_cast<bool>(prev & 0xFF);
            return false;

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            int32_t exp = static_cast<int32_t>(expected);
            int32_t des = static_cast<int32_t>(desired);
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, true, memory_order, __ATOMIC_RELAXED);
            if (!ret)
            {
                expected = static_cast<bool>(exp & 0xFF);;
            };

            return static_cast<bool>(ret & 0xFF);;
#endif
        };

        bool CompareExchange_Strong(bool& expected, bool desired, SlibAtomicMemoryOrder memory_order) SLIB_CVR_NOEXCEPT
        {
#if defined(SLIB_COMPILER_DEF_MSVC)
            return CompareExchange_Weak(expected, desired, memory_order);

#elif defined(SLIB_COMPILER_DEF_CLANG) || defined(SLIB_COMPILER_DEF_GCC)
            //posix implement
            int32_t exp = static_cast<int32_t>(expected);
            int32_t des = static_cast<int32_t>(desired);
            bool ret = __atomic_compare_exchange_n(&m_value, &exp, des, false, memory_order, __ATOMIC_RELAXED);
            if (!ret)
            {
                expected = static_cast<bool>(exp & 0xFF);;
            };

            return static_cast<bool>(ret & 0xFF);;
#endif
        };

    };//end class


}; //end namespace


#endif //SONIKLIB_SONIKATOMIC_DEFINITION_HPP_