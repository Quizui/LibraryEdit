
#ifndef __SONIKLIB_MEMORYLIBRARY_TLSFMEMORY_ALLOCATOR_H__
#define __SONIKLIB_MEMORYLIBRARY_TLSFMEMORY_ALLOCATOR_H__

#include <stdint.h>

#include "./AllocateInterface.h"
#include "../SonikCAS/SonikAtomicLock.h"
#include "../SmartPointer/SonikSmartPointer.hpp"

namespace std
{
#if defined(__GNUC__) || defined(__clang__)
	//clang, GCC環境ではinline namespace __1 内に定義されている。
	//つけるとあいまいエラーが発生してコンパイルﾃﾞｷﾅｲﾖ
	inline namespace __1
	{
		template <class _Ty>
		struct atomic;
	};

#elif defined(_MSC_VER)
	//MSVCではclangやGCC環境みたいに__1などのバージョン管理は行っていないらしい。
	//そのためinline namespace __1 が不要
	//つけるとあいまいエラーが発生してコンパイルﾃﾞｷﾅｲﾖ
	template <class _Ty>
	struct atomic;

#endif

};

namespace SonikLib
{

	//分割数は2^5 固定とする。
	class TLSFAllocator : public SLibAllocateInterface
	{

	private:
		uint8_t* mp_main_memblock;		//メインメモリブロックの先頭
		uint8_t* mp_freelistblock;		//(フリーリストビット列 + フリーリスト実領域)全体の先頭
		uint8_t* mp_FreelistAreaTop;	//フリーリスト実領域の先頭
		SonikLib::S_CAS::SonikAtomicLock* m_lock; //マルチスレッドロック
		uint32_t mem_block_size;


	private:

		//InnerFunctions

		//スマートポインタ式に変更したためこちらに移動。しかし一応処理は残しておく(戻せる様になったときにまた実装はめんどくさいので)
		TLSFAllocator(void) noexcept;

		TLSFAllocator(const TLSFAllocator& _copy_) noexcept;
		TLSFAllocator(TLSFAllocator&& _move_) noexcept;
		TLSFAllocator& operator =(const TLSFAllocator& _copy_) noexcept;
		TLSFAllocator& operator =(TLSFAllocator&& _move_) noexcept;

	public:

		~TLSFAllocator(void);

		//アロケータの有効化設定を行います。
		//失敗の場合はサイズ調整の上で再度実施してみてください。
		static SLibAllocEnums::EnableRet CreateAllocator(SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>& _out_, uint32_t _CreateMemorySize_);

		void* memal(size_t _size_) noexcept override;
		void* memal(size_t _size_, SLibAllocEnums::EnableRet& _errcode_) noexcept override;//エラーコード出力バージョン
		void* memal_Exception(size_t _size_) override; //例外送出版
		void* memalArray(size_t _sizeof_, size_t _elemcnt_ = 1) noexcept override;
		void* memalArray(size_t _sizeof_, size_t _elemcnt_, SLibAllocEnums::EnableRet& _errcode_) noexcept override;//エラーコード出力バージョン
		void* memalArray_Exception(size_t _sizeof_, size_t _elemcnt_ = 1) override; //例外送出版
		void memdel(void* _pfree_) noexcept override;
		void memdelArray(void* _pfree_) noexcept override;

	};


};




#endif //__SONIKLIB_MEMORYLIBRARY_TLSFMEMORY_ALLOCATOR_H__