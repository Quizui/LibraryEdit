#ifndef SONIKSTRING_RANGEDFORCONTAINER_HPP_
#define SONIKSTRING_RANGEDFORCONTAINER_HPP_

 //拡張式配列の機能を提供します。
 //配列サイズは拡張方向のみで、縮小はしません。
 //つまりpushbackにより１つ拡張された場合は拡張されたままになります。
#include <SmartPointer/SonikSmartPointer.hpp>
#include <SonikCAS/SonikAtomicLock.h>
#include <CPPGrammarDefines.h>

#include <cstdint>
#include <new>
#include <algorithm>

namespace SonikLib
{
	namespace Container
	{
		template <class T>
		class SonikVariableArrayContainer
		{
		public:
			//コンテナオブジェクト操作用のイテレータ
			//ローカルでの使用を想定。イテレータ取得後にオブジェクトの配列がreserveされたり操作が入ると無効(タングリング)が発生します。
			//これは一般的にはstdと同様の条件です。
			class VACIterator
			{
			private:
				T* TopPointer;
				T* ControlPointer;

			public:
				DEF_FORCE_INLINE VACIterator(T* _pointer_ = nullptr) SLIB_CVR_NOEXCEPT
					:TopPointer(_pointer_)
					, ControlPointer(_pointer_)
				{
					//no process
				};

				DEF_FORCE_INLINE VACIterator(const VACIterator& _copy_) SLIB_CVR_NOEXCEPT
					:TopPointer(_copy_.TopPointer)
					, ControlPointer(_copy_.ControlPointer)
				{
					//no process
				};

				DEF_FORCE_INLINE VACIterator(VACIterator&& _move_) SLIB_CVR_NOEXCEPT
				{
					TopPointer = std::move(_move_.TopPointer);
					ControlPointer = std::move(_move_.ControlPointer);

					_move_.TopPointer = nullptr;
					_move_.ControlPointer = nullptr;
				};

				DEF_FORCE_INLINE VACIterator& operator ++(void) SLIB_CVR_NOEXCEPT
				{
					++ControlPointer;
					return (*this);
				};

				DEF_FORCE_INLINE VACIterator& operator ++(int) SLIB_CVR_NOEXCEPT
				{
					VACIterator tmp = (*this);

					++ControlPointer;
					return tmp;
				};

				DEF_FORCE_INLINE VACIterator& operator --(void) SLIB_CVR_NOEXCEPT
				{
					--ControlPointer;
					return (*this);
				};

				DEF_FORCE_INLINE VACIterator& operator --(int) SLIB_CVR_NOEXCEPT
				{

					VACIterator tmp = (*this);

					--ControlPointer;
					return tmp;
				};

				DEF_FORCE_INLINE bool operator ==(const VACIterator& _compare_) const SLIB_CVR_NOEXCEPT
				{
					return ControlPointer == _compare_.ControlPointer;
				};

				DEF_FORCE_INLINE bool operator !=(const VACIterator& _compare_) const SLIB_CVR_NOEXCEPT
				{
					return ControlPointer != _compare_.ControlPointer;
				};

				DEF_FORCE_INLINE const T& operator[](uint64_t _index_) const
				{
					return TopPointer[_index_];
				};

				DEF_FORCE_INLINE T& operator[](uint64_t _index_)
				{
					return TopPointer[_index_];
				};

				DEF_FORCE_INLINE const T& operator*(void) const
				{
					return (*ControlPointer);
				};

				DEF_FORCE_INLINE T& operator*(void)
				{
					return (*ControlPointer);
				};

				DEF_FORCE_INLINE const T* operator ->(void) const SLIB_CVR_NOEXCEPT
				{
					return ControlPointer;
				};

				DEF_FORCE_INLINE T* operator ->(void) SLIB_CVR_NOEXCEPT
				{
					return ControlPointer;
				};
			};

		private:
			T* AllocAreaPtr;
			uint64_t AllocCount;
			uint64_t MaxCnt;

			SonikLib::S_CAS::SonikAtomicLock m_lock;
			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

		private:
			SonikVariableArrayContainer(void)
			:AllocAreaPtr(nullptr)
			, AllocCount(0)
			, MaxCnt(0)
			{
				// no process;
			};
			SonikVariableArrayContainer(T* _ptr_, uint64_t _maxcnt_)
			:AllocAreaPtr(_ptr_)
			, AllocCount(0)
			, MaxCnt(_maxcnt_)
			{
				// no process;
			};

#if defined(__cplusplus) && __cplusplus >= 201103L //C++ 11 以上
			//コピーと代入の禁止
			SonikVariableArrayContainer(const SonikVariableArrayContainer& _copy_) = delete;
			SonikVariableArrayContainer(SonikVariableArrayContainer&& _move_) = delete;
			SonikVariableArrayContainer& operator =(const SonikVariableArrayContainer& _copy_) = delete;
			SonikVariableArrayContainer& operator =(SonikVariableArrayContainer&& _move_) = delete;

#else //C++ 11 以下
			//コピーと代入の禁止
			SonikVariableArrayContainer(const SonikVariableArrayContainer& _copy_);
			SonikVariableArrayContainer& operator =(const SonikVariableArrayContainer& _copy_);

	#if defined(SLIB_COMPILER_DEF_MSVC) && _MSC_VER >= 1600
			//MSVC2010ならmove可能なので定義だけしておく。
			SonikVariableArrayContainer(SonikVariableArrayContainer&& _move_);
			SonikVariableArrayContainer& operator =(SonikVariableArrayContainer&& _move_);

	#endif
#endif

			//リザーブ
			DEF_FORCE_INLINE bool __RESERVE__(void) SLIB_CVR_NOEXCEPT
			{
				//新しいサイズを計算
				//現在のMaxSizeから1.5倍する。
				uint64_t l_new_size = MaxCnt + (MaxCnt >> 1);

				//1.5倍したサイズが現在のAlllocCountより少なければAllocCountの数に合わせる
				if (l_new_size < AllocCount)
				{
					l_new_size = AllocCount;
				};

				//別に極端に小さい値を拒んでいるわけではないので、初期値として64等は設定しない。
				//現状PushBackからしかコールされず、必ずAllocCountが1以上になるためl_new_sizeは0にならない。
				//今後RESERVEを直接コールできるようになった場合はコール元で0を弾く。

				void* l_allocbuffer = m_allocator->memal((sizeof(T) * l_new_size));
				if (l_allocbuffer == nullptr)
				{
					return false;
				};

				int8_t* l_tmparea = new(l_allocbuffer) int8_t[(sizeof(T) * l_new_size)]{};

				T* l_new_area = reinterpret_cast<T*>(l_tmparea);
				T* old_area = AllocAreaPtr;

				for (uint64_t i = 0; i < AllocCount; ++i)
				{
					new(&l_new_area[i]) T(SLIB_CVR_STDMOVE(old_area[i]));
					old_area[i].~T(); //デストラクタコール
				};

				m_allocator->memdel(AllocAreaPtr);
				AllocAreaPtr = reinterpret_cast<T*>(l_tmparea);
				MaxCnt = l_new_size;

				return true;
			};

			//リサイズ
			DEF_FORCE_INLINE bool __RESIZE__(uint64_t _targetsize_) SLIB_CVR_NOEXCEPT
			{
				//容量拡張が必要かチェック
    			if (_targetsize_ > MaxCnt)
    			{
        			// ターゲットサイズが現在の容量を超えている場合、まず容量をリザーブ（拡張）する。
        			// リザーブにはAllocCountの値が関係するため、一時的にターゲットサイズに合わせておく
        			// (PushBackの動作を模倣し、RESERVEに論理サイズを教える)
        			uint64_t l_old_count = AllocCount;
        			AllocCount = _targetsize_;
        
        			if (!__RESERVE__())
        			{
            			// RESERVE失敗時はAllocCountを元に戻して終了
            			AllocCount = l_old_count;
            			return false;
        			};
        			// RESERVE成功時は、MaxCntが新しいサイズに更新されている
    			};

				//要素数の増減処理
    			if (_targetsize_ < AllocCount)
    			{
        			//要素数を減らす場合 (縮小)
        			//縮小される要素のデストラクタを明示的に呼び出す。
        			for (uint64_t i = _targetsize_; i < AllocCount; ++i)
        			{
            			AllocAreaPtr[i].~T();
        			};
        			//論理サイズを更新
        			AllocCount = _targetsize_;

    			}else if (_targetsize_ > AllocCount)
    			{		
        			//要素数を増やす場合 (拡張)
        			//増える分の要素を配置 new でデフォルト構築する。
        			for (uint64_t i = AllocCount; i < _targetsize_; ++i)
        			{
            			//Tがデフォルト構築可能であることを要求する（std::vectorと同じ）
            			new(&AllocAreaPtr[i]) T(); 
        			};
        			//論理サイズを更新
        			AllocCount = _targetsize_;
    			};

    			return true;
			};

		public:
			DEF_FORCE_INLINE static bool CreateContainer(SonikLib::SharedSmtPtr<SonikVariableArrayContainer<T>>& _out_smtptr_, int32_t _ElemCount_ = 64)
			{
				if (_ElemCount_ == 0)
				{
					return false;
				};

				SonikLib::SLibAllocateInterface* l_allocator_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
				if (l_allocator_obj == nullptr)
				{
					return false;
				};

				void* allocbuffer = l_allocator_obj->memal((sizeof(T) * _ElemCount_));
				if (allocbuffer == nullptr)
				{
					delete l_allocator_obj;
					return false;
				};

				int8_t* l_tmp = new(allocbuffer) int8_t[(sizeof(T) * _ElemCount_)]{};

				allocbuffer = l_allocator_obj->memal(sizeof(SonikVariableArrayContainer<T>));
				if (allocbuffer == nullptr)
				{
					delete l_allocator_obj;
					return false;
				};

				SonikVariableArrayContainer<T>* l_vacobj = new(allocbuffer) SonikVariableArrayContainer<T>;

				if(!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_allocator_obj, l_vacobj->m_allocator))
				{
					l_vacobj->~SonikVariableArrayContainer();
					delete l_allocator_obj;

					return false;
				};

				l_vacobj->AllocAreaPtr = reinterpret_cast<T*>(l_tmp);
				l_vacobj->MaxCnt = _ElemCount_;

				if (!SonikLib::SharedSmtPtr<SonikVariableArrayContainer<T>>::SmartPointerCreate(l_vacobj, _out_smtptr_))
				{
					l_vacobj->~SonikVariableArrayContainer();
					delete l_allocator_obj;

					return false;
				};

				return true;

			};
			DEF_FORCE_INLINE static bool CreateContainer(SonikLib::SharedSmtPtr<SonikVariableArrayContainer<T>>& _out_smtptr_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, int32_t _ElemCount_ = 64)
			{
				if (_ElemCount_ == 0)
				{
					return false;
				};

				void* allocbuffer = _allocator_->memal((sizeof(T) * _ElemCount_));
				if (allocbuffer == nullptr)
				{
					return false;
				};

				int8_t* l_tmp = new(allocbuffer) int8_t[(sizeof(T) * _ElemCount_)]{};

				allocbuffer = _allocator_->memal(sizeof(SonikVariableArrayContainer<T>));
				if (allocbuffer == nullptr)
				{
					_allocator_->memdel(l_tmp);
					return false;
				};

				SonikVariableArrayContainer<T>* l_vacobj = new(allocbuffer) SonikVariableArrayContainer<T>;

				l_vacobj->m_allocator = _allocator_;
				l_vacobj->AllocAreaPtr = reinterpret_cast<T*>(l_tmp);
				l_vacobj->MaxCnt = _ElemCount_;

				if (!SonikLib::SharedSmtPtr<SonikVariableArrayContainer<T>>::SmartPointerCreate(l_vacobj, _out_smtptr_, _allocator_))
				{
					l_vacobj->~SonikVariableArrayContainer();
					_allocator_->memdel(l_vacobj);

					return false;
				};

				return true;

			};

			DEF_FORCE_INLINE ~SonikVariableArrayContainer(void)
			{
				if (AllocAreaPtr != nullptr)
				{
					for (uint64_t i = 0; i < AllocCount; ++i)
					{
						AllocAreaPtr[i].~T();
					};

					m_allocator->memdel(AllocAreaPtr);
					AllocAreaPtr = nullptr;
				};

			};

			DEF_FORCE_INLINE VACIterator begin(void) SLIB_CVR_NOEXCEPT
			{
				return VACIterator(AllocAreaPtr);
			};

			DEF_FORCE_INLINE VACIterator back(void) SLIB_CVR_NOEXCEPT
			{
				return (AllocCount > 0) ? VACIterator(&AllocAreaPtr[(AllocCount - 1)])
					: VACIterator(AllocAreaPtr);
			};

			DEF_FORCE_INLINE VACIterator end(void) SLIB_CVR_NOEXCEPT
			{
				return VACIterator(&AllocAreaPtr[AllocCount]);
			};

			DEF_FORCE_INLINE VACIterator rend(void) SLIB_CVR_NOEXCEPT
			{
				return VACIterator(&(AllocAreaPtr - 1));
			};

			DEF_FORCE_INLINE uint64_t GetAllocCount(void) const SLIB_CVR_NOEXCEPT
			{
				return AllocCount;
			};

			DEF_FORCE_INLINE uint64_t GetSizeMax(void) const SLIB_CVR_NOEXCEPT
			{
				return MaxCnt;
			};

			DEF_FORCE_INLINE bool PushBack(const T& PushItem) SLIB_CVR_NOEXCEPT
			{
				m_lock.lock();

				++AllocCount;

				if (MaxCnt < AllocCount)
				{
					if (!__RESERVE__())
					{
						--AllocCount;
						m_lock.unlock();
						return false;
					};
				};

				T* lp_placement = &AllocAreaPtr[(AllocCount - 1)];

				new(lp_placement) T(PushItem);

				m_lock.unlock();
				return true;
			};

			DEF_FORCE_INLINE bool PopBack(void) SLIB_CVR_NOEXCEPT
			{
				m_lock.lock();

				if (AllocCount == 0)
				{
					m_lock.unlock();
					return true;
				};

				AllocAreaPtr[AllocCount - 1].~T();

				--AllocCount;

				m_lock.unlock();
				return true;
			};

			DEF_FORCE_INLINE bool ReSize(uint64_t _resize_size_) SLIB_CVR_NOEXCEPT
			{
				m_lock.lock();

				bool ret = false;

				ret =  __RESIZE__(_resize_size_);

				m_lock.unlock();

				return ret;
			};

			DEF_FORCE_INLINE void Clear(void) SLIB_CVR_NOEXCEPT
			{
				m_lock.lock();

				if (AllocCount == 0)
				{
					m_lock.unlock();
					return;
				};

				for (uint64_t i = 0; i < AllocCount; ++i)
				{
					AllocAreaPtr[i].~T();
				};

				AllocCount = 0;

				m_lock.unlock();
				return;
			};

			//copy実装
			//引数のアイテム情報を自身へコピーします。
			//自身のAllocSizeが引数コンテナのAllocSizeより下ならReserveしてコピーします。
			//拡張によるメモリ的なエラーが発生した場合falseを返却します。基本的にはtrueが返ります。
			//スマートポインタでカウンタ参照管理が入るので参照せずコピーを使う(constはスマートポインタ経由で値が変えられてしまうのでつける)
			DEF_PRE_NO_DISCARD DEF_FORCE_INLINE bool Copy(const SonikLib::SharedSmtPtr<SonikVariableArrayContainer<T>> _copy_) DEF_POST_NO_DISCARD
			{
				//サイズチェック
				if(AllocCount < _copy_->AllocCount)
				{
					//RESIZEする。
					if(!__RESIZE__(_copy_->AllocCount))
					{
						return false;
					};
				};

				//ループコピー
				//この関数を抜けるまではスマートポインタのおかげでポインタ先が消えることはないので
				//生ポインタを取得して、->オーバーロードアクセス演算子のオーバーヘッドを減らす。
				SonikVariableArrayContainer<T>* l_pointer = _copy_.GetPointer();
				uint64_t l_copysize = l_pointer->AllocCount;
				T* l_ary = l_pointer->AllocAreaPtr;
				for(uint64_t i = 0; i < l_copysize; ++i)
				{
					AllocAreaPtr[i] = l_ary[i];
				};

				return true;
			};

			DEF_FORCE_INLINE const T& operator[](uint64_t _index_) const
			{
				return AllocAreaPtr[_index_];
			};

			DEF_FORCE_INLINE T& operator[](uint64_t _index_)
			{
				return AllocAreaPtr[_index_];
			};

		}; //end class

	}; //end namespace Container

}; //end namespace SonikLib

#endif /* SONIKSTRING_RANGEDFORCONTAINER_HPP_ */