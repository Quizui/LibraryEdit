#pragma once

#ifndef SONIKFILECONTROLLER_H_
#define SONIKFILECONTROLLER_H_

#include "../SonikString/SonikString.h"
#include "../SonikCAS/SonikAtomicLock.h"
#include "../SmartPointer/SonikSmartPointer.hpp"
#include "../EnumsBitOperatorTemplate.hpp"
#include "../CPPGrammarDefines.h"

#include <cstdint>

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
    namespace FileSystem
    {
        enum class FILEOPENSWITCH : uint32_t
        {
            FOSW_UNKNOWN = 0x00000000, //デフォ値として使用
            FOSW_READ = 0x00000001,
            FOSW_WRITE = 0x00000002,
            FOSW_ADD = 0x00000004,
            FOSW_OPENTEXT = 0x00000008,
            FOSW_CREATE = 0x00000010,
            FOSW_CLEAR = 0x00000020,
            FOSW_READSHARE = 0x00000040,
            FOSW_WRITESHARE = 0x00000080,
        };// end enum FILE OPEN SWITCH

        enum class FILEERROR : uint32_t
        {
            FERR_NOT_ERROR_PROCESS_SUCCEEDED = 0, //正常に終了しました。
            FERR_FILE_NOT_FOUND,                  //指定したパス先に指定のファイル名が存在しません。アクセス権限以前にファイルそのものが存在しません。
            FERR_PATH_NOT_FOUND,                  //パスの形式やパス先に到達できず、パス解決できませんでした。
            FERR_ACCESS_DENIED,                   //該当の操作を行う権限がなく、アクセスが拒否されました。どのアクセス権限が原因かまでは不明です。
            FERR_FILEDISCRIPT_FOUND,              //WindowsならHandle, PosixならDiscripta が無効です。
            FERR_SHARING_BLOCKED,                 //他プロセスが指定したファイルを共有不可の状態(排他モード等)で開いていて要求したアクセス(開く/読み書き)ができませんでした。
            FERR_GEN_FAILURE,                     //ハードウェア故障、デバイスアクセス失敗、ドライバ異常等の低レベルにおける致命的なI/Oエラーです。
            FERR_IO_DEVICE,                       //汎用的な入出力、ファイルシステム異常等の低レベルにおける致命的なIPエラーです。
            FERR_SEEK_NOTSUPPORTED,               //シーク操作自体が対象デバイスでサポートされていない。
            FERR_SEEK_NEGATIVEVALUE,              //結果としてシーク位置が負の位置になってしまう等の無効なシーク要求
            FERR_SEEK_OUT_OF_RANGE,               //シーク移動先が範囲外です。
            FERR_READBYTE_NEGATIVEVALUE,          //読み込み時に指定された読み込みサイズが負の値になっています。
            FERR_READBUFFER_NULL,                 //読み込んだデータを保持するバッファがnullptrです。
            FERR_LARGEREADSIZE,                   //大容量のサイズが読み込みサイズとして指定されました。(１回で8GB以上の書き込みを行おうとすると発生します。)
            FERR_READ_EOF,                        //読み込みが最終端です。
            FERR_WRITEBYTE_NEGATIVEVALUE,         //書き込み時に指定された書き込みサイズが負の値になっています。
            FERR_WRITEBUFFER_NULL,                //書き込み時に使用するバッファがnullptrです。
            FERR_LARGEWRITESIZE,                  //大容量のサイズが書き込みサイズとして指定されました。(4TBを超えると発生します。)
            FERR_DISK_FULL,                       //書き込み先の容量不足が発生しました。
            FERR_QUOTA_EXCESS,                    //書き込み中にクォータ超過が発生しました。
            FERR_SECTION_WRITE,                   //部分書き込みが発生しました。
            FERR_OUTOFMEMORY,                     //リソース不足が発生しました。
            FERR_WRITEFILED,                      //なんらかの理由で書き込みが失敗しました。(広義に使用します)
            FERR_NOT_OPENMODE_TEXT,               //テキストモードで開かれていないため処理を実行できませんでした。
            FERR_NOT_READACCESS_DENIED,           //オープン時にRead権限が付与されていないため実行できませんでした。
            FERR_NOT_WRITEACCESS_DENIED,          //オープン時にWrite権限が付与されていないため実行できませんでした。
            FERR_NOT_FILEOPENSTATE,               //CloseのあとやOpen前、Open失敗後等の、ファイルが非Open状態の時に関数がコールされたため処理を実行しませんでした。
            FERR_OPENSTATE_UKNOWN,                //【オープンモード組み合わせエラー】オープン時、オープンモードでUNKNOWNが指定されたためオープン処理を実行しませんでした。
            FERR_OPENSTATE_READ_MISSMATCH_CLEAR_CREATE, //【オープンモード組み合わせエラー】オープン時、オープンモードがRead有/Write無 の状態で CLEARまたは、CREATEが指定されていたためオープン処理を実行しませんでした。
            FERR_OPENSTATE_SHEARINGMODE_MISSMATCH, //【オープンモード組み合わせエラー】オープン時、オープンモードで共有モード有の状態でRead/Writeの状態が不一致だったためオープン処理を実行しませんでした。

            //特殊値
            FERR_EXCEPTION, //広義の例外が発生しました。(原因が複数あったりAPI側で同じようなエラーが来た時の特殊値)
        };

    }; //end namespace FileSystem

}; //end namespace SonikLib

//Enum class のビット演算を有効化するために一旦切る。
SLIB_ENABLE_ENUMCLASS_BITMASK(::SonikLib::FileSystem::FILEOPENSWITCH) //該当のenum classのビット演算有効化


namespace SonikLib
{
    //using SFileSystemController = SonikLib::SharedSmtPtr<SonikLib::FileSystemControllers::SonikFileSystemController>;
    SLIB_CVR_USING(SFileSystemController, SonikLib::SharedSmtPtr<SonikLib::FileSystemControllers::SonikFileSystemController>);

    namespace FileSystemControllers
    {

        class SonikFileSystemController
        {
        private:
            class FileSystemFunctions; //InnerFunctionClass
            class FSF_ParamBox; //InnerFunctionParamClass

            SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;
            SonikLib::S_CAS::SonikAtomicLock m_lock;
            FSF_ParamBox* parambox;

            void(** ret_void_func)(FSF_ParamBox&);
            uint64_t(** ret_uint64_func)(FSF_ParamBox&);
            int64_t(** ret_int64_func)(FSF_ParamBox&);
        private:
#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
            //コピー禁止&ムーブ禁止
            SonikFileSystemController(const SonikFileSystemController& _copy_) = delete;
            SonikFileSystemController(SonikFileSystemController&& _move_) = delete;
            SonikFileSystemController& operator =(const SonikFileSystemController& _copy_) = delete;
            SonikFileSystemController& operator =(SonikFileSystemController&& _move_) = delete;

#else //C++ 11 以下
            //コピーと代入の禁止
            SonikFileSystemController(const SonikFileSystemController& _copy_);
            SonikFileSystemController& operator =(const SonikFileSystemController& _copy_);

    #if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
            //MSVC2010ならmove可能なので定義だけしておく。
            SonikFileSystemController(SonikFileSystemController&& _move_);
            SonikFileSystemController& operator =(SonikFileSystemController&& _move_);

    #endif
#endif

            //コンストラクタ
            SonikFileSystemController(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
        public:
            //デストラクタ
            ~SonikFileSystemController(void);

            static bool CreateFileController(SFileSystemController& _out_);
            static bool CreateFileController(SFileSystemController& _out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

            //FileOpen
            bool OpenFile(SonikLib::SonikString _filepath_, FileSystem::FILEOPENSWITCH _open_switch_);
            bool OpenFile(SonikLib::SonikString _filepath_, FileSystem::FILEOPENSWITCH _open_switch_, SonikLib::FileSystem::FILEERROR& _errcode_); //out ErrorCode Version
            //FileClose
            void CloseFile(void);

            //現在のファイルサイズの取得
            uint64_t GetFileSize(void);
            uint64_t GetFileSize(SonikLib::FileSystem::FILEERROR& _errcode_);

            //シーク位置を最初に移動します。
            void SeekPointSet_Top(void);
            void SeekPointSet_Top(SonikLib::FileSystem::FILEERROR& _errcode_);
            //シーク位置を最後に移動します。
            void SeekPointSet_End(void);
            void SeekPointSet_End(SonikLib::FileSystem::FILEERROR& _errcode_);
            //現在ンお読み込みシーク位置から指定バイト数分移動します。
            void SeekPointSet_Point(int64_t _offset_);
            void SeekPointSet_Point(int64_t _offset_, SonikLib::FileSystem::FILEERROR& _errcode_);
            //現在のシーク位置を取得します。
            int64_t SeekPointGet(void);
            int64_t SeekPointGet(SonikLib::FileSystem::FILEERROR& _errcode_);

            //指定したサイズ分読み込みます。
            void Read(char* _buffer_, uint64_t _size_, bool isLargeUsed = false);
            void Read(char* _buffer_, uint64_t _size_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed = false);
            //バイナリデータ書き込み用
            void Write(char* _writevalue_, uint64_t _writesize_, bool isLargeUsed = false);
            void Write(char* _writevalue_, uint64_t _writesize_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed = false);

            //SonikStringの吐き出し方法で分けています。
            //テキストモードでオープンした状態だとファイル内の文字のエンコーディングがUTF-8に代わるといったことはありません。
            //バイナリで、追記以外...つまりすべての文字を再出力..となった場合は変換されるかもしれません。
            void Write_char(SonikLib::SonikString _writevalue_, bool isLargeUsed = false);
            void Write_char(SonikLib::SonikString _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed = false);
            void Write_UTF8(SonikLib::SonikString _writevalue_, bool isLargeUsed = false);
            void Write_UTF8(SonikLib::SonikString _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed = false);
            void Write_UTF16(SonikLib::SonikString _writevalue_, bool isLargeUsed = false);
            void Write_UTF16(SonikLib::SonikString _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed = false);

            //テキストモード専用　指定された行数文TEXTを読み込みます。
            void ReadText_Line(SonikLib::SonikString& _str_, uint64_t GetRowCnt = 1, bool isLargeUsed = false);
            void ReadText_Line(SonikLib::SonikString& _str_, SonikLib::FileSystem::FILEERROR& _errcode_, uint64_t GetRowCnt = 1, bool isLargeUsed = false);
            //テキストモード専用　指定された行数文TEXTを読み込ます。改行は削除され、改行で分割されたQueueとして取得します。
            void ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>& _GetLineQueue_, uint64_t GetRowCnt = 1, bool isLargeUsed = false);
            void ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikString>& _GetLineQueue_, SonikLib::FileSystem::FILEERROR& _errcode_, uint64_t GetRowCnt = 1, bool isLargeUsed = false);

        };

    }; //end namespace FileSystemController

}; //end namespace SonikLib

//


#endif /* SONIKFILECONTROLLER_H_ */