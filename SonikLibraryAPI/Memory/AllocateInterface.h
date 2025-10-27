#ifndef __SONIKLIB_ALLOCATE_TRAITS_HEADER__
#define __SONIKLIB_ALLOCATE_TRAITS_HEADER__

#include <new>
#include <type_traits>
#include "../CompilersPreProcesser.h"

namespace SonikLib
{
	namespace SLibAllocEnums
	{
		enum class EnableRet
		{
			ENABLED_OK = 0,	//処理成功

			HEEPALLOCERR_CREATEFILED,	//アロケータクリエイトが失敗(アロケータオブジェクトの空作成(new)が失敗)
			HEEPALLOCERR_MAINBLOCK,		//メインブロックのヒープ領域獲得時にエラーが発生しました。
			HEEPALLOCERR_FREEBLOCK,		//フリーリスト管理用のヒープ領域獲得時にエラーが発生しました。
			HEEPALLOCERR_MTBLOCK,		//マルチスレッドブロックオブジェクトのヒープ領域獲得時にエラーが発生しました。
//			HEEPALLOCERR_REFCNT,		//参照カウンタオブジェクトのヒープ領域獲得時にエラーが発生しました。(2024-12-27 継承先をスマートポインタ形式にしたため不要とした。でも復活するかもしれないからコメントアウトで対処
			HEEPALLOCERR_OBJECTSMTPTR,	//最終スマートポインタ生成処理に失敗しました。(一応atomic<unsigned int>型の newを１回行います..)

			MEM_AL_ERR_OVERSIZE,		//memal、memalarray関数において、貸し出せるメモリ容量が無いため関数は失敗として終了しました。(戻り値はnullptrで返ります。)
			MEM_AL_ERR_TARGETSIZE_ZERO,	//memalで指定された確保領域要求サイズが0です。
			MEM_AL_ERR_UNEXCEPTED,		//memalで想定していない何等かのエラーが発生しています。実装のパターン漏れかもしれません。

			ENABLE_FAILED,				//Enableの状態がOK以外かつ、本アロケータが使用不可能な状態の時。
			ENABLE_DEFAULT = 255,		//Enabled初期状態(生成関数が通されていない初期状態。)
		};
	};

	//アロケータインターフェース
	class SLibAllocateInterface
	{
	protected:
		SLibAllocEnums::EnableRet m_enabled_state;		//有効状態

	private:
		//自分のとこのポインタかチェックします。
		DEF_FORCE_INLINE virtual int8_t __INNER_IS_ADDR__(void* _checkpointer_)
		{
			return 0; //デフォルトの証
		};
		//配列サイズを確認します。
		DEF_FORCE_INLINE virtual uint32_t __INNER_ARRAYSIZECHECK__(void* _checkpointer_)
		{
			return 0; //デフォルトの証
		};

	protected:
		virtual void __vfunc_memdel__(void* _pfree_)
		{
			::operator delete(_pfree_);
		};
		
		virtual void __vfung_memdelarray__(void* _pfree_)
		{
			::operator delete[](_pfree_);
		};

	public:
		DEF_FORCE_INLINE SLibAllocateInterface(void) noexcept
			:m_enabled_state(SLibAllocEnums::EnableRet::ENABLE_DEFAULT)
		{
			//no porcess;
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE SLibAllocateInterface(const SLibAllocateInterface& _copy_) noexcept
			:m_enabled_state(SLibAllocEnums::EnableRet::ENABLE_DEFAULT)
		{
			//no process;
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE SLibAllocateInterface(SLibAllocateInterface&& _move_) noexcept
			:m_enabled_state(SLibAllocEnums::EnableRet::ENABLE_DEFAULT)
		{
			//no process;
		};

		//デストラクタ
		DEF_FORCE_INLINE virtual ~SLibAllocateInterface(void)
		{
			//no process
		};

		//operator = copy
		DEF_FORCE_INLINE SLibAllocateInterface& operator =(const SLibAllocateInterface& _copy_) noexcept
		{
			return (*this);
		};

		//operator = move
		DEF_FORCE_INLINE SLibAllocateInterface& operator =(SLibAllocateInterface& _move_) noexcept
		{
			return (*this);
		};

		//現在の有効状態を取得します。
		DEF_FORCE_INLINE SLibAllocEnums::EnableRet GetNowEnabledState(void) noexcept
		{
			return m_enabled_state;
		};

		//対象のデストラクタをコールします。
		//管理管轄外のポインタが指定された場合で、カスタムアロケータの場合は継承先の動作によって変わります。
		template <class Type>
		void CallDestructor(Type* _Target_)
		{
			static_assert(!std::is_pointer_v<Type>, "Please Used NoPointerType | ポインタ型は許容していません。内部でType* とするため指定自体はオブジェクト型としてください。");

			_Target_->~Type();
		};

		template <class Type>
		void CallDestructor_Array(Type* _Target_Top_, uint32_t _elemcnt_)
		{
			static_assert(!std::is_pointer_v<Type>, "Please Used NoPointerType | ポインタ型は許容していません。内部でType* とするため指定自体はオブジェクト型としてください。");

			//回数分デストラクタをコール
			for (uint32_t i = 0; i < _elemcnt_; ++i)
			{
				_Target_Top_[i].~Type();
			}

			return;

		};

		DEF_FORCE_INLINE virtual void* memal(size_t _size_) noexcept
		{
			return ::operator new(_size_, std::nothrow);
		};
		DEF_FORCE_INLINE virtual void* memal(size_t _size_, SLibAllocEnums::EnableRet& _errcode_) noexcept//エラーコード出力バージョン
		{
			void* ret = ::operator new(_size_, std::nothrow);
			_errcode_ = (ret == nullptr) ? SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE : SLibAllocEnums::EnableRet::ENABLED_OK;

			return ret;
		};
		DEF_FORCE_INLINE virtual void* memal_Exception(size_t _size_)//例外throw
		{
			return ::operator new(_size_);
		};

		DEF_FORCE_INLINE virtual void* memalArray(size_t _size_, size_t _elem_) noexcept
		{
			return ::operator new[](_size_* _elem_, std::nothrow);
		};
		DEF_FORCE_INLINE virtual void* memalArray(size_t _size_, size_t _elem_, SLibAllocEnums::EnableRet& _errcode_) noexcept//エラーコード出力バージョン
		{
			void* ret = ::operator new[](_size_* _elem_, std::nothrow);
			_errcode_ = (ret == nullptr) ? SLibAllocEnums::EnableRet::MEM_AL_ERR_OVERSIZE : SLibAllocEnums::EnableRet::ENABLED_OK;

			return ret;
		}
		DEF_FORCE_INLINE virtual void* memalArray_Exception(size_t _size_, size_t _elem_)//例外throw
		{
			return ::operator new[](_size_* _elem_);
		};

		template <class Type>
		DEF_FORCE_INLINE void memdel(Type* _del_)
		{
			if(_del_ == nullptr)
			{
				return;
			};

			_del_->~Type();
			__vfunc_memdel__(_del_);
		};
		template<>
		DEF_FORCE_INLINE void memdel<void>(void* _del_)
		{
			if(_del_ == nullptr)
			{
				return;
			};

			__vfunc_memdel__(_del_);
		};

		template <class Type>
		DEF_FORCE_INLINE void memdelArray(Type* _del_, uint32_t index_size)
		{
			if(_del_ == nullptr)
			{
				return;
			};

			//回数分デストラクタをコール
			for (uint32_t i = 0; i < index_size; ++i)
			{
				_del_[i].~Type();
			}

			 __vfung_memdelarray__(_del_);
		};
	};

};

#endif //#define __SONIKLIB_ALLOCATE_TRAITS_HEADER__