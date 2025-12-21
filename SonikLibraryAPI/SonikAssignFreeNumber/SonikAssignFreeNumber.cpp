#include "SonikAssignFreeNumber.h"

#include <new>

#include <Container/RangedForContainer.hpp>
#include <MathBit/MathBit.hpp>

namespace SonikLib
{
    SonikAssignFreeNumber::SonikAssignFreeNumber(void)
    {
        //initialize membar variable only
    };

    SonikAssignFreeNumber::~SonikAssignFreeNumber(void)
    {
        //noprocess
    };

    //Create
    bool SonikAssignFreeNumber::CreateObject(SonikLib::SharedSmtPtr<SonikAssignFreeNumber>& _get_)
    {
        //アロケータ生成
        SonikLib::SLibAllocateInterface* l_defalloc = new(std::nothrow) SonikLib::SLibAllocateInterface;
        if (l_defalloc == nullptr)
        {
            return false;
        };

        SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
        if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, l_allocsmtptr))
        {
            delete l_defalloc;
            return false;
        };

        void* l_allocbuffer = l_defalloc->memal(sizeof(SonikAssignFreeNumber));
        if (l_allocbuffer == nullptr)
        {
            return false;
        };

        SonikAssignFreeNumber* l_object = new(l_allocbuffer) SonikAssignFreeNumber;

        //Create bitheader
        if (!SonikLib::Container::SonikVariableArrayContainer<uint64_t>::CreateContainer(l_object->m_bitheader, l_allocsmtptr, 1))
        {
            l_object->~SonikAssignFreeNumber();
            l_defalloc->memdel(l_object);
            return false;
        };

        if (!SonikLib::Container::SonikVariableArrayContainer<uint64_t>::CreateContainer(l_object->m_bitvalue, l_allocsmtptr, 1))
        {
            l_object->~SonikAssignFreeNumber();
            l_defalloc->memdel(l_object);
            return false;
        };

        uint64_t l_bitvalue = 0;
        l_object->m_bitheader->PushBack(l_bitvalue);
        l_object->m_bitvalue->PushBack(l_bitvalue);
        l_object->m_allocator = l_allocsmtptr;

        if (!SonikLib::SharedSmtPtr<SonikAssignFreeNumber>::SmartPointerCreate(l_object, _get_, l_allocsmtptr))
        {
            l_object->~SonikAssignFreeNumber();
            l_defalloc->memdel(l_object);
            return false;
        };

        return true;
    };
    bool SonikAssignFreeNumber::CreateObject(SonikLib::SharedSmtPtr<SonikAssignFreeNumber>& _get_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
    {

        void* l_allocbuffer = _allocator_->memal(sizeof(SonikAssignFreeNumber));
        if (l_allocbuffer == nullptr)
        {
            return false;
        };

        SonikAssignFreeNumber* l_object = new(l_allocbuffer) SonikAssignFreeNumber;

        //Create bitheader
        if (!SonikLib::Container::SonikVariableArrayContainer<uint64_t>::CreateContainer(l_object->m_bitheader, _allocator_, 1))
        {
            l_object->~SonikAssignFreeNumber();
            _allocator_->memdel(l_object);
            return false;
        };

        if (!SonikLib::Container::SonikVariableArrayContainer<uint64_t>::CreateContainer(l_object->m_bitvalue, _allocator_, 1))
        {
            l_object->~SonikAssignFreeNumber();
            _allocator_->memdel(l_object);
            return false;
        };

        uint64_t l_bitvalue = 0;
        l_object->m_bitheader->PushBack(l_bitvalue);
        l_object->m_bitvalue->PushBack(l_bitvalue);
        l_object->m_allocator = _allocator_;

        if (!SonikLib::SharedSmtPtr<SonikAssignFreeNumber>::SmartPointerCreate(l_object, _get_, _allocator_))
        {
            l_object->~SonikAssignFreeNumber();
            _allocator_->memdel(l_object);
            return false;
        };

        return true;
    };
    
    //SingletonCreate
    bool SonikAssignFreeNumber::SingletonCreate(SonikLib::SharedSmtPtr<SonikAssignFreeNumber>& _get_)
    {
        static SonikLib::SharedSmtPtr<SonikAssignFreeNumber> _static_smtptr_;

        if(!_static_smtptr_.IsNullptr())
        {
            _get_ = _static_smtptr_;
            return true;
        };

        bool ret = SonikLib::SonikAssignFreeNumber::CreateObject(_static_smtptr_);

        _get_ = _static_smtptr_;
        return ret;

    };


    //空き番号の貸出
    //最大値はint64_t型で0x7FFFFFFFFFFFFFFF(実値:9223372036854775807)
    //異常値は負の値
    int64_t SonikAssignFreeNumber::LendNumber(void)
    {
        uint64_t l_header_index = 0; //ヘッダ何周目か？
        int64_t l_header_lsb = -1; //LSB位置

        m_lock.lock();

        SonikLib::Container::SonikVariableArrayContainer<uint64_t>::VACIterator l_itr(m_bitheader->begin());
        SonikLib::Container::SonikVariableArrayContainer<uint64_t>::VACIterator l_itr_end(m_bitheader->end());

        while (l_itr != l_itr_end)
        {
            l_header_lsb = SonikMathBit::GetZEROLSB((*l_itr));
            if (l_header_lsb != -1) //ビット列に0があれば利用可能
            {
                break;
            };

            ++l_header_index;
            ++l_itr;
        };

        if (l_header_lsb == -1)
        {
            //空き無し
            return -1;
        };

        //空き番号があればそれを利用
        uint64_t bitvalueindex = (64 * l_header_index) + static_cast<uint64_t>(l_header_lsb);

        int64_t l_bitzerolsb = SonikMathBit::GetZEROLSB((*m_bitvalue)[bitvalueindex]);
        if(l_bitzerolsb == -1)
        {
            //空き番号無し。
            m_lock.unlock();
            return -1;
        };
        //オーバーフローチェック
        uint64_t l_blockpoint = static_cast<uint64_t>(l_bitzerolsb);
        if ( l_blockpoint > (0x7FFFFFFFFFFFFFFFull / 64ull) )
        {
            m_lock.unlock();
            return -1;
        };
        l_blockpoint = bitvalueindex * 64ull + l_blockpoint; //変数再利用
        if(l_blockpoint > 0x7FFFFFFFFFFFFFFFull)
        {
            m_lock.unlock();
            return -1;
        };

        //ビット操作
        uint64_t l_mask = (1ULL << static_cast<uint64_t>(l_bitzerolsb));
        (*m_bitvalue)[bitvalueindex] |= l_mask;

        //操作の結果ビットがすべて1になったらヘッダのビットを1(配下が満タン)状態にする。
        if ((*m_bitvalue)[bitvalueindex] == 0xFFFFFFFFFFFFFFFFull)
        {
            //列のビットをON(1)にしてそこのビット列はすべてONであることを示す。
            l_mask = (1ULL << static_cast<uint64_t>(l_header_lsb));
            (*l_itr) |= l_mask;

            uint64_t l_newvalue = 0;

            if ((*l_itr) == 0xFFFFFFFFFFFFFFFFull)
            {
               //ヘッダがMAXなら、ヘッダをPushBack
               m_bitheader->PushBack(l_newvalue);
            };

            //新しいビット列用オブジェクトをPushBack
            m_bitvalue->PushBack(l_newvalue);

        };

        m_lock.unlock();
        return l_blockpoint;
    };

    //空き番号の返還
    void SonikAssignFreeNumber::ReturnNumber(int64_t _lendnumber_)
    {
        if (_lendnumber_ < 0 || 0x7FFFFFFFFFFFFFFF < _lendnumber_)
        {
            // 帰し番号が0より下、つまりマイナス値なら何もしない。
            // 0x7FFFFFFFFFFFFFFF以上の番号は取得させないので何もしない。
            return; 
        };

        m_lock.lock();

        //返却された番号のフラグ位置を検索
        uint64_t l_bitvalueindex = static_cast<uint64_t>(_lendnumber_) >> 6;
        uint64_t l_headerindex = l_bitvalueindex >> 6;
        uint64_t l_headerbitpoint = l_bitvalueindex & 63ull;
        uint64_t l_bit_in_block = static_cast<uint64_t>(_lendnumber_) & 63ull;

        //範囲チェック
        if(l_bitvalueindex >= m_bitvalue->GetAllocCount())
        {
            //指定された番号割当無し。
            m_lock.unlock();
            return;
        };
        if (l_headerindex >= m_bitheader->GetAllocCount())
        {
            //ヘッダが足りない
            m_lock.unlock();
            return;
        };

        //該当ビットをクリア
        (*m_bitvalue)[l_bitvalueindex] &= (~(1ULL << l_bit_in_block));

        //クリア後、そのブロックが満杯でなくなったらheaderの対応ビットを0に戻す。
        if ((*m_bitvalue)[l_bitvalueindex] != 0xFFFFFFFFFFFFFFFFull)
        {
            //header 要素は存在チェック済みなので直接アクセスしてクリア
            (*m_bitheader)[l_headerindex] &= (~(1ULL << l_headerbitpoint));

        };

        m_lock.unlock();
    };

    //指定した番号が空き番号であればそれを使用します。
    bool SonikAssignFreeNumber::AssignNumber(int64_t _number_)
    {
        if (_number_ < 0 || 0x7FFFFFFFFFFFFFFF < _number_)
        {
            return false;
        };

        m_lock.lock();

        uint64_t l_bitvalueindex = static_cast<uint64_t>(_number_) >> 6;
        uint64_t l_headerindex = l_bitvalueindex >> 6;
        uint64_t l_bit_in_block = static_cast<uint64_t>(_number_) & 63ull;

        // Resizeで一括拡張（ReSize内部で0初期化されるため安全）
        if (l_bitvalueindex >= m_bitvalue->GetAllocCount())
        {
            if (!m_bitvalue->ReSize(l_bitvalueindex + 1))
            {
                m_lock.unlock();
                return false;
            };
        };

        if (l_headerindex >= m_bitheader->GetAllocCount())
        {
            if (!m_bitheader->ReSize(l_headerindex + 1))
            {
                m_lock.unlock();
                return false;
            };
        };

        uint64_t& r_target_bits = (*m_bitvalue)[l_bitvalueindex];
        if ((r_target_bits & (1ull << l_bit_in_block)) != 0)
        {
            m_lock.unlock();
            return false;
        };

        r_target_bits |= (1ull << l_bit_in_block);
        if (r_target_bits == 0xFFFFFFFFFFFFFFFF)
        {
            (*m_bitheader)[l_headerindex] |= (1ull << (l_bitvalueindex & 63ull));
        };

        m_lock.unlock();
        return true;
    };

    //指定した範囲の番号が空き番号であればその範囲を使用済みに設定します。
    //範囲内に既に使用済み番号が入っている場合は取得失敗となります。
    bool SonikAssignFreeNumber::AssignNumbers(int64_t _start_, int64_t _end_)
    {
        if (_start_ < 0 || _end_ < _start_ || 0x7FFFFFFFFFFFFFFF < _end_)
        {
            return false;
        };

        m_lock.lock();

        uint64_t l_start_idx = static_cast<uint64_t>(_start_) >> 6;
        uint64_t l_end_idx = static_cast<uint64_t>(_end_) >> 6;

        // 1. 拡張
        if (l_end_idx >= m_bitvalue->GetAllocCount())
        {
            if (!m_bitvalue->ReSize(l_end_idx + 1))
            {
                m_lock.unlock();
                return false;
            };
        };

        uint64_t l_end_h_idx = l_end_idx >> 6;
        if (l_end_h_idx >= m_bitheader->GetAllocCount())
        {
            if (!m_bitheader->ReSize(l_end_h_idx + 1))
            {
                m_lock.unlock();
                return false;
            };
        };

        // 2. 重複チェック（全範囲）
        for (int64_t i = _start_; i <= _end_; ++i)
        {
            if (((*m_bitvalue)[static_cast<uint64_t>(i) >> 6] & (1ull << (static_cast<uint64_t>(i) & 63ull))) != 0)
            {
                m_lock.unlock();
                return false;
            };
        };

        // 3. ビット適用とヘッダ更新の最適化
        uint64_t l_last_idx = 0xFFFFFFFFFFFFFFFFull;
        for (int64_t i = _start_; i <= _end_; ++i)
        {
            uint64_t l_idx = static_cast<uint64_t>(i) >> 6;
            (*m_bitvalue)[l_idx] |= (1ull << (static_cast<uint64_t>(i) & 63ull));

            // インデックスが変わるタイミング、または最後のループでヘッダをチェック
            if (l_idx != l_last_idx)
            {
                if ((*m_bitvalue)[l_idx] == 0xFFFFFFFFFFFFFFFFull)
                {
                    (*m_bitheader)[l_idx >> 6] |= (1ull << (l_idx & 63ull));
                };

                l_last_idx = l_idx;
            };
        };

        m_lock.unlock();
        return true;
    };

    //指定した範囲の使用済み番号を返却します。
    void SonikAssignFreeNumber::ReturnNumbers(int64_t _start_, int64_t _end_)
    {
        if (_start_ < 0 || _end_ < _start_)
        {
            return;
        };

        m_lock.lock();
        uint64_t l_alloc = m_bitvalue->GetAllocCount();

        for (int64_t i = _start_; i <= _end_; ++i)
        {
            uint64_t l_idx = static_cast<uint64_t>(i) >> 6;
            if (l_idx >= l_alloc)
            {
                continue;
            };

            (*m_bitvalue)[l_idx] &= ~(1ull << (static_cast<uint64_t>(i) & 63ull));
            // ヘッダは必ず降ろす（満杯ではなくなるため）
            if ((l_idx >> 6) < m_bitheader->GetAllocCount())
            {
                (*m_bitheader)[l_idx >> 6] &= ~(1ull << (l_idx & 63ull));
            };
        };

        m_lock.unlock();
    };


};