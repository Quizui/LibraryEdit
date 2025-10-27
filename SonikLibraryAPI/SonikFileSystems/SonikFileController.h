#pragma once

#ifndef SONIKFILECONTROLLER_H_
#define SONIKFILECONTROLLER_H_

#include "../SonikString/SonikString.h"
#include "../SonikCAS/SonikAtomicLock.h"
#include "../SmartPointer/SonikSmartPointer.hpp"

#include <stdint.h>

 //前方宣言============================
namespace SonikLib
{
    namespace Container
    {
        template<class QueueType>
        class SonikAtomicQueue;
    };

    namespace FileSystemControllers
    {
        class SonikFileSystemController;
    };
};

struct _iobuf;
typedef _iobuf FILE;

//====================================

namespace SonikLib
{
    using SFileSystemController = SonikLib::SharedSmtPtr<SonikLib::FileSystemControllers::SonikFileSystemController>;

    namespace FileSystem
    {
        enum FILEOPENSWITCH
        {
            FOSW_READ = 0x00000001,
            FOSW_WRITE = 0x00000002,
            FOSW_ADD = 0x00000004,
            FOSW_OPENTEXT = 0x00000008,
            FOSW_CREATE = 0x00000010,
            FOSW_CLEAR = 0x00000020,
            FOSW_READSHARE = 0x00000040,
            FOSW_WRITESHARE = 0x00000080,
        };// end enum FILE OPEN SWITCH

    }; //end namespace FileSystem

    namespace FileSystemControllers
    {

        class SonikFileSystemController
        {
        private:
            class InnerFileSystemFunction;
            InnerFileSystemFunction* mp_f_sys_func;

            SonikLib::S_CAS::SonikAtomicLock m_lock;

            SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

            using SFSC_VV_FUNC = void (InnerFileSystemFunction::*)(void);
            using SFSC_U64V_FUNC = uint64_t(InnerFileSystemFunction::*)(void);
            using SFSC_VU64_FUNC = void (InnerFileSystemFunction::*)(uint64_t);
            using SFSC_READ_FUNC = int32_t(InnerFileSystemFunction::*)(char*, uint64_t);
            using SFSC_WRITE_FUNC = void (InnerFileSystemFunction::*)(char*, uint64_t);
            using SFSC_WRITESTR_FUNC = void (InnerFileSystemFunction::*)(SonikLib::SonikString&);
            using SFSC_READLINE_FUNC = int32_t(InnerFileSystemFunction::*)(SonikLib::SonikString&, uint64_t);
            using SFSC_READLINEQUEUE_FUNC = int32_t(InnerFileSystemFunction::*)(SonikLib::Container::SonikAtomicQueue<SonikLib::SonikString>&, uint64_t);

            SFSC_VV_FUNC m_run_vv_func[3];
            SFSC_U64V_FUNC m_run_u64v_func[2];
            SFSC_VU64_FUNC m_run_vu64_func;
            SFSC_READ_FUNC m_run_read_func;
            SFSC_WRITE_FUNC m_run_write_func;
            SFSC_WRITESTR_FUNC m_run_writestr_func[3];
            SFSC_READLINE_FUNC m_run_readline_func;
            SFSC_READLINEQUEUE_FUNC m_run_rlqueue_func;

        private:
            //コピー禁止&ムーブ禁止
            SonikFileSystemController(const SonikFileSystemController& _copy_) = delete;
            SonikFileSystemController(SonikFileSystemController&& _move_) = delete;
            SonikFileSystemController& operator =(const SonikFileSystemController& _copy_) = delete;
            SonikFileSystemController& operator =(SonikFileSystemController& _move_) = delete;

            //コンストラクタ
            SonikFileSystemController(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
        public:
            //デストラクタ
            ~SonikFileSystemController(void);

            static bool CreateFileController(SFileSystemController& _out_);
            static bool CreateFileController(SFileSystemController& _out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

            //FileOpen
            bool OpenFile(SonikLib::SonikString _filepath_, uint32_t _open_switch_);
            //FileClose
            void CloseFile(void);

            //現在のファイルサイズの取得
            uint64_t GetFileSize(void);

            //シーク位置を最初に移動します。
            void SeekPointSet_Top(void);
            //シーク位置を最後に移動します。
            void SeekPointSet_End(void);
            //現在ンお読み込みシーク位置から指定バイト数分移動します。
            void SeekPointSet_Point(uint64_t _offset_);
            //現在のシーク位置を取得します。
            uint64_t SeekPointGet(void);

            //指定したサイズ文読み込みます。
            int32_t Read(char* _buffer_, uint64_t _size_);
            //バイナリデータ書き込み用
            void Write(char* _writevalue_, uint64_t _writesize_);

            //SonikStringの吐き出し方法で分けています。
            //テキストモードでオープンした状態だとファイル内の文字のエンコーディングがUTF-8に代わるといったことはありません。
            //バイナリで、追記以外...つまりすべての文字を再出力..となった場合は変換されるかもしれません。
            void Write_char(SonikLib::SonikString _writevalue_);
            void Write_UTF8(SonikLib::SonikString _writevalue_);
            void Write_UTF16(SonikLib::SonikString _writevalue_);

            //テキストモード専用　指定された行数文TEXTを読み込みます。
            int32_t ReadText_Line(SonikLib::SonikString& _str_, uint64_t GetRowCnt = 1);
            //テキストモード専用　指定された行数文TEXTを読み込ます。改行は削除され、改行で分割されたQueueとして取得します。
            int32_t ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>& _GetLineQueue_, uint64_t GetRowCnt = 1);
        };

    }; //end namespace FileSystemController

}; //end namespace SonikLib

#endif /* SONIKFILECONTROLLER_H_ */