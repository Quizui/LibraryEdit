//0～連番で番号を排出し、管理します。
//返却された番号が最大値より前にあればそれを返却します。

#ifndef __SONIK_ASSIGN_FEEE_NUMBER_CLASS_HEADER__
#define __SONIK_ASSIGN_FEEE_NUMBER_CLASS_HEADER__

#include <stdint.h>
#include "../SmartPointer/SonikSmartPointer.hpp"
#include "../SonikCAS/SonikAtomicLock.h"

//前方宣言
namespace SonikLib
{
    namespace Container
    {
        template <class T>
        class SonikVariableArrayContainer;
    };
};


namespace SonikLib
{
    class SonikAssignFreeNumber
    {
    private:
        SonikLib::S_CAS::SonikAtomicLock m_lock;
        SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<uint64_t>> m_bitheader;
        SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<uint64_t>> m_bitvalue;

        //アロケータ
        SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

    private:
        SonikAssignFreeNumber(void);


#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
        //コピーと代入の禁止
        SonikAssignFreeNumber(const SonikAssignFreeNumber& _copy_) = delete;
        SonikAssignFreeNumber(SonikAssignFreeNumber&& _copy_) = delete;
        SonikAssignFreeNumber& operator =(const SonikAssignFreeNumber& _copy_) = delete;
        SonikAssignFreeNumber& operator =(SonikAssignFreeNumber&& _copy_) = delete;

#else //C++ 11 以下
        //コピーと代入の禁止
        SonikAssignFreeNumber(const SonikAssignFreeNumber& _copy_);
        SonikAssignFreeNumber& operator =(const SonikAssignFreeNumber& _copy_);

    #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
        //MSVC2010ならmove可能なので定義だけしておく。
        SonikAssignFreeNumber(SonikAssignFreeNumber&& _copy_);
        SonikAssignFreeNumber& operator =(SonikAssignFreeNumber&& _copy_);

    #endif
#endif


    public:
        ~SonikAssignFreeNumber(void);

        //通常のオブジェクトを生成します。
        static bool CreateObject(SonikLib::SharedSmtPtr<SonikAssignFreeNumber>& _get_);
        static bool CreateObject(SonikLib::SharedSmtPtr<SonikAssignFreeNumber>& _get_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

        //シングルトンオブジェクトとして生成します。
        static SonikAssignFreeNumber& SingletonCreate(void);

        //空き番号の貸出
        //最大値はint64_t型で0x7FFFFFFFFFFFFFFF(実値:9223372036854775807)
        //異常値は負の値
        int64_t LendNumber(void);

        //空き番号の返還
        void ReturnNumber(int64_t _lendnumber_);

    };


};



#endif //__SONIK_ASSIGN_FEEE_NUMBER_CLASS_HEADER__