#include "SonikFileController.h"

#include <Container/SonikAtomicQueue.hpp>
#include <Container/RangedForContainer.hpp>
#include <SonikString/SonikStringConvert.hpp>
#include <CPPGrammarDefines.h>

#include <cstdint>
#include <new>


#if defined(_WIN64) || defined (_WIN32)
 //windows include
#include <windows.h>
#define SLIB_FILECONTROLLER_FILEPOINTER HANDLE

#else
 //linux, unix, apple included
 //環境がないためまだ実装できないので未実装
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SLIB_FILECONTROLLER_FILEPOINTER int32_t
#endif



namespace SonikLib
{
    namespace FileSystemControllers
    {
#if defined(_WIN64) || defined (_WIN32)
        //Windows Implemets
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR FileSystem::FILEERROR FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(DWORD _errorcode_) SLIB_CVR_NOEXCEPT;
#else
        //Linux, Unix, Apple Implemets
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR FileSystem::FILEERROR FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(uint64_t _errorcode_) SLIB_CVR_NOEXCEPT;
#endif

        static const uint64_t SLIB_FILECONTROLLER_FUNCTION_READ_WRITE_LIMITMEMORYSIZE = (8ULL << 30);

        //FileSystemFunctionsで使うパラメータボックス
        class SonikFileSystemController::FSF_ParamBox
        {
        public:
            SLIB_FILECONTROLLER_FILEPOINTER fp;
            FileSystem::FILEERROR errorcode;

            //以降は重複したり使いまわしするため各関数内の呼び出し元や呼び出し先で帳尻合わせをする。
            int64_t  arg1;  //offsetとかsizeとか。
            uint64_t arg2;  //メモリ使用上限とか。
            uint64_t arg3;  //取得行数とか。
            SonikLib::SonikStringDefault* str; //read格納先やwrite書き出し元
            SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> allocator; //アロケータ
            bool isLargeUseOk;  //メモリ使用上限突破許容フラグ
            SonikLib::Container::SonikAtomicQueue<SonikStringDefault>* queue; //行取得時のQueueとか。
            unsigned char* buffer; //書き込み元バッファ/読み込み保存先バッファとか。
        };

        //メインクラスからのみコールされる関数群を定義/実装するクラス。
        class SonikFileSystemController::FileSystemFunctions
        {
        public:
            enum RET_VOID_FUNC_INDEX : uint32_t
            {
                RET_VFI_SEEKTOP = 0,    //SeekPointSet_Top関数を指定するindex
                RET_VFI_SEEKEND,        //SeekPointSet_End関数を指定するindex
                RET_VFI_SEEKPOINT,      //SeekPointSet_Point関数を指定するindex
                RET_VFI_READ,           //Read関数を指定するindex
                RET_VFI_WRITE,          //Write関数を指定するindex
                RET_VFI_WRITE_CHAR,     //Write_char関数を指定するindex
                RET_VFI_WRITE_UTF8,     //Write_UTF8関数を指定するindex
                RET_VFI_WRITE_UTF16,    //Write_UTF16関数を指定するindex
                RET_VFI_READLINE,       //ReadText_Line関数を指定するindex
                RET_VFI_READLINEQUEUE,  //ReadText_LineQueue関数を指定するindex

                RET_VFI_CNT,            //ここより上の定義数です。
            };
            enum RET_UINT64_FUNC_INDEX : uint32_t
            {
                RET_UI64FI_GETFILESIZE = 0, //GetFileSize関数を指定するindex

                RET_U64FI_CNT, //ここより上の定義数です。
            };
            enum RET_INT64_FUNC_INDEX : uint32_t
            {
                RET_I64FI_SEEKPOINTGET = 0, //SeekPointGet関数を指定するindex

                RET_I64FI_CNT, //ここより上の定義数です。
            };
            
        public:
            //関数群===========================================================
            // OP = Open時関数、 NOP = 非オープン時間数
            //ファイルのクローズ
            static void CloseFile(FSF_ParamBox& argbox);
            //ファイルサイズの取得
            static uint64_t OP_GetFileSize(FSF_ParamBox& argbox);
            static uint64_t NOP_GetFileSize(FSF_ParamBox& argbox);
            //シーク位置変更(先頭へ)
            static void OP_SeekPointSet_Top(FSF_ParamBox& argbox);
            static void NOP_SeekPointSet_Top(FSF_ParamBox& argbox);
            //シーク位置変更(最後尾へ)
            static void OP_SeekPointSet_End(FSF_ParamBox& argbox);
            static void NOP_SeekPointSet_End(FSF_ParamBox& argbox);
            //シーク位置変更(任意位置へ)
            static void OP_SeekPointSet_Point(FSF_ParamBox& argbox);
            static void NOP_SeekPointSet_Point(FSF_ParamBox& argbox);
            //現在のシーク位置の取得
            static int64_t OP_SeekPointGet(FSF_ParamBox& argbox);
            static int64_t NOP_SeekPointGet(FSF_ParamBox& argbox);
            //任意サイズの読み込み(単位：Byte)
            static void OP_Read(FSF_ParamBox& argbox);
            static void NOP_Read(FSF_ParamBox& argbox);
            //任意サイズの書き込み(単位：Byte)
            static void OP_Write(FSF_ParamBox& argbox);
            static void NOP_Write(FSF_ParamBox& argbox);
            //SonikString型に設定した文字列をSJIS形式で書き込み
            static void OP_Write_char(FSF_ParamBox& argbox);
            static void NOP_Write_char(FSF_ParamBox& argbox);
            //SonikString型に設定した文字列をUTF-8(BOM無し)形式で書き込み
            static void OP_Write_UTF8(FSF_ParamBox& argbox);
            static void NOP_Write_UTF8(FSF_ParamBox& argbox);
            //SonikString型に設定した文字列をUTF-16(BOM無し)形式で書き込み
            static void OP_Write_UTF16(FSF_ParamBox& argbox);
            static void NOP_Write_UTF16(FSF_ParamBox& argbox);
            //任意行読み込んでSonikString型へ格納します。
            static void OP_ReadText_Line(FSF_ParamBox& argbox);
            static void NOP_ReadText_Line(FSF_ParamBox& argbox);
            //任意行読み込んでSonikString型のQueueへ１行単位でキューイングします。
            static void OP_ReadText_LineQueue(FSF_ParamBox& argbox);
            static void NOP_ReadText_LineQueue(FSF_ParamBox& argbox);

            //ReadText_Line(1行読み込み)の際にstrchrとstrwcschrで分岐が必要になったが1つの関数にすると関数ポインタかループ内ifかの選択肢が発生し、
            //可読性なら関数ポインタがスマートに思えたが速度も落としたくなかったので別途ルート分岐先関数を作成
            static void inner_ReadLineMemChr(SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> readcontainer, int64_t readsize_, int64_t before_readseekpoint, FSF_ParamBox& argbox);
            static void inner_ReadLineWMemChr(SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> readcontainer, int64_t readsize_, uint64_t ElemSize, int64_t before_readseekpoint, FSF_ParamBox& argbox);

        };

        //static関数実装============================================================================
        // OP = Open時関数、 NOP = 非オープン時間数
        //ファイルのクローズ
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::CloseFile(FSF_ParamBox& argbox)
        {
#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            if (argbox.fp != nullptr)
            {
                CloseHandle(argbox.fp);
                argbox.fp = nullptr;
            };
#else
            //Linux, Unix, Apple Implemets
            if (argbox.fp != -1)
            {
                close(argbox.fp);
                argbox.fp = -1;
            };
#endif
        };

        //ファイルサイズの取得
        DEF_FORCE_INLINE uint64_t SonikFileSystemController::FileSystemFunctions::OP_GetFileSize(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            LARGE_INTEGER ret{};
            if ((::GetFileSizeEx(argbox.fp, &ret)) == 0)
            {
                argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
                ret.QuadPart = 0;
            };

            return static_cast<uint64_t>(ret.QuadPart);
#else
            //Linux, Unix, Apple Implemets
#endif
        };
        DEF_FORCE_INLINE uint64_t SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return 0;
        };
        //シーク位置変更(先頭へ)
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Top(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            LARGE_INTEGER l_seek{};
            if ((::SetFilePointerEx(argbox.fp, l_seek, nullptr, FILE_BEGIN)) == 0)
            {
                argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
            };

#elif defined(__linux__)
            //linux definition
#endif
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //シーク位置変更(最後尾へ)
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_End(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            LARGE_INTEGER l_seek{};
            if ((::SetFilePointerEx(argbox.fp, l_seek, nullptr, FILE_END)) == 0)
            {
                argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
            };

#elif defined(__linux__)
            //linux definition
#endif
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //シーク位置変更(任意位置へ)
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            LARGE_INTEGER l_seek{};
            l_seek.QuadPart = argbox.arg1;
            //現在位置からoffset分移動。ファイルサイズを超えた場合は自動で拡張される。
            if ((::SetFilePointerEx(argbox.fp, l_seek, nullptr, FILE_CURRENT)) == 0)
            {
                argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
                return;
            };

#elif defined(__linux__)
            //linux definition
#endif
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //現在のシーク位置の取得
        DEF_FORCE_INLINE int64_t SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            LARGE_INTEGER l_zero{};
            LARGE_INTEGER l_seek{};
            //シーク現在位置を取得。
            if ((::SetFilePointerEx(argbox.fp, l_zero, &l_seek, FILE_CURRENT)) == 0)
            {
                argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
                l_seek.QuadPart = -1;
            };

            return l_seek.QuadPart;

#elif defined(__linux__)
            //linux definition
#endif
        };
        DEF_FORCE_INLINE int64_t SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return -1;
        };
        //任意サイズの読み込み(単位：Byte)
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_Read(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

            //サイズがマイナス値や0Byte指定なら即終了
            if (argbox.arg1 <= 0)
            {
                argbox.errorcode = FileSystem::FILEERROR::FERR_READBYTE_NEGATIVEVALUE;
                return;
            };
            //読み込み先バッファがnullptrなら即終了
            if (argbox.buffer == nullptr)
            {
                argbox.errorcode = FileSystem::FILEERROR::FERR_READBUFFER_NULL;
                return;
            };
            //_size_がlimit以上指定されたら拒否。ただし、第４引数でユーザーがリスクを承知の上でフラグをtrueにするなら許容。
            if (static_cast<uint64_t>(argbox.arg1) >= argbox.arg2)
            {
                //フラグは許容しているか？
                if (!argbox.isLargeUseOk)
                {
                    //してないのでエラー終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_LARGEREADSIZE;
                    return;
                };
            };

#if defined(_WIN64) || defined (_WIN32)
            uint64_t cast_size = static_cast<uint64_t>(argbox.arg1);
            SLIB_CVR_CONSTEXPR uint64_t cast_uint32_max = static_cast<uint64_t>(UINT32_MAX);
            DWORD ReadCompByte = 0;
            DWORD readsize = 0;
            unsigned char*& l_buffer = argbox.buffer;
            SLIB_FILECONTROLLER_FILEPOINTER l_fp = argbox.fp;

            //break(or return)条件はすべて正常にRead終了時 or 短読み時 or エラー発生時
            while (cast_size > 0)
            {
                //std::minの代わり。なんとなく。
                readsize = static_cast<DWORD>((cast_size < cast_uint32_max) ? cast_size : cast_uint32_max);

                //エラーが発生したら問答無用で終了
                if ((::ReadFile(l_fp, l_buffer, readsize, &ReadCompByte, nullptr)) == 0)
                {
                    argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
                    return;
                };

                //成功して読み込み0なら終端と判断
                if (ReadCompByte == 0)
                {
                    //EOF検知と判断する。
                    argbox.errorcode = FileSystem::FILEERROR::FERR_READ_EOF;
                    break;
                };

                //短読みの可能性があるらしいのでチェックしてEOFとして扱う
                if (ReadCompByte < readsize)
                {
                    //EOF検知と判断する。
                    argbox.errorcode = FileSystem::FILEERROR::FERR_READ_EOF;
                    break;
                };

                //それ以外はループ
                l_buffer += ReadCompByte;
                cast_size -= ReadCompByte;

            };

            return;

#elif defined(__linux__)
            //linux definition
            return;//環境がないため未実装
#endif
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_Read(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //任意サイズの書き込み(単位：Byte)
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_Write(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;

            //サイズがマイナス値や0Byte指定なら即終了
            if (argbox.arg1 <= 0)
            {
                argbox.errorcode = FileSystem::FILEERROR::FERR_WRITEBYTE_NEGATIVEVALUE;
                return;
            };
            //読み込み先バッファがnullptrなら即終了
            if (argbox.buffer == nullptr)
            {
                argbox.errorcode = FileSystem::FILEERROR::FERR_WRITEBUFFER_NULL;
                return;
            };
            //WriteSizeが8GB指定されたら拒否。ただし、第４引数でユーザーがリスクを承知の上でフラグをtrueにするなら許容。
            if (static_cast<uint64_t>(argbox.arg1) >= argbox.arg2)
            {
                //フラグは許容しているか？
                if (!argbox.isLargeUseOk)
                {
                    //してないのでエラー終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_LARGEWRITESIZE;
                    return;
                };
            };

#if defined(_WIN64) || defined (_WIN32)
            uint64_t cast_writesize = static_cast<uint64_t>(argbox.arg1);
            SLIB_CVR_CONSTEXPR uint64_t cast_uint32_max = static_cast<uint64_t>(UINT32_MAX);
            DWORD WriteCompByte = 0;
            DWORD writesize = 0;
            unsigned char*& l_buffer = argbox.buffer;
            SLIB_FILECONTROLLER_FILEPOINTER l_fp = argbox.fp;

            //break(or return)条件はすべて正常にWrite終了時 or エラー発生時
            while (cast_writesize > 0)
            {
                //std::minの代わり。なんとなく。
                writesize = static_cast<DWORD>((cast_writesize < cast_uint32_max) ? cast_writesize : cast_uint32_max);

                //エラーが発生したら問答無用で終了
                if ((::WriteFile(l_fp, l_buffer, writesize, &WriteCompByte, nullptr)) == 0)
                {
                    argbox.errorcode = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(::GetLastError());
                    return;
                };

                //書き込みバイト数が0の場合
                if (WriteCompByte == 0)
                {
                    //DISKFULL等の可能性があるためエラーとして扱う。(たぶんその場合WriteFileが 非0を返却しそうだが...。)
                    argbox.errorcode = FileSystem::FILEERROR::FERR_WRITEFILED;
                    return;
                };

                //要求より少なく書き込めた場合もエラー扱いとする。
                if (writesize > WriteCompByte)
                {
                    //部分書き込みエラーとして終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_SECTION_WRITE;
                    return;
                };

                //それ以外はループ
                l_buffer += WriteCompByte;
                cast_writesize -= WriteCompByte;

            };

            return;

#elif defined(__linux__)
            //linux definition
            return;//環境がないため未実装
#endif
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_Write(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //SonikString型に設定した文字列をSJIS形式で書き込み
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_Write_char(FSF_ParamBox& argbox)
        {
            uint64_t l_size = 0;
            l_size = argbox.str->GetCpy_str_c(nullptr); //文字列の合計Byte数を取得

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                argbox.errorcode = FileSystem::FILEERROR::FERR_WRITEBYTE_NEGATIVEVALUE;
                return;
            };
            //書き込む文字のByte数が大きすぎるならエラー。ただし使用者がリスクを許容するフラグを立てていれば通す。
            if (l_size >= argbox.arg2) //8GB
            {
                //フラグは許容しているか？
                if (!argbox.isLargeUseOk)
                {
                    //してないのでエラー終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_LARGEWRITESIZE;
                    return;
                };
            };

            //領域を取得
            void* l_allocArea = argbox.allocator->memal(l_size);
            if (l_allocArea == nullptr)
            {
                //リソース不足
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //charで初期化。(placement newなので失敗しない。)
            char* l_strbuffer = new(l_allocArea) char();

            //文字列コピー
            if (argbox.str->GetCpy_str_c(l_strbuffer) == 0)
            {

                //リソース不足
                argbox.allocator->memdel(l_strbuffer);
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //自分のWriteを使用
            argbox.buffer = reinterpret_cast<unsigned char*>(l_strbuffer);
            argbox.arg1 = l_size;
            SonikFileSystemController::FileSystemFunctions::OP_Write(argbox);

            //解放
            argbox.allocator->memdel(l_strbuffer);
            argbox.buffer = nullptr;
            //終了
            return;
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_Write_char(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //SonikString型に設定した文字列をUTF-8(BOM無し)形式で書き込み
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_Write_UTF8(FSF_ParamBox& argbox)
        {
            uint64_t l_size = 0;
            l_size = argbox.str->GetCpy_str_utf8(nullptr); //文字列の合計Byte数を取得

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                argbox.errorcode = FileSystem::FILEERROR::FERR_WRITEBYTE_NEGATIVEVALUE;
                return;
            };
            //書き込む文字のByte数が大きすぎるならエラー。ただし使用者がリスクを許容するフラグを立てていれば通す。
            if (l_size >= argbox.arg2) //8GB
            {
                //フラグは許容しているか？
                if (!argbox.isLargeUseOk)
                {
                    //してないのでエラー終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_LARGEWRITESIZE;
                    return;
                };
            };

            //領域を取得
            void* l_allocArea = argbox.allocator->memal(l_size);
            if (l_allocArea == nullptr)
            {
                //リソース不足
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //charで初期化。(placement newなので失敗しない。)
            utf8_t* l_strbuffer = new(l_allocArea) utf8_t();

            //文字列コピー
            if (argbox.str->GetCpy_str_utf8(l_strbuffer) == 0)
            {
                //リソース不足
                argbox.allocator->memdel(l_strbuffer);
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //自分のWriteを使用
            argbox.buffer = reinterpret_cast<unsigned char*>(l_strbuffer);
            argbox.arg1 = l_size;
            SonikFileSystemController::FileSystemFunctions::OP_Write(argbox);

            //解放
            argbox.allocator->memdel(l_strbuffer);
            argbox.buffer = nullptr;

            //終了
            return;
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //SonikString型に設定した文字列をUTF-16(BOM無し)形式で書き込み
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_Write_UTF16(FSF_ParamBox& argbox)
        {
            uint64_t l_size = 0;
            l_size = argbox.str->GetCpy_str_utf16(nullptr); //文字列の合計Byte数を取得

            //長さ0 文字列だったら処理
            if (l_size == 0)
            {
                //何も書き込まず終了
                return;
            };
            //書き込む文字のByte数が大きすぎるならエラー。ただし使用者がリスクを許容するフラグを立てていれば通す。
            if (l_size >= argbox.arg2) //8GB
            {
                //フラグは許容しているか？
                if (!argbox.isLargeUseOk)
                {
                    //してないのでエラー終了
                    argbox.errorcode = FileSystem::FILEERROR::FERR_LARGEWRITESIZE;
                    return;
                };
            };

            //領域を取得
            void* l_allocArea = argbox.allocator->memal(l_size);
            if (l_allocArea == nullptr)
            {
                //リソース不足
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //charで初期化。(placement newなので失敗しない。)
            char16_t* l_strbuffer = new(l_allocArea) char16_t();

            //文字列コピー
            if (argbox.str->GetCpy_str_utf16(l_strbuffer) == 0)
            {
                //リソース不足
                argbox.allocator->memdel(l_strbuffer);
                argbox.errorcode = FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                return;
            };

            //自分のWriteを使用
            argbox.buffer = reinterpret_cast<unsigned char*>(l_strbuffer);
            argbox.arg1 = l_size;
            SonikFileSystemController::FileSystemFunctions::OP_Write(argbox);

            //解放
            argbox.allocator->memdel(l_strbuffer);
            argbox.buffer = nullptr;

            //終了
            return;
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //任意行読み込んでSonikString型へ格納します。
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_ReadText_Line(FSF_ParamBox& argbox)
        {
            int64_t l_readsize = 10240;
            int64_t l_before_readseekpoint = SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet(argbox); //処理前シーク位置
            if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
            {
                return;
            };
            SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> l_container;
            if (!SonikLib::Container::SonikVariableArrayContainer<char>::CreateContainer(l_container, argbox.allocator, 10250))
            {
                argbox.errorcode = FileSystem::FILEERROR::FERR_EXCEPTION;
                return;
            };

            argbox.buffer = reinterpret_cast<unsigned char*>(&(*l_container)[0]);
            argbox.arg1 = l_readsize;
            SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);

            //Readの結果、エラーコードが設定されていて、EOFとSCCEEDED以外なら終了
            if ((argbox.errorcode != FileSystem::FILEERROR::FERR_READ_EOF) && (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED))
            {
                return;
            };

            //1文字もとれなければ終了
            if ((*l_container)[0] == '\0')
            {
                //文字無し
                (*argbox.str) = "";
                return;
            };

            //１回目の読み込みで文字列の型をチェック
            SonikLibConvertType l_convtype = SonikLibConvertType::SCHTYPE_UNKNOWN;
            l_convtype = SonikLibStringConvert::CheckConvertType(&(*l_container)[0]);

            //文字コードタイプによって処理切り替え
            switch (l_convtype)
            {
            case SonikLibConvertType::SCHTYPE_SJIS:
            case SonikLibConvertType::SCHTYPE_UTF8:
            case SonikLibConvertType::SCHTYPE_BOMUTF8:
                //最小が1Byte単位の文字列
                SonikFileSystemController::FileSystemFunctions::inner_ReadLineMemChr(l_container, l_readsize, l_before_readseekpoint, argbox);
                break;

            case SonikLibConvertType::SCHTYPE_UTF16:
                //2Byte単位の文字列
                SonikFileSystemController::FileSystemFunctions::inner_ReadLineWMemChr(l_container, l_readsize, sizeof(char16_t), l_before_readseekpoint, argbox);
                break;

            case SonikLibConvertType::SCHTYPE_UTF32:
                //4Byte単位の文字列
                SonikFileSystemController::FileSystemFunctions::inner_ReadLineWMemChr(l_container, l_readsize, sizeof(char32_t), l_before_readseekpoint, argbox);
                break;

            default:
                //処理不可
                argbox.errorcode = FileSystem::FILEERROR::FERR_EXCEPTION;
                break;
            };

            //終了
            return;
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };
        //任意行読み込んでSonikString型のQueueへ１行単位でキューイングします。
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::OP_ReadText_LineQueue(FSF_ParamBox& argbox)
        {
            SonikLib::SonikStringDefault l_getstr(argbox.allocator);
            uint64_t loopcnt = argbox.arg3;

            argbox.str = &l_getstr;
            argbox.arg3 = 1;
            for (uint64_t i = 0; i < loopcnt; ++i)
            {
                SonikFileSystemController::FileSystemFunctions::OP_ReadText_Line(argbox);
                if ((argbox.errorcode != FileSystem::FILEERROR::FERR_READ_EOF) && (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED))
                {
                    //EOFや成功コード以外なら終了
                    return;
                };

                //EOFならbreakとなるが、EOFでも途中まで読み込んでEOFとなっている場合があるためQueueにはPushを行う。
                argbox.queue->EnQueue(l_getstr); //Push。

                if (argbox.errorcode == FileSystem::FILEERROR::FERR_READ_EOF)
                {
                    //EOFならBreak;
                    break;
                };

                l_getstr = "";
            };

            argbox.arg3 = loopcnt;
            argbox.str = nullptr;

            return;
        };
        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue(FSF_ParamBox& argbox)
        {
            argbox.errorcode = FileSystem::FILEERROR::FERR_NOT_FILEOPENSTATE;
            return;
        };


        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::inner_ReadLineMemChr(SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> readcontainer, int64_t readsize_, int64_t before_readseekpoint, FSF_ParamBox& argbox)
        {
            //オフセット値(ReadSizeと一緒)
            uint64_t l_container_offset = readsize_;
            uint64_t buffer_address_diff = 0;
            //1回チェック
            char* retpoint = static_cast<char*>(memchr(&(*readcontainer)[0], '\n', readsize_));
            if (retpoint != 0)
            {
                //一旦置き換え
                (*retpoint) = '\0';

                //バッファの先頭とmemchrで見つかったアドレス位置との差分を取る。
                buffer_address_diff = retpoint - (&(*readcontainer)[0]);
            };

            //0 なら取れてないのでループ処理へ。
            while (retpoint == 0)
            {
                //領域の拡大
                readcontainer->ReSize(readcontainer->GetSizeMax() + readsize_);

                //読み込み
                argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[l_container_offset]);
                argbox.arg1 = readsize_;
                SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);

                //EOFに到達なら終了
                if (argbox.errorcode == FileSystem::FILEERROR::FERR_READ_EOF)
                {
                    //EOFに到達したら、１行の文字列として返却して終了
                    (*argbox.str) = (*readcontainer)[0];
                    return;
                };

                //オフセット分移動した先で関数実行し、結果を取得
                retpoint = static_cast<char*>(memchr(&(*readcontainer)[l_container_offset], '\n', readsize_));
                if (retpoint != 0)
                {
                    //一旦置き換え
                    (*retpoint) = '\0';

                    //バッファの先頭とmemchrで見つかったアドレス位置との差分を取る。
                    buffer_address_diff = retpoint - (&(*readcontainer)[0]);
                    break;
                };

                l_container_offset += readsize_;
            };

            //0 以外が取れたら１行として取得
            //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
            //読み込み後の現在のシーク位置を取得する。
            int64_t l_after_seekpoint = SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet(argbox);
            if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
            {
                //エラーコードが帰ってきたら終了
                (*argbox.str) = "";
                return;
            };

            //diff分進めたシーク位置と現在の位置との差分をとり、差分のぶんだけ戻す。
            // +1 は\0 で置き換えた分
            argbox.arg1 = ((before_readseekpoint + buffer_address_diff) +1) -l_after_seekpoint;
            //ファイルポインタを戻す
            SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point(argbox); //戻す。
            if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
            {
                //エラーコードが帰ってきたら終了
                (*argbox.str) = "";
                return;
            };

            //引っかかれば改行文字は終端文字に変更しているので、そのまま代入して改行を付け加える
            (*argbox.str) += (&(*readcontainer)[0]);
            (*argbox.str) += "\n";

            //クリアして次へ
            readcontainer->Clear();

            //すでに1行分は取れているので、残りの行数分↑の処理を実施。(↑の処理は最初に外側で10240Byte分取得していて、文字コード判別等をしているためループに組み込めないので、似ている処理になるがここからループ)
            for (uint64_t l_rowcnt = 1; l_rowcnt < argbox.arg3; ++l_rowcnt)
            {
                before_readseekpoint = SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet(argbox); //処理前シーク位置
                if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    return;
                };

                argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[0]);
                argbox.arg1 = readsize_;
                SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);
                //Readの結果、エラーコードが設定されていて、EOFとSCCEEDED以外なら終了
                if ((argbox.errorcode != FileSystem::FILEERROR::FERR_READ_EOF) && (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED))
                {
                    return;
                };

                //チェック
                buffer_address_diff = 0;
                retpoint = static_cast<char*>(memchr(&(*readcontainer)[0], '\n', readsize_));
                if (retpoint != 0)
                {
                    //置き換え
                    (*retpoint) = '\0';

                    //バッファの先頭とmemchrで見つかったアドレス位置との差分を取る。
                    buffer_address_diff = retpoint - (&(*readcontainer)[0]);
                };

                //0 なら取れてないのでループ処理へ。
                l_container_offset = readsize_;
                while (retpoint == 0)
                {
                    //領域の拡大
                    readcontainer->ReSize(readcontainer->GetSizeMax() + readsize_);

                    //読み込み
                    argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[l_container_offset]);
                    argbox.arg1 = readsize_;
                    SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);

                    //EOFに到達なら終了
                    if (argbox.errorcode == FileSystem::FILEERROR::FERR_READ_EOF)
                    {
                        //EOFに到達したら、１行の文字列として返却して終了
                        (*argbox.str) = (*readcontainer)[0];
                        return;
                    };

                    //オフセット分移動した先で関数実行し、結果を取得
                    retpoint = static_cast<char*>(memchr(&(*readcontainer)[l_container_offset], '\n', readsize_));
                    if (retpoint != 0)
                    {
                        //置き換え
                        (*retpoint) = '\0';

                        //バッファの先頭とmemchrで見つかったアドレス位置との差分を取る。
                        buffer_address_diff = retpoint - (&(*readcontainer)[0]);
                    };

                    l_container_offset += readsize_;

                }; //end while

                //0 以外が取れたら１行として取得
                //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
                //読み込み後の現在のシーク位置を取得する。
                l_after_seekpoint = SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet(argbox);
                if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    //エラーコードが帰ってきたら終了
                    (*argbox.str) = "";
                    return;
                };

                //diff分進めたシーク位置と現在の位置との差分をとり、差分のぶんだけ戻す。
                // +1 は\0 で置き換えた分
                argbox.arg1 = ((before_readseekpoint + buffer_address_diff) + 1) - l_after_seekpoint;
                //ファイルポインタを戻す
                SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point(argbox); //戻す。
                if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    //エラーコードが帰ってきたら終了
                    (*argbox.str) = "";
                    return;
                };

                //引っかかれば改行文字は終端文字に変更しているので、そのまま代入して改行を付け加える
                (*argbox.str) += (&(*readcontainer)[0]);
                (*argbox.str) += "\n";

                //クリアして次へ
                readcontainer->Clear();

            };//end for

        }; // end function

        DEF_FORCE_INLINE void SonikFileSystemController::FileSystemFunctions::inner_ReadLineWMemChr(SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<char>> readcontainer, int64_t readsize_, uint64_t ElemSize, int64_t before_readseekpoint, FSF_ParamBox& argbox)
        {
            //オフセット値(ReadSizeと一緒)
            uint64_t l_container_offset = readsize_;
            uint64_t l_readelem_cnt = readsize_ / ElemSize;

            //1回チェック
            uintptr_t retpoint = reinterpret_cast<uintptr_t>(wmemchr(reinterpret_cast<wchar_t*>(&(*readcontainer)[0]), '\n', l_readelem_cnt));
            if (retpoint != 0)
            {
                //一旦置き換え
                (*reinterpret_cast<char*>(retpoint)) = '\0';
            };

            //0 なら取れてないのでループ処理へ。
            while (retpoint == 0)
            {
                //領域の拡大
                readcontainer->ReSize(readcontainer->GetSizeMax() + readsize_);

                //読み込み
                argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[l_container_offset]);
                argbox.arg1 = readsize_;
                SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);

                //EOFに到達なら終了
                if (argbox.errorcode == FileSystem::FILEERROR::FERR_READ_EOF)
                {
                    //EOFに到達したら、１行の文字列として返却して終了
                    (*argbox.str) = (*readcontainer)[0];
                    return;
                };

                //オフセット分移動した先で関数実行し、結果を取得
                retpoint = reinterpret_cast<uintptr_t>(wmemchr(reinterpret_cast<wchar_t*>(&(*readcontainer)[l_container_offset]), '\n', l_readelem_cnt));
                if (retpoint != 0)
                {
                    //一旦置き換え
                    (*reinterpret_cast<char*>(retpoint)) = '\0';
                };

                l_container_offset += readsize_;
            };

            //0 以外が取れたら１行として取得
            //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
            argbox.arg1 = ((readcontainer->GetSizeMax() - (retpoint - reinterpret_cast<uintptr_t>(&(*readcontainer)[0]))) * (-1)) + 1;
            //ファイルポインタを戻す
            SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point(argbox);
            if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
            {
                //エラーコードが帰ってきたら終了
                (*argbox.str) = "";
                return;
            };

            //引っかかれば改行文字は終端文字に変更しているので、そのまま代入して改行を付け加える
            (*argbox.str) += (&(*readcontainer)[0]);
            (*argbox.str) += "\n";

            //クリアして次へ
            readcontainer->Clear();

            //すでに1行分は取れているので、残りの行数分↑の処理を実施。(↑の処理は最初に外側で10240Byte分取得していて、文字コード判別等をしているためループに組み込めないので、似ている処理になるがここからループ)
            for (uint64_t l_rowcnt = 1; l_rowcnt < argbox.arg3; ++l_rowcnt)
            {
                argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[0]);
                argbox.arg1 = readsize_;
                SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);
                //Readの結果、エラーコードが設定されていて、EOFとSCCEEDED以外なら終了
                if ((argbox.errorcode != FileSystem::FILEERROR::FERR_READ_EOF) && (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED))
                {
                    return;
                };

                //チェック
                retpoint = reinterpret_cast<uintptr_t>(wmemchr(reinterpret_cast<wchar_t*>(&(*readcontainer)[0]), '\n', l_readelem_cnt));
                if (retpoint != 0)
                {
                    //置き換え
                    (*reinterpret_cast<char*>(retpoint)) = '\0';
                };

                //0 なら取れてないのでループ処理へ。
                l_container_offset = readsize_;
                while (retpoint == 0)
                {
                    //領域の拡大
                    readcontainer->ReSize(readcontainer->GetSizeMax() + readsize_);

                    //読み込み
                    argbox.buffer = reinterpret_cast<unsigned char*>(&(*readcontainer)[l_container_offset]);
                    argbox.arg1 = readsize_;
                    SonikFileSystemController::FileSystemFunctions::OP_Read(argbox);

                    //EOFに到達なら終了
                    if (argbox.errorcode == FileSystem::FILEERROR::FERR_READ_EOF)
                    {
                        //EOFに到達したら、１行の文字列として返却して終了
                        (*argbox.str) = (*readcontainer)[0];
                        return;
                    };

                    //オフセット分移動した先で関数実行し、結果を取得
                    retpoint = reinterpret_cast<uintptr_t>(wmemchr(reinterpret_cast<wchar_t*>(&(*readcontainer)[l_container_offset]), '\n', l_readelem_cnt));
                    if (retpoint != 0)
                    {
                        //一旦置き換え
                        (*reinterpret_cast<char*>(retpoint)) = '\0';
                    };

                    l_container_offset += readsize_;

                }; //end while

                //0 以外が取れたら１行として取得
                //改行までファイルポインタを戻す際のオフセットを算出(戻す方向のオフセットなので負の値にしておく。)
                argbox.arg1 = ((readcontainer->GetSizeMax() - (retpoint - reinterpret_cast<uintptr_t>(&(*readcontainer)[0]))) * (-1)) + 1;
                //ファイルポインタを戻す
                SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point(argbox);
                if (argbox.errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    //エラーコードが帰ってきたら終了
                    (*argbox.str) = "";
                    return;
                };

                //引っかかれば改行文字は終端文字に変更しているので、そのまま代入して改行を付け加える
                (*argbox.str) += (&(*readcontainer)[0]);
                (*argbox.str) += "\n";

                //クリアして次へ
                readcontainer->Clear();

            };//end for

        }; // end function

        //==========================================================================================

        //______________________________________________________________________________________________________________________________
        // 
        // 親クラス実装
        //______________________________________________________________________________________________________________________________
        //コンストラクタ
        SonikFileSystemController::SonikFileSystemController(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
        :m_allocator(_allocator_)
        ,parambox(nullptr)
        ,ret_void_func(nullptr)
        ,ret_uint64_func(nullptr)
        ,ret_int64_func(nullptr)
        {/*none*/};

        //デストラクタ
        SonikFileSystemController::~SonikFileSystemController(void)
        {
            if(ret_void_func != nullptr)
            {
                m_allocator->memdelArray(ret_void_func, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_CNT);
                ret_void_func = nullptr;
            };
            if(ret_uint64_func != nullptr)
            {
                m_allocator->memdelArray(ret_uint64_func, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX::RET_U64FI_CNT);
                ret_uint64_func = nullptr;
            };
            if(ret_int64_func != nullptr)
            {
                m_allocator->memdelArray(ret_int64_func, SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX::RET_I64FI_CNT);
                ret_int64_func = nullptr;
            };

            if(parambox != nullptr)
            {
                SonikFileSystemController::FileSystemFunctions::CloseFile((*parambox));

                m_allocator->memdel(parambox);
                parambox = nullptr;
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

            //FileSystemController本体生成
            void* l_allocbuffer = l_defalloc->memal(sizeof(SonikFileSystemController));
            if (l_allocbuffer == nullptr)
            {
                return false;
            };

            SonikFileSystemController* ptmp;
            ptmp = new(l_allocbuffer) SonikFileSystemController(l_allocSmtPtr);

            //スマートポインタに入れる前に生ポ操作でメンバクラスのオブジェクトを作っていく。
            //アロケータはコンストラクタで設定しているので..。paramboxを作って作成
            FSF_ParamBox* l_parambox = nullptr;
            //領域取得
            l_allocbuffer = l_defalloc->memal(sizeof(FSF_ParamBox));
            if(l_allocbuffer == nullptr)
            {
                return false;
            };
            //領域が取れたのでplacement new。領域確保済みなので失敗しない。
            l_parambox = new(l_allocbuffer) FSF_ParamBox;
            //環境によってfpの初期値をセット
            l_parambox->allocator = l_allocSmtPtr;
#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            l_parambox->fp = nullptr;
#else
            //Linux, Unix, Apple Implemets
            l_parambox->fp = -1;
#endif
            l_parambox->arg2 = (8ULL << 30); //8GB

            //本体にセット
            ptmp->parambox = l_parambox; //ここ以降は本体(ptmp)のデストラクタで処理されるのでこいつはdeleteしない。

            //各関数ポインタ用配列の生成
            void(** l_voidfunc)(FSF_ParamBox&);
            uint64_t(** l_uint64func)(FSF_ParamBox&);
            int64_t(** l_int64func)(FSF_ParamBox&);

            void* l_vfi_func_allocbuffer = nullptr;;
            void* l_ui64fi_func_allocbuffer = nullptr;
            void* l_i64fi_func_allocbuffer = nullptr;

            SLIB_CVR_USING(def_vfi    , void(*)(FSF_ParamBox&));
            SLIB_CVR_USING(def_ui64vfi, uint64_t(*)(FSF_ParamBox&));
            SLIB_CVR_USING(def_i64vfi , int64_t(*)(FSF_ParamBox&));
            SLIB_CVR_USING(VOIDFUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX);
            SLIB_CVR_USING(UI64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX);
            SLIB_CVR_USING(I64FUNCINDEX , SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX);

            l_vfi_func_allocbuffer    = l_defalloc->memal(sizeof(void(FileSystemFunctions::*)(FSF_ParamBox&)) * VOIDFUNCINDEX::RET_VFI_CNT);
            l_ui64fi_func_allocbuffer = l_defalloc->memal(sizeof(uint64_t(FileSystemFunctions::*)(FSF_ParamBox&)) * UI64FUNCINDEX::RET_U64FI_CNT);
            l_i64fi_func_allocbuffer  = l_defalloc->memal(sizeof(int64_t(FileSystemFunctions::*)(FSF_ParamBox&)) * I64FUNCINDEX::RET_I64FI_CNT);
            
            bool allocated_ok = true;
            allocated_ok &= (!!l_vfi_func_allocbuffer);
            allocated_ok &= (!!l_ui64fi_func_allocbuffer);
            allocated_ok &= (!!l_i64fi_func_allocbuffer);

            if (!allocated_ok)
            {
                l_defalloc->memdel(l_vfi_func_allocbuffer);
                l_defalloc->memdel(l_ui64fi_func_allocbuffer);
                l_defalloc->memdel(l_i64fi_func_allocbuffer);

                ptmp->~SonikFileSystemController();
                l_defalloc->memdel(ptmp);
                return false;
            };

            l_voidfunc   = reinterpret_cast<def_vfi*>(l_vfi_func_allocbuffer);
            l_uint64func = reinterpret_cast<def_ui64vfi*>(l_ui64fi_func_allocbuffer);
            l_int64func = reinterpret_cast<def_i64vfi*>(l_i64fi_func_allocbuffer);

            //非オープン関数で初期化しておく。
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKTOP]       = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKEND]       = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKPOINT]     = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READ]          = &SonikFileSystemController::FileSystemFunctions::NOP_Read;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE]         = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]   = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READLINE]      = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            l_uint64func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize;

            l_int64func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet;

            //本体にセット
            ptmp->ret_void_func   = l_voidfunc;
            ptmp->ret_uint64_func = l_uint64func;
            ptmp->ret_int64_func  = l_int64func;

            if (!SFileSystemController::SmartPointerCreate(ptmp, _out_))
            {
                ptmp->~SonikFileSystemController();
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

            //スマートポインタに入れる前に生ポ操作でメンバクラスのオブジェクトを作っていく。
            //アロケータはコンストラクタで設定しているので..。paramboxを作って作成
            FSF_ParamBox* l_parambox = nullptr;
            //領域取得
            l_allocbuffer = _allocator_->memal(sizeof(FSF_ParamBox));
            if (l_allocbuffer == nullptr)
            {
                return false;
            };
            //領域が取れたのでplacement new。領域確保済みなので失敗しない。
            l_parambox = new(l_allocbuffer) FSF_ParamBox;
            //環境によってfpの初期値をセット
            l_parambox->allocator = _allocator_;
#if defined(_WIN64) || defined (_WIN32)
            //Windows Implemets
            l_parambox->fp = nullptr;
#else
            //Linux, Unix, Apple Implemets
            l_parambox->fp = -1;
#endif
            l_parambox->arg2 = (8ULL << 30); //8GB
            //本体にセット
            ptmp->parambox = l_parambox; //ここ以降は本体(ptmp)のデストラクタで処理されるのでこいつはdeleteしない。


            //各関数ポインタ用配列の生成
            void(**l_voidfunc)(FSF_ParamBox&);
            uint64_t(**l_uint64func)(FSF_ParamBox&);
            int64_t(**l_int64func)(FSF_ParamBox&);

            void* l_vfi_func_allocbuffer = nullptr;;
            void* l_ui64fi_func_allocbuffer = nullptr;
            void* l_i64fi_func_allocbuffer = nullptr;

            SLIB_CVR_USING(def_vfi, void(*)(FSF_ParamBox&));
            SLIB_CVR_USING(def_ui64vfi, uint64_t(*)(FSF_ParamBox&));
            SLIB_CVR_USING(def_i64vfi, int64_t(*)(FSF_ParamBox&));
            SLIB_CVR_USING(VOIDFUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX);
            SLIB_CVR_USING(UI64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX);
            SLIB_CVR_USING(I64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX);


            l_vfi_func_allocbuffer    = _allocator_->memal(sizeof(void(FileSystemFunctions::*)(FSF_ParamBox&)) * VOIDFUNCINDEX::RET_VFI_CNT);
            l_ui64fi_func_allocbuffer = _allocator_->memal(sizeof(uint64_t(FileSystemFunctions::*)(FSF_ParamBox&)) * UI64FUNCINDEX::RET_U64FI_CNT);
            l_i64fi_func_allocbuffer  = _allocator_->memal(sizeof(int64_t(FileSystemFunctions::*)(FSF_ParamBox&)) * I64FUNCINDEX::RET_I64FI_CNT);

            bool allocated_ok = true;
            allocated_ok &= (!!l_vfi_func_allocbuffer);
            allocated_ok &= (!!l_ui64fi_func_allocbuffer);
            allocated_ok &= (!!l_i64fi_func_allocbuffer);

            if (!allocated_ok)
            {
                _allocator_->memdel(l_vfi_func_allocbuffer);
                _allocator_->memdel(l_ui64fi_func_allocbuffer);
                _allocator_->memdel(l_i64fi_func_allocbuffer);

                ptmp->~SonikFileSystemController();
                _allocator_->memdel(ptmp);
                return false;
            };

            l_voidfunc = reinterpret_cast<def_vfi*>(l_vfi_func_allocbuffer);
            l_uint64func = reinterpret_cast<def_ui64vfi*>(l_ui64fi_func_allocbuffer);
            l_int64func = reinterpret_cast<def_i64vfi*>(l_i64fi_func_allocbuffer);

            //非オープン関数で初期化しておく。
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKTOP]       = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKEND]       = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_SEEKPOINT]     = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READ]          = &SonikFileSystemController::FileSystemFunctions::NOP_Read;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE]         = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]   = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READLINE]      = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
            l_voidfunc[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            l_uint64func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize;

            l_int64func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet;

            //本体にセット
            ptmp->ret_void_func = l_voidfunc;
            ptmp->ret_uint64_func = l_uint64func;
            ptmp->ret_int64_func = l_int64func;


            if (!SFileSystemController::SmartPointerCreate(ptmp, _out_, _allocator_))
            {
                ptmp->~SonikFileSystemController();
                _allocator_->memdel(ptmp);
                return false;
            };

            return true;
        };


        //FileOpen
        bool SonikFileSystemController::OpenFile(SonikLib::SonikStringDefault _filepath_, FileSystem::FILEOPENSWITCH _open_switch_)
        {
            m_lock.lock();

            //エラーとする組み合わせをチェック処理
            if (_open_switch_ == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //0指定は問答無用で失敗
                m_lock.unlock();
                return false;
            };

            //READフラグが立っていて、WRITEフラグが立っていない場合
            if (((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
                && ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN))
            {
                //CLEAR と CREATEフラグが立っていれば、エラー(WRITEフラグが必要)
                if ((_open_switch_ & (FileSystem::FILEOPENSWITCH::FOSW_CREATE | FileSystem::FILEOPENSWITCH::FOSW_CLEAR)) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
                {
                    //失敗
                    m_lock.unlock();
                    return false;
                };
            };

            //共有モードは、使用する場合はオープンモードと同じでなけれならないため同じでないなら失敗
            //共有モード：読み込みが立っていた場合、READフラグがっていなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //失敗
                m_lock.unlock();
                return false;
            };
            //続けて、共有モード：書き込みが立っていた場合、WRITEフラグがなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //失敗
                m_lock.unlock();
                return false;
            };

            //一旦クローズ処理
            SonikFileSystemController::FileSystemFunctions::CloseFile((*parambox));
            //非オープン関数へセット
            SLIB_CVR_USING(VOIDFUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX);
            SLIB_CVR_USING(UI64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX);
            SLIB_CVR_USING(I64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX);

            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKTOP] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKEND] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKPOINT] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READ] = &SonikFileSystemController::FileSystemFunctions::NOP_Read;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE] = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            ret_uint64_func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize;

            ret_int64_func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet;

            //ここからプラットフォーム別処理===========================
#if defined(_WIN64)
            //ファイルハンドル(最後にメンバ変数に代入)
            HANDLE l_filehandle = nullptr;

            //オープンモードの設定
            DWORD l_openmode = 0;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_openmode |= GENERIC_READ;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_openmode |= GENERIC_WRITE;
            };

            //共有モードの設定
            DWORD l_sharemode = 0;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_sharemode |= FILE_SHARE_READ;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_sharemode |= FILE_SHARE_WRITE;
            };

            //オープンモードの設定
            DWORD l_opencontrol = OPEN_EXISTING;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CREATE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_opencontrol = OPEN_ALWAYS;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CLEAR) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
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

            parambox->fp = l_filehandle;

#elif defined(__linux__)

#endif
            //オープン状態、クローズ状態関数のセット
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKTOP] = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Top;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKEND] = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_End;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKPOINT] = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READ] = &SonikFileSystemController::FileSystemFunctions::OP_Read;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE] = &SonikFileSystemController::FileSystemFunctions::OP_Write;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR] = &SonikFileSystemController::FileSystemFunctions::OP_Write_char;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8] = &SonikFileSystemController::FileSystemFunctions::OP_Write_UTF8;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16] = &SonikFileSystemController::FileSystemFunctions::OP_Write_UTF16;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE] = &SonikFileSystemController::FileSystemFunctions::OP_ReadText_Line;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::OP_ReadText_LineQueue;

            ret_uint64_func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::OP_GetFileSize;

            ret_int64_func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet;

            //Writeフラグが立ってなければWrite系は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE] = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;

            };

            //テキストモードフラグが立っていなければWriteChar, ReadText関連は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_OPENTEXT) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16] = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            };

            //ADDフラグが立っていればファイルシークを最終へ
            //※現状ここでは常にオープン状態となるので直接コールする。
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_ADD) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_End((*parambox));
                if (parambox->errorcode != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    //error
                    return false;
                };

            };

            //終了
            m_lock.unlock();
            return true;
        };

        bool SonikFileSystemController::OpenFile(SonikLib::SonikStringDefault _filepath_, FileSystem::FILEOPENSWITCH _open_switch_, SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            //エラーとする組み合わせをチェック処理
            if (_open_switch_ == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //0指定は問答無用で失敗
                _errcode_ = FileSystem::FILEERROR::FERR_OPENSTATE_UKNOWN;
                m_lock.unlock();
                return false;
            };

            //READフラグが立っていて、WRITEフラグが立っていない場合
            if (((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
                && ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN))
            {
                //CLEAR と CREATEフラグが立っていれば、エラー(WRITEフラグが必要)
                if ((_open_switch_ & (FileSystem::FILEOPENSWITCH::FOSW_CREATE | FileSystem::FILEOPENSWITCH::FOSW_CLEAR)) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
                {
                    //失敗
                    _errcode_ = FileSystem::FILEERROR::FERR_OPENSTATE_READ_MISSMATCH_CLEAR_CREATE;
                    m_lock.unlock();
                    return false;
                };
            };

            //共有モードは、使用する場合はオープンモードと同じでなけれならないため同じでないなら失敗
            //共有モード：読み込みが立っていた場合、READフラグがっていなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //失敗
                _errcode_ = FileSystem::FILEERROR::FERR_OPENSTATE_SHEARINGMODE_MISSMATCH;
                m_lock.unlock();
                return false;
            };
            //続けて、共有モード：書き込みが立っていた場合、WRITEフラグがなければ不成立
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN
                && (_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                //失敗
                _errcode_ = FileSystem::FILEERROR::FERR_OPENSTATE_SHEARINGMODE_MISSMATCH;
                m_lock.unlock();
                return false;
            };

            //一旦クローズ処理
            SonikFileSystemController::FileSystemFunctions::CloseFile((*parambox));
            //非オープン関数へセット
            SLIB_CVR_USING(VOIDFUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX);
            SLIB_CVR_USING(UI64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX);
            SLIB_CVR_USING(I64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX);

            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKTOP]        = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKEND]        = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKPOINT]      = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READ]           = &SonikFileSystemController::FileSystemFunctions::NOP_Read;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE]          = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]     = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]     = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE]       = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE]  = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            ret_uint64_func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize;

            ret_int64_func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet;

            //ここからプラットフォーム別処理===========================
#if defined(_WIN64)
            //ファイルハンドル(最後にメンバ変数に代入)
            HANDLE l_filehandle = nullptr;

            //オープンモードの設定
            DWORD l_openmode = 0;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READ) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_openmode |= GENERIC_READ;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_openmode |= GENERIC_WRITE;
            };

            //共有モードの設定
            DWORD l_sharemode = 0;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_READSHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_sharemode |= FILE_SHARE_READ;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITESHARE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_sharemode |= FILE_SHARE_WRITE;
            };

            //オープンモードの設定
            DWORD l_opencontrol = OPEN_EXISTING;
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CREATE) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                l_opencontrol = OPEN_ALWAYS;
            };
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_CLEAR) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
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
                _errcode_ = FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(GetLastError());
                m_lock.unlock();
                return false;
            };

            parambox->fp = l_filehandle;

#elif defined(__linux__)

#endif
            //オープン状態、クローズ状態関数のセット
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKTOP]       = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Top;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKEND]       = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_End;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKPOINT]     = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_Point;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READ]          = &SonikFileSystemController::FileSystemFunctions::OP_Read;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE]         = &SonikFileSystemController::FileSystemFunctions::OP_Write;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]    = &SonikFileSystemController::FileSystemFunctions::OP_Write_char;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]    = &SonikFileSystemController::FileSystemFunctions::OP_Write_UTF8;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]   = &SonikFileSystemController::FileSystemFunctions::OP_Write_UTF16;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE]      = &SonikFileSystemController::FileSystemFunctions::OP_ReadText_Line;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::OP_ReadText_LineQueue;

            ret_uint64_func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE] = &SonikFileSystemController::FileSystemFunctions::OP_GetFileSize;

            ret_int64_func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET] = &SonikFileSystemController::FileSystemFunctions::OP_SeekPointGet;

            //Writeフラグが立ってなければWrite系は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_WRITE) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE]         = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]   = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;

            };

            //テキストモードフラグが立っていなければWriteChar, ReadText関連は非オープン関数へ
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_OPENTEXT) == FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]    = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]   = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE]      = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
                ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE] = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            };

            //ADDフラグが立っていればファイルシークを最終へ
            //※現状ここでは常にオープン状態となるので直接コールする。
            if ((_open_switch_ & FileSystem::FILEOPENSWITCH::FOSW_ADD) != FileSystem::FILEOPENSWITCH::FOSW_UNKNOWN)
            {
                SonikFileSystemController::FileSystemFunctions::OP_SeekPointSet_End((*parambox));
                _errcode_ = parambox->errorcode;
                if (_errcode_ != FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED)
                {
                    //error
                    return false;
                };

            };

            //終了
            _errcode_ = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            m_lock.unlock();
            return true;
        };

        //FileClose
        void SonikFileSystemController::CloseFile(void)
        {
            m_lock.lock();

            //クローズ処理
            SonikFileSystemController::FileSystemFunctions::CloseFile((*parambox));
            //非オープン関数へセット
            SLIB_CVR_USING(VOIDFUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX);
            SLIB_CVR_USING(UI64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX);
            SLIB_CVR_USING(I64FUNCINDEX, SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX);

            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKTOP]           = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Top;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKEND]           = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_End;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_SEEKPOINT]         = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointSet_Point;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READ]              = &SonikFileSystemController::FileSystemFunctions::NOP_Read;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE]             = &SonikFileSystemController::FileSystemFunctions::NOP_Write;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_CHAR]        = &SonikFileSystemController::FileSystemFunctions::NOP_Write_char;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF8]        = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF8;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_WRITE_UTF16]       = &SonikFileSystemController::FileSystemFunctions::NOP_Write_UTF16;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINE]          = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_Line;
            ret_void_func[VOIDFUNCINDEX::RET_VFI_READLINEQUEUE]     = &SonikFileSystemController::FileSystemFunctions::NOP_ReadText_LineQueue;

            ret_uint64_func[UI64FUNCINDEX::RET_UI64FI_GETFILESIZE]  = &SonikFileSystemController::FileSystemFunctions::NOP_GetFileSize;

            ret_int64_func[I64FUNCINDEX::RET_I64FI_SEEKPOINTGET]    = &SonikFileSystemController::FileSystemFunctions::NOP_SeekPointGet;

            m_lock.unlock();
        };

        //現在のファイルサイズの取得
        uint64_t SonikFileSystemController::GetFileSize(void)
        {
            m_lock.lock();

            uint64_t ret = (*(ret_uint64_func[SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX::RET_UI64FI_GETFILESIZE]))((*parambox));

            m_lock.unlock();

            return ret;
        };
        uint64_t SonikFileSystemController::GetFileSize(SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            uint64_t ret = (*(ret_uint64_func[SonikFileSystemController::FileSystemFunctions::RET_UINT64_FUNC_INDEX::RET_UI64FI_GETFILESIZE]))((*parambox));
            
            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return ret;
        };

        //シーク位置を最初に移動します。
        void SonikFileSystemController::SeekPointSet_Top(void)
        {
            m_lock.lock();

            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKTOP])((*parambox));

            m_lock.unlock();
        };
        void SonikFileSystemController::SeekPointSet_Top(SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKTOP])((*parambox));
            
            _errcode_ = parambox->errorcode;

            m_lock.unlock();


        };
        //シーク位置を最後に移動します。
        void SonikFileSystemController::SeekPointSet_End(void)
        {
            m_lock.lock();

            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKEND])((*parambox));

            m_lock.unlock();

        };
        void SonikFileSystemController::SeekPointSet_End(SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKEND])((*parambox));
            
            _errcode_ = parambox->errorcode;

            m_lock.unlock();
        };

        //現在の読み込みシーク位置から指定バイト数分移動します。
        void SonikFileSystemController::SeekPointSet_Point(int64_t _offset_)
        {
            m_lock.lock();

            parambox->arg1 = _offset_;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKPOINT])((*parambox));

            m_lock.unlock();

        };
        void SonikFileSystemController::SeekPointSet_Point(int64_t _offset_, SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            parambox->arg1 = _offset_;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_SEEKPOINT])((*parambox));
            
            _errcode_ = parambox->errorcode;

            m_lock.unlock();
        };
        //現在のシーク位置を取得します。
        int64_t SonikFileSystemController::SeekPointGet(void)
        {
            m_lock.lock();

            int64_t ret = (*ret_int64_func[SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX::RET_I64FI_SEEKPOINTGET])((*parambox));
            
            m_lock.unlock();

            return ret;
        };
        int64_t SonikFileSystemController::SeekPointGet(SonikLib::FileSystem::FILEERROR& _errcode_)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            int64_t ret = (*ret_int64_func[SonikFileSystemController::FileSystemFunctions::RET_INT64_FUNC_INDEX::RET_I64FI_SEEKPOINTGET])((*parambox));
           
            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return ret;
        };

        //指定したサイズ文読み込みます。
        void SonikFileSystemController::Read(char* _buffer_, uint64_t _size_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->buffer = reinterpret_cast<unsigned char*>(_buffer_);
            parambox->arg1 = _size_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READ])((*parambox));

            m_lock.unlock();

            return;
        };
        void SonikFileSystemController::Read(char* _buffer_, uint64_t _size_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->buffer = reinterpret_cast<unsigned char*>(_buffer_);
            parambox->arg1 = _size_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READ])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        //バイナリデータ書き込み用 4バイト配列をreinterpretで渡している場合はblocksizeはsizeof(uint32_t)等...。
        void SonikFileSystemController::Write(char* _writevalue_, uint64_t _writesize_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->buffer = reinterpret_cast<unsigned char*>(_writevalue_);
            parambox->arg1 = _writesize_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE])((*parambox));

            m_lock.unlock();

            return;
        };
        void SonikFileSystemController::Write(char* _writevalue_, uint64_t _writesize_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->buffer = reinterpret_cast<unsigned char*>(_writevalue_);
            parambox->arg1 = _writesize_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        }

        //SonikStringの吐き出し方法で分けています。
        //テキストモードでオープンした状態だとファイル内の文字のエンコーディングがUTF-8に代わるといったことはありません。
        //バイナリで、追記以外...つまりすべての文字を再出力..となった場合は変換されるかもしれません。
        void SonikFileSystemController::Write_char(SonikLib::SonikStringDefault _writevalue_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_CHAR])((*parambox));

            m_lock.unlock();

            return;

        };
        void SonikFileSystemController::Write_char(SonikLib::SonikStringDefault _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_CHAR])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        void SonikFileSystemController::SonikFileSystemController::Write_UTF8(SonikLib::SonikStringDefault _writevalue_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_UTF8])((*parambox));

            m_lock.unlock();

            return;

        };
        void SonikFileSystemController::SonikFileSystemController::Write_UTF8(SonikLib::SonikStringDefault _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_UTF8])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        void SonikFileSystemController::Write_UTF16(SonikLib::SonikStringDefault _writevalue_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_UTF16])((*parambox));

            m_lock.unlock();

            return;

        };
        void SonikFileSystemController::Write_UTF16(SonikLib::SonikStringDefault _writevalue_, SonikLib::FileSystem::FILEERROR& _errcode_, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_writevalue_;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_WRITE_UTF16])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        //テキストモード専用　指定された行数文TEXTを読み込みます。
        void SonikFileSystemController::ReadText_Line(SonikLib::SonikStringDefault& _str_, uint64_t GetRowCnt, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_str_;
            parambox->arg3 = GetRowCnt;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READLINE])((*parambox));

            m_lock.unlock();

            return;
        };
        void SonikFileSystemController::ReadText_Line(SonikLib::SonikStringDefault& _str_, SonikLib::FileSystem::FILEERROR& _errcode_, uint64_t GetRowCnt, bool isLargeUsed)
        {
            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->str = &_str_;
            parambox->arg3 = GetRowCnt;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READLINE])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        //テキストモード専用　指定された行数文TEXTを読み込ます。改行は削除され、改行で分割されたQueueとして取得します。
        void SonikFileSystemController::ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikStringDefault>& _GetLineQueue_, uint64_t GetRowCnt, bool isLargeUsed)
        {
            SonikLib::SonikStringDefault local_strarea;

            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->queue = &_GetLineQueue_;
            parambox->arg3 = GetRowCnt;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READLINEQUEUE])((*parambox));

            m_lock.unlock();

            return;
        };
        void SonikFileSystemController::ReadText_LineQueue(SonikLib::Container::SonikAtomicQueue<SonikStringDefault>& _GetLineQueue_, SonikLib::FileSystem::FILEERROR& _errcode_, uint64_t GetRowCnt, bool isLargeUsed)
        {
            SonikLib::SonikStringDefault local_strarea;

            m_lock.lock();

            parambox->errorcode = FileSystem::FILEERROR::FERR_NOT_ERROR_PROCESS_SUCCEEDED;
            parambox->queue = &_GetLineQueue_;
            parambox->arg3 = GetRowCnt;
            parambox->isLargeUseOk = isLargeUsed;
            (*ret_void_func[SonikFileSystemController::FileSystemFunctions::RET_VOID_FUNC_INDEX::RET_VFI_READLINEQUEUE])((*parambox));

            _errcode_ = parambox->errorcode;

            m_lock.unlock();

            return;
        };

        //エラコードenum変換用グローバル関数
        //SUCCEEDED は返却しません。
#if defined(_WIN64) || defined (_WIN32)
        //Windows Implemets
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR FileSystem::FILEERROR FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(DWORD _errorcode_) SLIB_CVR_NOEXCEPT
        {
            switch (_errorcode_)
            {
            case ERROR_FILE_NOT_FOUND:
                return FileSystem::FILEERROR::FERR_FILE_NOT_FOUND;
                break;

            case ERROR_PATH_NOT_FOUND:
                return FileSystem::FILEERROR::FERR_PATH_NOT_FOUND;
                break;

            case ERROR_ACCESS_DENIED:
                return FileSystem::FILEERROR::FERR_ACCESS_DENIED;
                break;

            case ERROR_SHARING_VIOLATION:
                return FileSystem::FILEERROR::FERR_SHARING_BLOCKED;
                break;

            case ERROR_INVALID_HANDLE:
                return FileSystem::FILEERROR::FERR_FILEDISCRIPT_FOUND;
                break;

            case ERROR_NOT_SUPPORTED:
                return FileSystem::FILEERROR::FERR_SEEK_NOTSUPPORTED;
                break;

            case ERROR_INVALID_FUNCTION:
                return FileSystem::FILEERROR::FERR_SEEK_NOTSUPPORTED;
                break;

            case ERROR_HANDLE_EOF:
                return FileSystem::FILEERROR::FERR_READ_EOF;
                break;

            case ERROR_NEGATIVE_SEEK:
                return FileSystem::FILEERROR::FERR_SEEK_NEGATIVEVALUE;
                break;

            case ERROR_GEN_FAILURE:
                return FileSystem::FILEERROR::FERR_GEN_FAILURE;
                break;

            case ERROR_IO_DEVICE:
                return FileSystem::FILEERROR::FERR_IO_DEVICE;
                break;

            case ERROR_DISK_FULL:
                return FileSystem::FILEERROR::FERR_DISK_FULL;
                break;

            case ERROR_NOT_ENOUGH_QUOTA:
                return FileSystem::FILEERROR::FERR_QUOTA_EXCESS;
                break;

            case ERROR_OUTOFMEMORY:
                return FileSystem::FILEERROR::FERR_OUTOFMEMORY;
                break;

            default:
                return FileSystem::FILEERROR::FERR_EXCEPTION;
                break;
            };
        };
#else
        //Linux, Unix, Apple Implemets
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR FileSystem::FILEERROR FILEERROR_ENUM_CONVERT_APIERROR_TO_FERR(uint64_t _errorcode_) SLIB_CVR_NOEXCEPT
        {
            //環境がないため未実装
        };
#endif

    }; // end namespace FileSystemControllers
}; // end namespace SonikLib