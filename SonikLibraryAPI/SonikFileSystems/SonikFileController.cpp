
#include "SonikFileController.h"
#include "../Container/SonikAtomicQueue.hpp"
#include "../Container/RangedForContainer.hpp"
#include "../SonikString/SonikStringConvert.hpp"

#include <stdint.h>
#include <new>


#if defined(_WIN64)
 //windows include
#include <windows.h>
#include <fileapi.h>

#elif defined(__linux__)
 //linux included

#endif

namespace SonikLib
{
    namespace FileSystemControllers
    {
        class SonikFileSystemController::InnerFileSystemFunction
        {
        public:
#if defined(_WIN64)
            HANDLE m_file;

#elif defined(__linux__)
            void* m_file;

#endif
            SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;
            SonikLib::SonikString m_filepath;

        public:
            InnerFileSystemFunction(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
            ~InnerFileSystemFunction(void);

            //Open成功中に呼ばれる関数
            void OpenFunction_CloseFile(void);
            uint64_t OpenFunction_GetFileSize(void);
            void OpenFunction_SeekPointSet_Top(void);
            void OpenFunction_SeekPointSet_End(void);
            void OpenFunction_SeekPointSet_Point(uint64_t);
            uint64_t OpenFunction_SeekPointGet(void);
            int32_t OpenFunction_Read(char*, uint64_t);
            void OpenFunction_Write(char*, uint64_t);
            void OpenFunction_Write_char(SonikLib::SonikString&);
            void OpenFunction_Write_UTF8(SonikLib::SonikString&);
            void OpenFunction_Write_UTF16(SonikLib::SonikString&);
            int32_t OpenFunction_ReadText_Line(SonikLib::SonikString&, uint64_t);
            int32_t OpenFunction_ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>&, uint64_t);

            //Close中に呼ばれる関数
            void NoOpenFunction_CloseFile(void);
            uint64_t NoOpenFunction_GetFileSize(void);
            void NoOpenFunction_SeekPointSet_Top(void);
            void NoOpenFunction_SeekPointSet_End(void);
            void NoOpenFunction_SeekPointSet_Point(uint64_t);
            uint64_t NoOpenFunction_SeekPointGet(void);
            int32_t NoOpenFunction_Read(char*, uint64_t);
            void NoOpenFunction_Write(char*, uint64_t);
            void NoOpenFunction_Write_char(SonikLib::SonikString&);
            void NoOpenFunction_Write_UTF8(SonikLib::SonikString&);
            void NoOpenFunction_Write_UTF16(SonikLib::SonikString&);
            int32_t NoOpenFunction_ReadText_Line(SonikLib::SonikString&, uint64_t);
            int32_t NoOpenFunction_ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>&, uint64_t);

        };

        SonikFileSystemController::InnerFileSystemFunction::InnerFileSystemFunction(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
        :m_file(nullptr)
        ,m_allocator(_allocator_)
        ,m_filepath(_allocator_)
        {

        };
        SonikFileSystemController::InnerFileSystemFunction::~InnerFileSystemFunction(void)
        {

        };

        //Open成功中に呼ばれる関数
        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_CloseFile(void)
        {
#if defined(_WIN64)
            CloseHandle(m_file);
            m_file = nullptr;

#elif defined(__linux__)
            //linux definition
#endif

        };

        uint64_t SonikFileSystemController::InnerFileSystemFunction::OpenFunction_GetFileSize(void)
        {
#if defined(_WIN64)
            LARGE_INTEGER ret{};
            ::GetFileSizeEx(m_file, &ret);

            return static_cast<uint64_t>(ret.QuadPart);

#elif defined(__linux__)
            //linux definition
#endif

        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_SeekPointSet_Top(void)
        {
#if defined(_WIN64)
            LARGE_INTEGER l_seek{};
            SetFilePointerEx(m_file, l_seek, nullptr, FILE_BEGIN);

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_SeekPointSet_End(void)
        {
#if defined(_WIN64)
            LARGE_INTEGER l_seek{};
            SetFilePointerEx(m_file, l_seek, nullptr, FILE_END);

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_SeekPointSet_Point(uint64_t _offset_)
        {
#if defined(_WIN64)
            LARGE_INTEGER l_seek{};
            l_seek.QuadPart = _offset_;

            SetFilePointerEx(m_file, l_seek, nullptr, FILE_CURRENT);

#elif defined(__linux__)
            //linux definition
#endif
        };

        uint64_t SonikFileSystemController::InnerFileSystemFunction::OpenFunction_SeekPointGet(void)
        {
#if defined(_WIN64)
            LARGE_INTEGER l_seek{};
            SetFilePointerEx(m_file, l_seek, &l_seek, FILE_CURRENT);

            return static_cast<uint64_t>(l_seek.QuadPart);
#elif defined(__linux__)
            //linux definition
#endif
        };

        int32_t SonikFileSystemController::InnerFileSystemFunction::OpenFunction_Read(char* _buffer_, uint64_t _size_)
        {
#if defined(_WIN64)
            uint64_t l_split_cnt = _size_ / UINT32_MAX;
            DWORD l_err = 0;

            for (uint64_t i = 0; i < l_split_cnt; ++i)
            {
                SetLastError(0);
                ReadFile(m_file, _buffer_, static_cast<unsigned long>(_size_), nullptr, nullptr);

                l_err = GetLastError();

                if (l_err == ERROR_HANDLE_EOF)
                {
                    return 0xE0F; //EOF到達
                };

                _buffer_ += UINT32_MAX;
                _size_ -= UINT32_MAX;
            };

            ReadFile(m_file, _buffer_, static_cast<unsigned long>(_size_), nullptr, nullptr);
            return 0;

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_Write(char* _writevalue_, uint64_t _writesize_)
        {
#if defined(_WIN64)
            uint64_t l_split_cnt = _writesize_ / UINT32_MAX;

            for (uint64_t i = 0; i < l_split_cnt; ++i)
            {
                WriteFile(m_file, _writevalue_, static_cast<unsigned long>(_writesize_), nullptr, nullptr);
                _writevalue_ += UINT32_MAX;
                _writesize_ -= UINT32_MAX;
            };

            WriteFile(m_file, _writevalue_, static_cast<unsigned long>(_writesize_), nullptr, nullptr);

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_Write_char(SonikLib::SonikString& _writevalue_)
        {
            uint64_t l_size = 0;
            l_size = _writevalue_.GetCpy_str_c(nullptr);

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                return;
            };

            //長さ0以外処理続行
#if defined(_WIN64)
            char* l_buffer = new(std::nothrow) char[l_size] {};
            if (l_buffer == nullptr)
            {
                return;
            };

            _writevalue_.GetCpy_str_c(l_buffer);

            uint64_t l_split_cnt = l_size / UINT32_MAX;
            char* l_buf_control = l_buffer;

            for (uint64_t i = 0; i < l_split_cnt; ++i)
            {
                WriteFile(m_file, l_buf_control, UINT32_MAX, nullptr, nullptr);
                l_buf_control += UINT32_MAX;
                l_size -= UINT32_MAX;
            };

            WriteFile(m_file, l_buf_control, static_cast<unsigned long>(l_size), nullptr, nullptr);
            delete[] l_buffer;

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_Write_UTF8(SonikLib::SonikString& _writevalue_)
        {
#if defined(_WIN64)
            uint64_t l_size = 0;
            l_size = _writevalue_.GetCpy_str_utf8(nullptr);

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                return;
            };

            //長さ0以外はこっち
            void* l_allocbuffer = m_allocator->memal(sizeof(char) * l_size);
            if (l_allocbuffer == nullptr)
            {
                return;
            };

            char* l_buffer = new(l_allocbuffer) char[l_size] {};
            _writevalue_.GetCpy_str_utf8(reinterpret_cast<utf8_t*>(l_buffer));

            uint64_t l_split_cnt = l_size / UINT32_MAX;
            char* l_buf_control = l_buffer;

            for (uint64_t i = 0; i < l_split_cnt; ++i)
            {
                WriteFile(m_file, l_buf_control, UINT32_MAX, nullptr, nullptr);
                l_buf_control += UINT32_MAX;
                l_size -= UINT32_MAX;
            };

            WriteFile(m_file, l_buf_control, static_cast<unsigned long>(l_size), nullptr, nullptr);
            
            m_allocator->memdel(l_buffer);

#elif defined(__linux__)
            //linux definition
#endif
        };

        void SonikFileSystemController::InnerFileSystemFunction::OpenFunction_Write_UTF16(SonikLib::SonikString& _writevalue_)
        {
            uint64_t l_size = 0;
            l_size = _writevalue_.GetCpy_str_utf16(nullptr);

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                return;
            };

            //長さ0以外処理続行
#if defined(_WIN64)

            void* l_allocbuffer = m_allocator->memal(sizeof(char) * l_size);
            if (l_allocbuffer == nullptr)
            {
                return;
            };

            char* l_buffer = new(l_allocbuffer) char[l_size] {};
            _writevalue_.GetCpy_str_utf16(reinterpret_cast<char16_t*>(l_buffer));

            uint64_t l_split_cnt = l_size / UINT32_MAX;
            char* l_buf_control = l_buffer;

            for (uint64_t i = 0; i < l_split_cnt; ++i)
            {
                WriteFile(m_file, l_buf_control, UINT32_MAX, nullptr, nullptr);
                l_buf_control += UINT32_MAX;
                l_size -= UINT32_MAX;
            };

            WriteFile(m_file, l_buf_control, static_cast<unsigned long>(l_size), nullptr, nullptr);
            
            m_allocator->memdel(l_buffer);

#elif defined(__linux__)
            //linux definition
#endif
        };

        inline int32_t SonikFileSystemController::InnerFileSystemFunction::OpenFunction_ReadText_Line(SonikLib::SonikString& _str_, uint64_t GetRowCnt)
        {
#if defined(_WIN64)
            SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> l_container;
            SonikLib::Container::SonikVariableArrayContainer<char>::CreateContainer(l_container, m_allocator, 10240);

            ReadFile(m_file, &(*l_container)[0], 10240, nullptr, nullptr);

            if ((*l_container)[0] == '\0')
            {
                //文字無し
                _str_ = "";
                return 0;
            };

            //ラムダ式保有用
            using L_FIND_CHAR_LAMDA = uintptr_t(*)(void*);
            L_FIND_CHAR_LAMDA l_lamda;

            //１回目の読み込みで文字列の型をチェック
            SonikLibConvertType l_convtype = SonikLibConvertType::SCHTYPE_UNKNOWN;
            l_convtype = SonikLibStringConvert::CheckConvertType(&(*l_container)[0]);

            //文字列でwcsstr か strstrか変わるので、ラムダ式で切り替える
            switch (l_convtype)
            {
            case SonikLibConvertType::SCHTYPE_SJIS:
            case SonikLibConvertType::SCHTYPE_UTF8:
            case SonikLibConvertType::SCHTYPE_BOMUTF8:
                //最小が1Byte単位の文字列
                l_lamda = [](void* _readdata_) -> uintptr_t
                {
                    uintptr_t ret = reinterpret_cast<uintptr_t>(strstr(reinterpret_cast<char*>(_readdata_), "\n"));
                    if (ret != 0)
                    {
                        //一旦置き換え
                        (*reinterpret_cast<char*>(ret)) = '\0';
                    };
                    return ret;
                };
                break;

            case SonikLibConvertType::SCHTYPE_UTF16:
            case SonikLibConvertType::SCHTYPE_UTF32:
                //2Byte～4Byte単位の文字列
                l_lamda = [](void* _readdata_) -> uintptr_t
                {
                    uintptr_t ret = reinterpret_cast<uintptr_t>(wcsstr(reinterpret_cast<wchar_t*>(_readdata_), L"\n"));
                    if (ret != 0)
                    {
                        //一旦置き換え
                        (*reinterpret_cast<wchar_t*>(ret)) = L'\0';
                    };
                    return ret;

                };
                break;

            default:
                //処理不可
                return -1;
            };

            //０なら取れてないのでループ処理へ。
            uint64_t l_container_offset = 10240;
            uintptr_t l_lamda_ret = l_lamda(&(*l_container)[0]);

            while (l_lamda_ret == 0)
            {
                //領域の拡大
                l_container->Reserve(l_container->GetSizeMax() + 10240);

                //読み込み
                SetLastError(0);
                ReadFile(m_file, &(*l_container)[l_container_offset], 10240, nullptr, nullptr);
                DWORD l_err = GetLastError();

                if (l_err == ERROR_HANDLE_EOF)
                {
                    //EOFに到達したら、１行の文字列として返却して終了
                    _str_ = (*l_container)[0];
                    return 0xE0F;
                };

                //オフセット分移動した先で関数実行し、結果を取得
                l_lamda_ret = l_lamda(&(*l_container)[l_container_offset]);
                l_container_offset += 10240;
            };

            //０以外が取れたら１行として取得
            //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
            LARGE_INTEGER l_fpoffset{};
            l_fpoffset.QuadPart = ((l_container->GetSizeMax() - (l_lamda_ret - reinterpret_cast<uintptr_t>(&(*l_container)[0]))) * (-1)) + 1;

            //ファイルポインタを戻す
            SetFilePointerEx(m_file, l_fpoffset, nullptr, FILE_CURRENT);

            //引っかかれば終端文字に変更されているはずなので、そのまま代入して改行を付け加える
            _str_ += (&(*l_container)[0]);
            _str_ += "\n";

            //クリアして次へ
            l_container->Clear();

            for (uint64_t l_rowcnt = 1; l_rowcnt < GetRowCnt; ++l_rowcnt)
            {
                ReadFile(m_file, &(*l_container)[0], 10240, nullptr, nullptr);

                l_container_offset = 10240;
                l_lamda_ret = l_lamda(&(*l_container)[0]);

                while (l_lamda_ret == 0)
                {
                    //領域の拡大
                    l_container->Reserve(l_container->GetSizeMax() + 10240);

                    //読み込み
                    SetLastError(0);
                    ReadFile(m_file, &(*l_container)[l_container_offset], 10240, nullptr, nullptr);
                    DWORD l_err = GetLastError();

                    if (l_err == ERROR_HANDLE_EOF)
                    {
                        //EOFに到達したら、１行の文字列として返却して終了
                        _str_ = (*l_container)[0];
                        return 0xE0F;
                    };

                    //オフセット分移動した先で関数実行し、結果を取得
                    l_lamda_ret = l_lamda(&(*l_container)[l_container_offset]);
                    l_container_offset += 10240;
                };

                //０以外が取れたら１行として取得
                //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
                LARGE_INTEGER l_fpoffset{};
                l_fpoffset.QuadPart = ((l_container->GetSizeMax() - (l_lamda_ret - reinterpret_cast<uintptr_t>(&(*l_container)[0]))) * (-1)) + 1;

                //ファイルポインタを戻す
                SetFilePointerEx(m_file, l_fpoffset, nullptr, FILE_CURRENT);

                //引っかかれば終端文字に変更されているはずなので、そのまま代入して改行を付け加える
                _str_ += (&(*l_container)[0]);
                _str_ += "\n";

                //クリアして次へ
                l_container->Clear();
            };

#elif defined(__linux__)
            //linux definition
#endif

    //rowcnt分取れたら終わり
            return 0;
        };
        int32_t SonikFileSystemController::InnerFileSystemFunction::OpenFunction_ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>& _GetLineQueue_, uint64_t GetRowCnt)
        {
            SonikLib::SonikString l_getstr(m_allocator);
            int32_t ret = 0;

            for (uint64_t i = 0; i < GetRowCnt; ++i)
            {
                ret = this->OpenFunction_ReadText_Line(l_getstr, 1);

                if (ret == 0xE0F)
                {
                    //EOF到達のため終了
                    return ret;
                };

                _GetLineQueue_.EnQueue(l_getstr);
                l_getstr = "";
            };

            return ret;
        };

        //Close中に呼ばれる関数
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_CloseFile(void)
        {
            return;
        };
        uint64_t SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_GetFileSize(void)
        {
            return static_cast<uint64_t>(-1);
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Top(void)
        {
            return;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_SeekPointSet_End(void)
        {
            return;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Point(uint64_t)
        {
            return;
        };
        uint64_t SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_SeekPointGet(void)
        {
            return static_cast<uint64_t>(-1);
        };
        int32_t SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_Read(char*, uint64_t)
        {
            return -1;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_Write(char*, uint64_t)
        {
            return;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_Write_char(SonikLib::SonikString&)
        {
            return;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_Write_UTF8(SonikLib::SonikString&)
        {
            return;
        };
        void SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_Write_UTF16(SonikLib::SonikString&)
        {
            return;
        };
        int32_t SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_ReadText_Line(SonikLib::SonikString&, uint64_t)
        {
            return -1;
        };
        int32_t SonikFileSystemController::InnerFileSystemFunction::NoOpenFunction_ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>&, uint64_t)
        {
            return -1;
        };

        //______________________________________________________________________________________________________________________________
        // 
        // 親クラス実装
        //______________________________________________________________________________________________________________________________
        //コンストラクタ
        SonikFileSystemController::SonikFileSystemController(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
            :mp_f_sys_func(nullptr)
            ,m_allocator(_allocator_)
            , m_run_vv_func{ &InnerFileSystemFunction::NoOpenFunction_CloseFile,
                           &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Top,
                           &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_End }
            , m_run_u64v_func{ &InnerFileSystemFunction::NoOpenFunction_GetFileSize,
                             &InnerFileSystemFunction::NoOpenFunction_SeekPointGet }
            , m_run_vu64_func(&InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Point)
            , m_run_read_func(&InnerFileSystemFunction::NoOpenFunction_Read)
            , m_run_write_func(&InnerFileSystemFunction::NoOpenFunction_Write)
            , m_run_writestr_func{ &InnerFileSystemFunction::NoOpenFunction_Write_char,
                                 &InnerFileSystemFunction::NoOpenFunction_Write_UTF8,
                                 &InnerFileSystemFunction::NoOpenFunction_Write_UTF16 }
            , m_run_readline_func(&InnerFileSystemFunction::NoOpenFunction_ReadText_Line)
            , m_run_rlqueue_func(&InnerFileSystemFunction::NoOpenFunction_ReadText_LineQueue)
        {

        };

        //デストラクタ
        SonikFileSystemController::~SonikFileSystemController(void)
        {
            if (mp_f_sys_func != nullptr)
            {
                CloseFile();
                mp_f_sys_func->~InnerFileSystemFunction();
                m_allocator->memdel(mp_f_sys_func);
                mp_f_sys_func = nullptr;
            };
        };

        bool SonikFileSystemController::CreateFileController(SFileSystemController& _out_)
        {
            SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
            SonikLib::SLibAllocateInterface* l_defalloc = new(std::nothrow) SonikLib::SLibAllocateInterface;
            if(l_defalloc == nullptr)
            {
                return false;
            };
            if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, l_allocSmtPtr))
            {
                delete l_defalloc;
                return false;
            };

            void* l_allocbuffer = l_defalloc->memal(sizeof(SonikFileSystemController));
            if (l_allocbuffer == nullptr)
            {
                return false;
            };

            SonikFileSystemController* ptmp;
            ptmp = new(l_allocbuffer) SonikFileSystemController(l_allocSmtPtr);

            l_allocbuffer = l_defalloc->memal(sizeof(SonikFileSystemController::InnerFileSystemFunction));
            if (l_allocbuffer == nullptr)
            {
                ptmp->~SonikFileSystemController();
                l_defalloc->memdel(ptmp);
                return false;
            };


            SonikFileSystemController::InnerFileSystemFunction* ptmp_inner;
            ptmp_inner = new(l_allocbuffer) SonikFileSystemController::InnerFileSystemFunction(l_allocSmtPtr);
            if (ptmp_inner == nullptr)
            {
                ptmp->~SonikFileSystemController();
                l_defalloc->memdel(l_allocbuffer);
                l_defalloc->memdel(ptmp);
                return false;
            };

            ptmp->mp_f_sys_func = ptmp_inner;

            if (!SFileSystemController::SmartPointerCreate(ptmp, _out_))
            {
                ptmp_inner->~InnerFileSystemFunction();
                ptmp->~SonikFileSystemController();
                l_defalloc->memdel(ptmp_inner);
                l_defalloc->memdel(ptmp);
                return false;
            };

            return true;
        };
        bool SonikFileSystemController::CreateFileController(SFileSystemController& _out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
        {
            void* l_allocbuffer = _allocator_->memal(sizeof(SonikFileSystemController));
            if (l_allocbuffer == nullptr)
            {
                return false;
            };

            SonikFileSystemController* ptmp;
            ptmp = new(l_allocbuffer) SonikFileSystemController(_allocator_);

            l_allocbuffer = _allocator_->memal(sizeof(SonikFileSystemController::InnerFileSystemFunction));
            if (l_allocbuffer == nullptr)
            {
                ptmp->~SonikFileSystemController();
                _allocator_->memdel(ptmp);
                return false;
            };


            SonikFileSystemController::InnerFileSystemFunction* ptmp_inner;
            ptmp_inner = new(l_allocbuffer) SonikFileSystemController::InnerFileSystemFunction(_allocator_);
            if (ptmp_inner == nullptr)
            {
                ptmp->~SonikFileSystemController();
                _allocator_->memdel(l_allocbuffer);
                _allocator_->memdel(ptmp);
                return false;
            };

            ptmp->mp_f_sys_func = ptmp_inner;

            if (!SFileSystemController::SmartPointerCreate(ptmp, _out_, _allocator_))
            {
                ptmp_inner->~InnerFileSystemFunction();
                ptmp->~SonikFileSystemController();
                _allocator_->memdel(ptmp_inner);
                _allocator_->memdel(ptmp);
                return false;
            };

            return true;
        };


        //FileOpen
        bool SonikFileSystemController::OpenFile(SonikLib::SonikString _filepath_, uint32_t _open_switch_)
        {
            m_lock.lock();

            //エラーとする組み合わせをチェック処理
            if (_open_switch_ == 0)
            {
                //0指定は問答無用で失敗
                m_lock.unlock();
                return false;
            };

            //READフラグが立っていて、WRITEフラグが立っていない場合
            if (((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) != 0)
                && ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == 0))
            {
                //CLEAR と CREATEフラグが立っていれば、エラー(WRITEフラグが必要)
                if ((_open_switch_ & (FileSystem::FILEOPENSWITCH::FOSW_CREATE | FileSystem::FILEOPENSWITCH::FOSW_CLEAR)) != 0)
                {
                    //失敗
                    m_lock.unlock();
                    return false;
                };
            };

            //共有モードは、使用する場合はオープンモードと同じでなけれならないため同じでないなら失敗
            //共有モード：読み込みが立っていた場合、READフラグがっていなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE) != 0
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) == 0)
            {
                //失敗
                m_lock.unlock();
                return false;
            };
            //続けて、共有モード：書き込みが立っていた場合、WRITEフラグがなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE) != 0
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == 0)
            {
                //失敗
                m_lock.unlock();
                return false;
            };

            //一旦クローズ処理
            (mp_f_sys_func->*m_run_vv_func[0])();
            //クローズ関数へセット
            m_run_vv_func[0] = &InnerFileSystemFunction::NoOpenFunction_CloseFile;
            m_run_vv_func[1] = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Top;
            m_run_vv_func[2] = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_End;

            m_run_u64v_func[0] = &InnerFileSystemFunction::NoOpenFunction_GetFileSize;
            m_run_u64v_func[1] = &InnerFileSystemFunction::NoOpenFunction_SeekPointGet;

            m_run_vu64_func = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Point;
            m_run_read_func = &InnerFileSystemFunction::NoOpenFunction_Read;
            m_run_write_func = &InnerFileSystemFunction::NoOpenFunction_Write;
            m_run_writestr_func[0] = &InnerFileSystemFunction::NoOpenFunction_Write_char;
            m_run_writestr_func[1] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF8;
            m_run_writestr_func[2] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF16;

            m_run_readline_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_Line;
            m_run_rlqueue_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_LineQueue;

            //ここからプラットフォーム別処理===========================
#if defined(_WIN64)
            //ファイルハンドル(最後にメンバ変数に代入)
            HANDLE l_filehandle = nullptr;

            //オープンモードの設定
            DWORD l_openmode = 0;
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ)
            {
                l_openmode |= GENERIC_READ;
            };
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE)
            {
                l_openmode |= GENERIC_WRITE;
            };

            //共有モードの設定
            DWORD l_sharemode = 0;
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE)
            {
                l_sharemode |= FILE_SHARE_READ;
            };
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE)
            {
                l_sharemode |= FILE_SHARE_WRITE;
            };

            //オープンモードの設定
            DWORD l_opencontrol = OPEN_EXISTING;
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CREATE)
            {
                l_opencontrol = OPEN_ALWAYS;
            };
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CLEAR)
            {
                l_opencontrol = CREATE_ALWAYS;
            };

            l_filehandle = CreateFileW(_filepath_.str_wchar(), //ファイル名、パス
                l_openmode,             //オープンモード(読み込み許可、書き込み許可等)
                l_sharemode,            //共有モード
                nullptr,                //セキュリティ属性（nullptr = デフォルト)
                l_opencontrol,          //オープン時操作(OEPN_EXISITNGをデフォルトとしている(ファイルがなければ失敗とみなす))
                FILE_ATTRIBUTE_NORMAL,  //通常のファイル属性(ノーマル)
                nullptr                 //テンプレートファイル(なし)
            );

            if (l_filehandle == INVALID_HANDLE_VALUE)
            {
                //失敗
                m_lock.unlock();
                return false;
            };

            mp_f_sys_func->m_file = l_filehandle;

#elif defined(__linux__)

#endif
            //オープン状態、クローズ状態関数のセット
            m_run_vv_func[0] = &InnerFileSystemFunction::OpenFunction_CloseFile;
            m_run_vv_func[1] = &InnerFileSystemFunction::OpenFunction_SeekPointSet_Top;
            m_run_vv_func[2] = &InnerFileSystemFunction::OpenFunction_SeekPointSet_End;

            m_run_u64v_func[0] = &InnerFileSystemFunction::OpenFunction_GetFileSize;
            m_run_u64v_func[1] = &InnerFileSystemFunction::OpenFunction_SeekPointGet;

            m_run_vu64_func = &InnerFileSystemFunction::OpenFunction_SeekPointSet_Point;
            m_run_read_func = &InnerFileSystemFunction::OpenFunction_Read;
            m_run_write_func = &InnerFileSystemFunction::OpenFunction_Write;
            m_run_writestr_func[0] = &InnerFileSystemFunction::OpenFunction_Write_char;
            m_run_writestr_func[1] = &InnerFileSystemFunction::OpenFunction_Write_UTF8;
            m_run_writestr_func[2] = &InnerFileSystemFunction::OpenFunction_Write_UTF16;

            m_run_readline_func = &InnerFileSystemFunction::OpenFunction_ReadText_Line;
            m_run_rlqueue_func = &InnerFileSystemFunction::OpenFunction_ReadText_LineQueue;

            //Writeフラグが立ってなければWrite系は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == 0)
            {
                m_run_write_func = &InnerFileSystemFunction::NoOpenFunction_Write;

                m_run_writestr_func[0] = &InnerFileSystemFunction::NoOpenFunction_Write_char;
                m_run_writestr_func[1] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF8;
                m_run_writestr_func[2] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF16;
            };

            //テキストモードなら文字列系は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_OPENTEXT) == 0)
            {
                m_run_writestr_func[0] = &InnerFileSystemFunction::NoOpenFunction_Write_char;
                m_run_writestr_func[1] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF8;
                m_run_writestr_func[2] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF16;

                m_run_readline_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_Line;
                m_run_rlqueue_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_LineQueue;
            };

            //ADDフラグが立っていればファイルシークを最終へ
            if (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_ADD)
            {
                (mp_f_sys_func->*m_run_vv_func[2])();
            };

            //終了
            m_lock.unlock();
            return true;
        };

        //FileClose
        void SonikFileSystemController::CloseFile(void)
        {
            m_lock.lock();

            //クローズ処理
            (mp_f_sys_func->*m_run_vv_func[0])();
            //クローズ関数へセット
            m_run_vv_func[0] = &InnerFileSystemFunction::NoOpenFunction_CloseFile;
            m_run_vv_func[1] = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Top;
            m_run_vv_func[2] = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_End;

            m_run_u64v_func[0] = &InnerFileSystemFunction::NoOpenFunction_GetFileSize;
            m_run_u64v_func[1] = &InnerFileSystemFunction::NoOpenFunction_SeekPointGet;

            m_run_vu64_func = &InnerFileSystemFunction::NoOpenFunction_SeekPointSet_Point;
            m_run_read_func = &InnerFileSystemFunction::NoOpenFunction_Read;
            m_run_write_func = &InnerFileSystemFunction::NoOpenFunction_Write;
            m_run_writestr_func[0] = &InnerFileSystemFunction::NoOpenFunction_Write_char;
            m_run_writestr_func[1] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF8;
            m_run_writestr_func[2] = &InnerFileSystemFunction::NoOpenFunction_Write_UTF16;

            m_run_readline_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_Line;
            m_run_rlqueue_func = &InnerFileSystemFunction::NoOpenFunction_ReadText_LineQueue;

            m_lock.unlock();
        };

        //現在のファイルサイズの取得
        uint64_t SonikFileSystemController::GetFileSize(void)
        {
            m_lock.lock();

            uint64_t ret = (mp_f_sys_func->*m_run_u64v_func[0])();

            m_lock.unlock();

            return ret;
        };

        //シーク位置を最初に移動します。
        void SonikFileSystemController::SeekPointSet_Top(void)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_vv_func[1])();
            m_lock.unlock();
        };

        //シーク位置を最後に移動します。
        void SonikFileSystemController::SeekPointSet_End(void)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_vv_func[2])();
            m_lock.unlock();

        };

        //現在の読み込みシーク位置から指定バイト数分移動します。
        void SonikFileSystemController::SeekPointSet_Point(uint64_t _offset_)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_vu64_func)(_offset_);
            m_lock.unlock();

        };

        //現在のシーク位置を取得します。
        uint64_t SonikFileSystemController::SeekPointGet(void)
        {
            m_lock.lock();
            uint64_t ret = (mp_f_sys_func->*m_run_u64v_func[1])();
            m_lock.unlock();

            return ret;
        };

        //指定したサイズ文読み込みます。
        int32_t SonikFileSystemController::Read(char* _buffer_, uint64_t _size_)
        {
            int32_t ret = 0;
            m_lock.lock();
            ret = (mp_f_sys_func->*m_run_read_func)(_buffer_, _size_);
            m_lock.unlock();

            return ret;
        };

        //バイナリデータ書き込み用 4バイト配列をreinterpretで渡している場合はblocksizeはsizeof(uint32_t)等...。
        void SonikFileSystemController::Write(char* _writevalue_, uint64_t _writesize_)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_write_func)(_writevalue_, _writesize_);
            m_lock.unlock();
        };

        //SonikStringの吐き出し方法で分けています。
        //テキストモードでオープンした状態だとファイル内の文字のエンコーディングがUTF-8に代わるといったことはありません。
        //バイナリで、追記以外...つまりすべての文字を再出力..となった場合は変換されるかもしれません。
        void SonikFileSystemController::Write_char(SonikLib::SonikString _writevalue_)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_writestr_func[0])(_writevalue_);
            m_lock.unlock();

        };

        void SonikFileSystemController::Write_UTF8(SonikLib::SonikString _writevalue_)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_writestr_func[1])(_writevalue_);
            m_lock.unlock();

        };

        void SonikFileSystemController::Write_UTF16(SonikLib::SonikString _writevalue_)
        {
            m_lock.lock();
            (mp_f_sys_func->*m_run_writestr_func[2])(_writevalue_);
            m_lock.unlock();

        };

        //テキストモード専用　指定された行数文TEXTを読み込みます。
        int32_t SonikFileSystemController::ReadText_Line(SonikLib::SonikString& _str_, uint64_t GetRowCnt)
        {
            int32_t ret = 0;
            m_lock.lock();
            ret = (mp_f_sys_func->*m_run_readline_func)(_str_, GetRowCnt);
            m_lock.unlock();

            return ret;
        };

        //テキストモード専用　指定された行数文TEXTを読み込ます。改行は削除され、改行で分割されたQueueとして取得します。
        int32_t SonikFileSystemController::ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>& _GetLineQueue_, uint64_t GetRowCnt)
        {
            int32_t ret = 0;
            m_lock.lock();
            ret = (mp_f_sys_func->*m_run_rlqueue_func)(_GetLineQueue_, GetRowCnt);
            m_lock.unlock();

            return ret;
        };

    }; // end namespace FileSystemControllers
}; // end namespace SonikLib