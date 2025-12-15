
#ifndef SMARTPOINTER_SONIK_SMARTPOINTER_HPP_
#define SMARTPOINTER_SONIK_SMARTPOINTER_HPP_

#include <cstdint>
#include <new>
//#include <atomic>
#include <type_traits>
#include <utility>

#include <CompilersPreProcesser.h>
#include <Memory/AllocateInterface.h>
#include <SonikCAS/SonikAtomic.hpp>
#include <CPPGrammarDefines.h>

//MSVC2010とC++バージョン判定による可変長テンプレート判定に使う
//デフォ0(無効化)
#define SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_VISUALSTUDIO_ENABLE 0
#define SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_CPP_ENABLE 0
#if defined(_MSC_VER)
	#if _MSC_VER >= 1800
		//2013以上なら値1にする(有効化)
		#undef SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_VISUALSTUDIO_ENABLE
		#define SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_VISUALSTUDIO_ENABLE 1
	#endif
#endif

#if defined(__cplusplus)
	#if __cplusplus >= 201103L
		//C++11以上なら値1にする(有効化
		#undef SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_CPP_ENABLE
		#define SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_CPP_ENABLE 1
	#endif
#endif


//スマートポインタクラスでっす

 //前方宣言
namespace SonikLib
{
	template <class pType, class Enable = void>
	class SharedSmtPtr;
	template <class pType, class Enable>
	class SharedSmtPtr<pType[], Enable>;
	template <class pType, class Enable = void>
	class UniqueSmtPtr;
	template <class pType, class Enable>
	class UniqueSmtPtr<pType[], Enable>;
	template <class AllocatorClassType> //カスタムアロケータ用スマートポインタ
	class AllocatorSharedSmtPtr;

	//SharedSmtPtrのdynamic_castをサポートします。
	//失敗した場合はfalseを返却します。
	template <class before, class after>
	bool SharedCast_Dynamic(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

	template <class before, class after>
	bool SharedCast_Dynamic(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

	//SharedSmtPtrのreinterpret_castをサポートします。
	//ただし、普通にreinterpret_castをするだけです。通常のポインタをreinterpret_castをするのと違いはありません。
	//そのため、その先の例外処理、エラー処理等は使用者の責任になります。
	template <class before, class after>
	void SharedCast_Reinterpret(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

	template <class before, class after>
	void SharedCast_Reinterpret(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

	//UniqueSmtPtrのdynamic_castをサポートします。
	//失敗した場合はfalseを返却し、所有権を移譲せず終了します。
	template <class before, class after>
	bool UniqueCast_Dynamic(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

	template <class before, class after>
	bool UniqueCast_Dynamic(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

	//UniqueSmtPtrのreinterpret_castをサポートします。
	//ただし、普通にreinterpret_castをするだけです。通常のポインタをreinterpret_castをするのと違いはありません。
	//そのため、その先の例外処理、エラー処理等は使用者の責任になります。
	//また、SharedSmtPtrと違い、唯一性保持のため所有権を問答無用で変換先(_dst_側)へ移譲し、_src_側はnullptr化されます。
	//_src_へのsegment faultにご注意ください。
	template <class before, class after>
	void UniqueCast_Reinterpret(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

	template <class before, class after>
	void UniqueCast_Reinterpret(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

	//UniqueSmtPtrからSharedSmtPtrへの変換をサポートします。型は変わりません。
	//片方向のみサポートで、sharedからuniqueへは対応しません。というか厳しく禁じるために許可しません。
	template <class PtrTemplateValueType>
	void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_);

	template <class PtrTemplateValueType>
	void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_);

};

namespace SonikLib
{

	//AllocatorSmtPtr===========================================================================================================================================================================
	//カスタムアロケータ用のSharedスマートポインタ。
	//型束縛はしていないので、ぶっちゃけ他のクラスも入るが、↑のスマートポインタ群で使ってるから下手なクラス入れた場合の動作はｼﾗﾅｲﾖ。
	//基本機能は通常のスマートポインタだが、一部不要と思われる処理は削除
	template <class AllocatorClassType>
	class AllocatorSharedSmtPtr
	{
	private:
		//↓ポインタ型は許容していません。内部でAllocatorClassType* とするため指定自体はオブジェクト型としてください。
		static_assert(!SLIB_CVRT_IS_POINTER(AllocatorClassType), "Please Used NoPointerType");

		//フレンドクラス, typename std::enable_if<std::is_arithmetic<pType>::value>::type
		//Shared
		template <class pType, class Enable>
		friend class SharedSmtPtr;
		//Unique
		template <class pType, class Enable>
		friend class UniqueSmtPtr;

	private:
		AllocatorClassType* m_Pointer;
		//std::atomic<unsigned int>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		SonikLib::SonikAtomic<uint32_t>* m_Count;

	private:
		DEF_FORCE_INLINE void AddRef(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			//{
				//no process
			//};
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt + 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


		};

		DEF_FORCE_INLINE void Release(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			if (TmpCnt == 1)
			{
				delete m_Pointer;
				delete m_Count;

				m_Pointer = nullptr;
				m_Count = nullptr;
				return;
			}

			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt - 1, SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


			if (TmpCnt == 1)
			{
				delete m_Pointer;
				delete m_Count;
			};

			m_Pointer = nullptr;
			m_Count = nullptr;
		};

	public:

		//constructor
		DEF_FORCE_INLINE AllocatorSharedSmtPtr(void)
			:m_Pointer(nullptr)
			, m_Count(nullptr)
		{
			//initialize only
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE AllocatorSharedSmtPtr(const AllocatorSharedSmtPtr<AllocatorClassType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
		{
			AddRef();
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE AllocatorSharedSmtPtr(AllocatorSharedSmtPtr<AllocatorClassType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
		{
			_SmtPtr_.m_Pointer = nullptr;
			_SmtPtr_.m_Count = nullptr;
		};

		//Destructor
		DEF_FORCE_INLINE ~AllocatorSharedSmtPtr(void) SLIB_CVR_NOEXCEPT
		{
			Release();
		};

		//operator =
		DEF_FORCE_INLINE AllocatorSharedSmtPtr<AllocatorClassType>& operator =(const AllocatorSharedSmtPtr<AllocatorClassType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				AllocatorSharedSmtPtr<AllocatorClassType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;

				AddRef();
			};

			return (*this);
		};

		//MoveEqual
		DEF_FORCE_INLINE AllocatorSharedSmtPtr<AllocatorClassType>& operator =(AllocatorSharedSmtPtr<AllocatorClassType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				AllocatorSharedSmtPtr<AllocatorClassType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;

				_SmtPtr_.m_Pointer = nullptr;
				_SmtPtr_.m_Count = nullptr;
				/*_SmtPtr_.m_Allocate はオブジェクトなのでここでの初期化は必要ない*/

				//moveなのでAddref()をコールする必要はない。
			};

			return (*this);
		};

		DEF_FORCE_INLINE bool operator ==(const AllocatorSharedSmtPtr<AllocatorClassType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == _SmtPtr_.m_Pointer) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(SLIB_COMPILER_DEF_GCC) || defined(SLIB_COMPILER_DEF_CLANG)
	//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
	// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE AllocatorClassType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter. \nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it. \n")))

#elif defined(SLIB_COMPILER_DEF_MSVC) 
	//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
	// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE AllocatorClassType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif

#else
		DEF_FORCE_INLINE AllocatorClassType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif	
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE AllocatorClassType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			AllocatorClassType* l_pointer = m_Pointer;

			m_Pointer = nullptr;
			Release(); //m_Pointer に nullptr を入れた上でdeleteコールしてるため参照カウンタが0になってもオブジェクトは破棄されない。

			return l_pointer;

		};

		//現在のリファレンスカウント値を取得します。
		DEF_FORCE_INLINE uint32_t GetRefCount(void)
		{
			//return m_Count->load(std::memory_order_acquire);
			return m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
		};

		//NullptrならTrue
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		DEF_FORCE_INLINE AllocatorClassType* operator ->(void)
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE AllocatorClassType* operator ->(void) const
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE AllocatorClassType& operator*(void) const
		{
			return (*m_Pointer);
		};

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(AllocatorClassType* _transfer_managed_pointer_, AllocatorSharedSmtPtr<AllocatorClassType>& _out_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;

#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//l_count = new std::atomic<unsigned int>(1);
				l_count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//l_count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_count == nullptr)
			{
				return false;
			};
#endif

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;

			return true;
		};

	};

	//最低限の汎用スマートポインタ========================================================================
	//配列版は<ptype[]>としてこの定義の下に特殊化定義しています。
	//使用する際は静的メンバ関数でCreateしてください。カスタムアロケータを使用したい場合はそれぞれ指定してください。
	//アロケータ拡張
	template <class pType, class Enable>
	class SharedSmtPtr
	{
		//ポインタ型は許容していません。内部でpType* とするため指定自体はオブジェクト型としてください。
		static_assert(!SLIB_CVRT_IS_POINTER(pType), "Please Used NoPointerType.");
		//Enableパラメータは内部の判定で使用する領域なので使用者は指定しないでください。（指定を省略してください)
		static_assert(std::is_same<Enable, void>::value, "Enable parameter should not be specified by the user.");

		template <class before, class after>
		friend bool SharedCast_Dynamic(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

		template <class before, class after>
		friend void SharedCast_Reinterpret(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_);

		template <class AllocatorClassType>
		friend class AllocatorSharedSmtPtr;

	private:
		pType* m_Pointer;
		//std::atomic<unsigned int>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		SonikLib::SonikAtomic<uint32_t>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	private:
		DEF_FORCE_INLINE void AddRef(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			//{
			//	//no process
			//};
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt + 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


		};

		DEF_FORCE_INLINE void Release(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Allocate->memdel(m_Count);

				m_Pointer = nullptr;
				m_Count = nullptr;
				return;
			}

			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt - 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Allocate->memdel(m_Count);
			};

			m_Pointer = nullptr;
			m_Count = nullptr;
		};

	public:

		//constructor
		DEF_FORCE_INLINE SharedSmtPtr(void)
			:m_Pointer(nullptr)
			, m_Count(nullptr)
		{
			//initialize only
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(_SmtPtr_.m_Allocate)
		{

			AddRef();
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(SharedSmtPtr<pType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(std::move(_SmtPtr_.m_Allocate))
		{
			_SmtPtr_.m_Pointer = nullptr;
			_SmtPtr_.m_Count = nullptr;
			_SmtPtr_.m_Allocate = std::move(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>());
		};

		//Destructor
		DEF_FORCE_INLINE ~SharedSmtPtr(void) SLIB_CVR_NOEXCEPT
		{
			Release();
		};

		//operator =
		DEF_FORCE_INLINE SharedSmtPtr<pType>& operator =(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = m_Allocate;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = _SmtPtr_.m_Allocate;

				AddRef();
			};

			return (*this);
		};

		//MoveEqual
		DEF_FORCE_INLINE SharedSmtPtr<pType>& operator =(SharedSmtPtr<pType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = std::move(m_Allocate);

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = std::move(_SmtPtr_.m_Allocate);

				_SmtPtr_.m_Pointer = nullptr;
				_SmtPtr_.m_Count = nullptr;
				_SmtPtr_.m_Allocate = std::move(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>());
				/*_SmtPtr_.m_Allocate はオブジェクトなのでここでの初期化は必要ない*/

				//moveなのでAddref()をコールする必要はない。
			};

			return (*this);
		};

		DEF_FORCE_INLINE bool operator ==(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == _SmtPtr_.m_Pointer) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
	//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
	// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
	//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
	// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif

#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif	
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;

			m_Pointer = nullptr;
			Release(); //m_Pointer に nullptr を入れた上でdeleteコールしてるため参照カウンタが0になってもオブジェクトは破棄されない。

			return l_pointer;

		};

		//現在のリファレンスカウント値を取得します。
		DEF_FORCE_INLINE uint32_t GetRefCount(void)
		{
			//return m_Count->load(std::memory_order_acquire);
			return m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
		};

		//NullptrならTrue
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		DEF_FORCE_INLINE pType* operator ->(void)
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType* operator ->(void) const
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType& operator*(void) const
		{
			return (*m_Pointer);
		};

#if SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_VISUALSTUDIO_ENABLE || SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_CPP_ENABLE //VS2013以上かC++11以上なら有効化
		//C++11 以降 ->*演算子オーバーロード(メンバ関数ポインタ用)
		// メンバ関数ポインタを使用するためのオーバーロード演算子=========================
		template <class RetType, class ... Args>
		class MemberFunctionCaller
		{
			pType* object;
			RetType(pType::* func)(Args...);
		public:
			DEF_FORCE_INLINE MemberFunctionCaller(pType* obj, RetType(pType::* f)(Args...))
				: object(obj)
				, func(f)
			{
				//no process;
			};

			DEF_FORCE_INLINE RetType operator()(Args... args) const
			{
				return (object->*func)(std::forward<Args>(args)...);
			}
		};

		template <class RetType, class ... Args>
		DEF_FORCE_INLINE MemberFunctionCaller<RetType, Args...> operator->*(RetType(pType::* func)(Args...))
		{
			return MemberFunctionCaller<RetType, Args...>(m_Pointer, func);
		};
		//==================================================
#endif

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType>& _out_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<uint32_t>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = l_allocate->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = l_allocate->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = l_allocate->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = l_allocate->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;

			if (_allocate_.IsNullptr())
			{
				return false;
			};

#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = _allocate_->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = _allocate_->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = _allocate_->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = _allocate_->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//リテラル型用特殊化＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	template <class pType>
	class SharedSmtPtr<pType, typename std::enable_if<std::is_arithmetic<pType>::value>::type>
	{
		//ポインタ型は許容していません。内部でpType* とするため指定自体はオブジェクト型としてください。
		static_assert(!SLIB_CVRT_IS_POINTER(pType), "Please Used NoPointerType.");

		template <class before, class after>
		friend bool SharedCast_Dynamic(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

		template <class before, class after>
		friend void SharedCast_Reinterpret(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_);

	protected:
		pType* m_Pointer;
		//std::atomic<unsigned int>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		SonikLib::SonikAtomic<uint32_t>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	protected:
		DEF_FORCE_INLINE void AddRef(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt + 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};

		};

		DEF_FORCE_INLINE void Release(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Allocate->memdel(m_Count);

				m_Pointer = nullptr;
				m_Count = nullptr;
				return;
			}

			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt - 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Allocate->memdel(m_Count);
			};

			m_Pointer = nullptr;
			m_Count = nullptr;
		};

	public:

		//constructor
		DEF_FORCE_INLINE SharedSmtPtr(void)
			:m_Pointer(nullptr)
			, m_Count(nullptr)
		{
			//initialize only
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(_SmtPtr_.m_Allocate)
		{

			AddRef();
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(SharedSmtPtr<pType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(std::move(_SmtPtr_.m_Allocate))
		{
			_SmtPtr_.m_Pointer = nullptr;
			_SmtPtr_.m_Count = nullptr;
			_SmtPtr_.m_Allocate = std::move(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>());
		};

		//Destructor
		DEF_FORCE_INLINE ~SharedSmtPtr(void) SLIB_CVR_NOEXCEPT
		{
			Release();
		};

		//operator =
		DEF_FORCE_INLINE SharedSmtPtr<pType>& operator =(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = m_Allocate;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = _SmtPtr_.m_Allocate;

				AddRef();
			};

			return (*this);
		};

		//MoveEqual
		DEF_FORCE_INLINE SharedSmtPtr<pType>& operator =(SharedSmtPtr<pType>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = std::move(m_Allocate);

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = std::move(_SmtPtr_.m_Allocate);

				_SmtPtr_.m_Pointer = nullptr;
				_SmtPtr_.m_Count = nullptr;
				_SmtPtr_.m_Allocate = std::move(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>());
				/*_SmtPtr_.m_Allocate はオブジェクトなのでここでの初期化は必要ない*/

				//moveなのでAddref()をコールする必要はない。
			};

			return (*this);
		};

		DEF_FORCE_INLINE bool operator ==(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == _SmtPtr_.m_Pointer) ? true : false;
		};

		//生ポインタとして取得します。
//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif

#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif	
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;

			m_Pointer = nullptr;
			Release(); //m_Pointer に nullptr を入れた上でdeleteコールしてるため参照カウンタが0になってもオブジェクトは破棄されない。

			return l_pointer;

		};

		//現在のリファレンスカウント値を取得します。
		DEF_FORCE_INLINE uint32_t GetRefCount(void)
		{
			//return m_Count->load(std::memory_order_acquire);
			return m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
		};

		//NullptrならTrue
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		DEF_FORCE_INLINE pType* operator ->(void)
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType* operator ->(void) const
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType& operator*(void) const
		{
			return (*m_Pointer);
		};

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType>& _out_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
//				void* l_MemBlock = l_allocate->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = l_allocate->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = l_allocate->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = l_allocate->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
            l_count = new(l_MemBlock) SonikLib::SonikAtomic(static_cast<uint32_t>(1));

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;

			if (_allocate_.IsNullptr())
			{
				return false;
			};

#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
//				void* l_MemBlock = _allocate_->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = _allocate_->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));
			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = _allocate_->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = _allocate_->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = _allocate_;

			return true;
		};
	};


	//配列特殊化==================================
	//最低限の配列用特殊化のスマートポインタ 配列用特殊化1
	template <class pType, class Enable>
	class SharedSmtPtr<pType[], Enable>
	{
		//Enableパラメータは内部の判定で使用する領域なので使用者は指定しないでください。（指定を省略してください)
		static_assert(std::is_same<Enable, void>::value, "Enable parameter should not be specified by the user.");

		template <class before, class after>
		friend bool SharedCast_Dynamic(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

		template <class before, class after>
		friend void SharedCast_Reinterpret(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_);

	private:
		pType* m_Pointer;
		//std::atomic<unsigned int>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		SonikLib::SonikAtomic <uint32_t>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	private:
		DEF_FORCE_INLINE void AddRef(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt + 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};

		};

		DEF_FORCE_INLINE void Release(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor_Array(m_Pointer);
				m_Allocate->memdelArray(m_Pointer);
				m_Allocate->memdel(m_Count);

				m_Pointer = nullptr;
				m_Count = nullptr;
				return;
			}

			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt - 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor_Array(m_Pointer);
				m_Allocate->memdelArray(m_Pointer);
				m_Allocate->memdel(m_Count);
			};

			m_Pointer = nullptr;
			m_Count = nullptr;
		};

	public:

		//constructor
		DEF_FORCE_INLINE SharedSmtPtr(void)
			:m_Pointer(nullptr)
			, m_Count(nullptr)
		{
			//initialize only
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(const SharedSmtPtr<pType[]>& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(_SmtPtr_.m_Allocate)
		{

			AddRef();
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(SharedSmtPtr<pType[]>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(std::move(_SmtPtr_.m_Allocate))
		{
			_SmtPtr_.m_Pointer = nullptr;
			_SmtPtr_.m_Count = nullptr;
			_SmtPtr_.m_Allocate = std::move(SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>());
		};

		//Destructor
		DEF_FORCE_INLINE ~SharedSmtPtr(void) SLIB_CVR_NOEXCEPT
		{
			Release();
		};

		//operator =
		DEF_FORCE_INLINE SharedSmtPtr<pType[]>& operator =(const SharedSmtPtr<pType[]>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = m_Allocate;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = _SmtPtr_.m_Allocate;

				AddRef();
			};

			return (*this);
		};

		//MoveEqual
		DEF_FORCE_INLINE SharedSmtPtr<pType[]>& operator =(SharedSmtPtr<pType[]>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = std::move(m_Allocate);

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = std::move(_SmtPtr_.m_Allocate);

				_SmtPtr_.m_Pointer = nullptr;
				_SmtPtr_.m_Count = nullptr;
				_SmtPtr_.m_Allocate = std::move(SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>());
				/*_SmtPtr_.m_Allocate はオブジェクトなのでここでの初期化は必要ない*/

				//moveなのでAddref()をコールする必要はない。
			};

			return (*this);
		};

		DEF_FORCE_INLINE bool operator ==(const SharedSmtPtr<pType[]>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == _SmtPtr_.m_Pointer) ? true : false;
		};

		//生ポインタとして取得します。
//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif

#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif	
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;

			m_Pointer = nullptr;
			Release(); //m_Pointer に nullptr を入れた上でdeleteコールしてるため参照カウンタが0になってもオブジェクトは破棄されない。

			return l_pointer;

		};

		//現在のリファレンスカウント値を取得します。
		DEF_FORCE_INLINE uint32_t GetRefCount(void)
		{
			//return m_Count->load(std::memory_order_acquire);
			return m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
		};

		//NullptrならTrue
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		DEF_FORCE_INLINE const pType& operator[](uint64_t index) const
		{
			return m_Pointer[index];
		}

		DEF_FORCE_INLINE pType& operator[](uint64_t index)
		{
			return m_Pointer[index];
		};

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType[]>& _out_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = l_allocate->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = l_allocate->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = l_allocate->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = l_allocate->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType[]>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;

			if (_allocate_.IsNullptr())
			{
				return false;
			};

#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = _allocate_->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = _allocate_->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));
			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = _allocate_->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = _allocate_->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
            l_count = new(l_MemBlock) SonikLib::SonikAtomic(static_cast<uint32_t>(1));

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//リテラル配列特殊化==================================
	template <class pType>
	class SharedSmtPtr<pType[], typename std::enable_if<std::is_arithmetic<pType>::value>::type>
	{
		template <class before, class after>
		friend bool SharedCast_Dynamic(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

		template <class before, class after>
		friend void SharedCast_Reinterpret(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_);

	private:
		pType* m_Pointer;
		//std::atomic<unsigned int>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		SonikLib::SonikAtomic<uint32_t>* m_Count; //もったいないがm_Pointerがnullptrのときもカウント1として領域をnewする。
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	private:
		DEF_FORCE_INLINE void AddRef(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt + 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt + 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};

		};

		DEF_FORCE_INLINE void Release(void) SLIB_CVR_NOEXCEPT
		{
			if (m_Count == nullptr)
			{
				return;
			};

			//unsigned int TmpCnt = m_Count->load(std::memory_order_acquire);
			uint32_t TmpCnt = m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor_Array(m_Pointer);
				m_Allocate->memdelArray(m_Pointer);
				m_Allocate->memdel(m_Count);

				m_Pointer = nullptr;
				m_Count = nullptr;
				return;
			}

			//while (!m_Count->compare_exchange_strong(TmpCnt, TmpCnt - 1, std::memory_order_acq_rel))
			while (!m_Count->CompareExchange_Strong(TmpCnt, TmpCnt - 1, SonikLib::SlibAtomicMemoryOrder::order_acq_rel))
			{
				//no process
			};


			if (TmpCnt == 1)
			{
				/*
				delete m_Pointer;
				delete m_Count;
				*/

				//m_Allocate->CallDestructor_Array(m_Pointer);
				m_Allocate->memdelArray(m_Pointer);
				m_Allocate->memdel(m_Count);
			};

			m_Pointer = nullptr;
			m_Count = nullptr;
		};

	public:

		//constructor
		DEF_FORCE_INLINE SharedSmtPtr(void)
			:m_Pointer(nullptr)
			, m_Count(nullptr)
		{
			//initialize only
		};

		//コピーコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(const SharedSmtPtr<pType[]>& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(_SmtPtr_.m_Allocate)
		{

			AddRef();
		};

		//ムーヴコンストラクタ
		DEF_FORCE_INLINE SharedSmtPtr(SharedSmtPtr<pType[]>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_SmtPtr_.m_Pointer)
			, m_Count(_SmtPtr_.m_Count)
			, m_Allocate(std::move(_SmtPtr_.m_Allocate))
		{
			_SmtPtr_.m_Pointer = nullptr;
			_SmtPtr_.m_Count = nullptr;
			_SmtPtr_.m_Allocate = std::move(SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>());
		};

		//Destructor
		DEF_FORCE_INLINE ~SharedSmtPtr(void) SLIB_CVR_NOEXCEPT
		{
			Release();
		};

		//operator =
		DEF_FORCE_INLINE SharedSmtPtr<pType[]>& operator =(const SharedSmtPtr<pType[]>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = m_Allocate;

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = _SmtPtr_.m_Allocate;

				AddRef();
			};

			return (*this);
		};

		//MoveEqual
		DEF_FORCE_INLINE SharedSmtPtr<pType[]>& operator =(SharedSmtPtr<pType[]>&& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			//自己代入を解決する実装。
			if (m_Pointer != _SmtPtr_.m_Pointer)
			{
				SharedSmtPtr<pType> lsp;
				lsp.m_Pointer = m_Pointer;
				lsp.m_Count = m_Count;
				lsp.m_Allocate = std::move(m_Allocate);

				m_Pointer = _SmtPtr_.m_Pointer;
				m_Count = _SmtPtr_.m_Count;
				m_Allocate = std::move(_SmtPtr_.m_Allocate);

				_SmtPtr_.m_Pointer = nullptr;
				_SmtPtr_.m_Count = nullptr;
				_SmtPtr_.m_Allocate = std::move(SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>());
				/*_SmtPtr_.m_Allocate はオブジェクトなのでここでの初期化は必要ない*/

				//moveなのでAddref()をコールする必要はない。
			};

			return (*this);
		};

		DEF_FORCE_INLINE bool operator ==(const SharedSmtPtr<pType>& _SmtPtr_) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == _SmtPtr_.m_Pointer) ? true : false;
		};

		//生ポインタとして取得します。
//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif

#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif	
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;

			m_Pointer = nullptr;
			Release(); //m_Pointer に nullptr を入れた上でdeleteコールしてるため参照カウンタが0になってもオブジェクトは破棄されない。

			return l_pointer;

		};

		//現在のリファレンスカウント値を取得します。
		DEF_FORCE_INLINE uint32_t GetRefCount(void)
		{
			//return m_Count->load(std::memory_order_acquire);
			return m_Count->load(SonikLib::SlibAtomicMemoryOrder::order_acquire);
		};

		//NullptrならTrue
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		DEF_FORCE_INLINE const pType& operator[](uint64_t index) const
		{
			return m_Pointer[index];
		}

		DEF_FORCE_INLINE pType& operator[](uint64_t index)
		{
			return m_Pointer[index];
		};

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType[]>& _out_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = l_allocate->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = l_allocate->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = l_allocate->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = l_allocate->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, SharedSmtPtr<pType[]>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//std::atomic<unsigned int>* l_count = nullptr;
			SonikLib::SonikAtomic<uint32_t>* l_count = nullptr;

			if (_allocate_.IsNullptr())
			{
				return false;
			};

#if defined(SLIB_ALLOCATOR_V_EXCEPTION)
			try
			{
				//void* l_MemBlock = _allocate_->memal_Exception(sizeof(std::atomic<unsigned int>));
				void* l_MemBlock = _allocate_->memal_Exception(sizeof(SonikLib::SonikAtomic<uint32_t>));

			}
			catch (std::bad_alloc)
			{
				throw;
			}
			catch (...)
			{
				throw;
			};
#else
			//void* l_MemBlock = _allocate_->memal(sizeof(std::atomic<unsigned int>));
			void* l_MemBlock = _allocate_->memal(sizeof(SonikLib::SonikAtomic<uint32_t>));
			if (l_MemBlock == nullptr)
			{
				return false;
			};
#endif

			//l_count = new(l_MemBlock) std::atomic<unsigned int>(1);
			l_count = new(l_MemBlock) SonikLib::SonikAtomic<uint32_t>(1);

			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Count = l_count;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};


	//UniquePtr===========================================================================================================================================================================
	template <class pType, class Enable>
	class UniqueSmtPtr
	{
		//ポインタ型は許容していません。内部でpType* とするため指定自体はオブジェクト型としてください。
		static_assert(!SLIB_CVRT_IS_POINTER(pType), "Please Used NoPointerType.");
		//Enableパラメータは内部の判定で使用する領域なので使用者は指定しないでください。（指定を省略してください)
		static_assert(std::is_same<Enable, void>::value, "Enable parameter should not be specified by the user.");

		template <class before, class after>
		friend bool UniqueCast_Dynamic(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

		template <class before, class after>
		friend void UniqueCast_Reinterpret(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_);

	private:
		pType* m_Pointer;
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	public:
		//コンストラクタ
		DEF_FORCE_INLINE UniqueSmtPtr(void) SLIB_CVR_NOEXCEPT
			:m_Pointer(nullptr)
		{
			//no process
		};

		//コピーコンストラクタ（実質ユニークポインタなのでムーヴコンストラクタに等しい、がstd::moveをいちいち使うのもめんどくさい場合もあるので。)
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType>& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//ムーヴコンストラクタ。本当のムーヴコンストラクタ。std::move使用時にコールされまっせ。
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType>&& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//デストラクタ
		DEF_FORCE_INLINE ~UniqueSmtPtr(void)
		{
			//m_Allocate->CallDestructor(m_Pointer);
			m_Allocate->memdel(m_Pointer);
		};

		//operator = (Copy)
		DEF_FORCE_INLINE UniqueSmtPtr<pType>& operator =(UniqueSmtPtr<pType>& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//operator =(Move)
		DEF_FORCE_INLINE UniqueSmtPtr<pType>& operator =(UniqueSmtPtr<pType>&& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//Null なら True
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;
			m_Pointer = nullptr;

			return l_pointer;
		};

		DEF_FORCE_INLINE pType* operator ->(void)
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType* operator ->(void) const
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType& operator*(void) const
		{
			return (*m_Pointer);
		};

#if SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_VISUALSTUDIO_ENABLE || SLIB_SMARTPOINTER_VARIABLE_TEMPLATE_CPP_ENABLE //VS2013以上かC++11以上なら有効化
		//C++11 以降 ->*演算子オーバーロード(メンバ関数ポインタ用)
		// メンバ関数ポインタを使用するためのオーバーロード演算子=========================
		template <class RetType, class ... Args>
		class MemberFunctionCaller
		{
			pType* object;
			RetType(pType::* func)(Args...);
		public:
			DEF_FORCE_INLINE MemberFunctionCaller(pType* obj, RetType(pType::* f)(Args...))
				: object(obj)
				, func(f)
			{
				//no process;
			};

			DEF_FORCE_INLINE RetType operator()(Args... args) const
			{
				return (object->*func)(std::forward<Args>(args)...);
			}
		};

		template <class RetType, class ... Args>
		DEF_FORCE_INLINE MemberFunctionCaller<RetType, Args...> operator->*(RetType(pType::* func)(Args...))
		{
			return MemberFunctionCaller<RetType, Args...>(m_Pointer, func);
		};
		//==================================================
#endif

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType>& _out_)
		{
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//リテラル型用特殊化==================================
	template  <class pType>
	class UniqueSmtPtr<pType, typename std::enable_if<std::is_arithmetic<pType>::value>::type>
	{
		//ポインタ型は許容していません。内部でpType* とするため指定自体はオブジェクト型としてください。
		static_assert(!SLIB_CVRT_IS_POINTER(pType), "Please Used NoPointerType.");

		template <class before, class after>
		friend bool UniqueCast_Dynamic(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

		template <class before, class after>
		friend void UniqueCast_Reinterpret(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_);

	private:
		pType* m_Pointer;
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	public:
		//コンストラクタ
		DEF_FORCE_INLINE UniqueSmtPtr(void) SLIB_CVR_NOEXCEPT
			:m_Pointer(nullptr)
		{
			//no process
		};

		//コピーコンストラクタ（実質ユニークポインタなのでムーヴコンストラクタに等しい、がstd::moveをいちいち使うのもめんどくさい場合もあるので。)
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType>& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//ムーヴコンストラクタ。本当のムーヴコンストラクタ。std::move使用時にコールされまっせ。
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType>&& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//デストラクタ
		DEF_FORCE_INLINE ~UniqueSmtPtr(void)
		{
			//m_Allocate->CallDestructor(m_Pointer);
			m_Allocate->memdel(m_Pointer);
		};

		//operator = (Copy)
		DEF_FORCE_INLINE UniqueSmtPtr<pType>& operator =(UniqueSmtPtr<pType>& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//operator =(Move)
		DEF_FORCE_INLINE UniqueSmtPtr<pType>& operator =(UniqueSmtPtr<pType>&& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//Null なら True
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;
			m_Pointer = nullptr;

			return l_pointer;
		};

		DEF_FORCE_INLINE pType* operator ->(void)
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType* operator ->(void) const
		{
			return m_Pointer;

		};

		DEF_FORCE_INLINE pType& operator*(void) const
		{
			return (*m_Pointer);
		};

		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType>& _out_)
		{
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
            l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic(static_cast<uint32_t>(1));
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//配列特殊化=========================================================
	template <class pType, class Enable>
	class UniqueSmtPtr<pType[], Enable>
	{
		//Enableパラメータは内部の判定で使用する領域なので使用者は指定しないでください。（指定を省略してください)
		static_assert(std::is_same<Enable, void>::value, "Enable parameter should not be specified by the user.");

		template <class before, class after>
		friend bool UniqueCast_Dynamic(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

		template <class before, class after>
		friend void UniqueCast_Reinterpret(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_);

	private:
		pType* m_Pointer;
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_Allocate; //アロケータ

	public:
		//コンストラクタ
		DEF_FORCE_INLINE UniqueSmtPtr(void) SLIB_CVR_NOEXCEPT
			:m_Pointer(nullptr)
		{
			//no process
		};

		//コピーコンストラクタ（実質ユニークポインタなのでムーヴコンストラクタに等しい、がstd::moveをいちいち使うのもめんどくさい場合もあるので。)
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType[]>& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//ムーヴコンストラクタ。本当のムーヴコンストラクタ。std::move使用時にコールされまっせ。
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType[]>&& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//デストラクタ
		DEF_FORCE_INLINE ~UniqueSmtPtr(void)
		{
			//m_Allocate->CallDestructor(m_Pointer);
			m_Allocate->memdel(m_Pointer);
		};

		//operator = (Copy)
		DEF_FORCE_INLINE UniqueSmtPtr<pType[]>& operator =(UniqueSmtPtr<pType[]>& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//operator =(Move)
		DEF_FORCE_INLINE UniqueSmtPtr<pType[]>& operator =(UniqueSmtPtr<pType[]>&& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//Null なら True
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;
			m_Pointer = nullptr;

			return l_pointer;
		};

		DEF_FORCE_INLINE const pType& operator[](uint64_t index) const
		{
			return m_Pointer[index];
		}

		DEF_FORCE_INLINE pType& operator[](uint64_t index)
		{
			return m_Pointer[index];
		};


		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType[]>& _out_)
		{
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType[]>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//リテラル配列特殊化==================================
	template <class pType>
	class UniqueSmtPtr<pType[], typename std::enable_if<std::is_arithmetic<pType>::value>::type>
	{
		template <class before, class after>
		friend bool UniqueCast_Dynamic(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

		template <class before, class after>
		friend void UniqueCast_Reinterpret(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_);

		template <class PtrTemplateValueType>
		friend void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_);

	private:
		pType* m_Pointer;
		AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface > m_Allocate; //アロケータ

	public:
		//コンストラクタ
		DEF_FORCE_INLINE UniqueSmtPtr(void) SLIB_CVR_NOEXCEPT
			:m_Pointer(nullptr)
		{
			//no process
		};

		//コピーコンストラクタ（実質ユニークポインタなのでムーヴコンストラクタに等しい、がstd::moveをいちいち使うのもめんどくさい場合もあるので。)
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType[]>& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//ムーヴコンストラクタ。本当のムーヴコンストラクタ。std::move使用時にコールされまっせ。
		DEF_FORCE_INLINE UniqueSmtPtr(UniqueSmtPtr<pType[]>&& _OwnerSwap_) SLIB_CVR_NOEXCEPT
			:m_Pointer(_OwnerSwap_.m_Pointer)
			, m_Allocate(std::move(_OwnerSwap_.m_Allocate))
		{
			_OwnerSwap_.m_Pointer = nullptr;
			_OwnerSwap_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
		};

		//デストラクタ
		DEF_FORCE_INLINE ~UniqueSmtPtr(void)
		{
			//m_Allocate->CallDestructor(m_Pointer);
			m_Allocate->memdel(m_Pointer);
		};

		//operator = (Copy)
		DEF_FORCE_INLINE UniqueSmtPtr<pType[]>& operator =(UniqueSmtPtr<pType[]>& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//operator =(Move)
		DEF_FORCE_INLINE UniqueSmtPtr<pType[]>& operator =(UniqueSmtPtr<pType[]>&& _SetObj_) SLIB_CVR_NOEXCEPT
		{
			if (m_Pointer != _SetObj_.m_Pointer)
			{
				//m_Allocate->CallDestructor(m_Pointer);
				m_Allocate->memdel(m_Pointer);
				m_Pointer = _SetObj_.m_Pointer;
				m_Allocate = std::move(_SetObj_.m_Allocate);

				_SetObj_.m_Pointer = nullptr;
				_SetObj_.m_Allocate = SonikLib::SharedSmtPtr<SonikLib::SLibAllocateInterface>();
			};

			return (*this);
		};

		//Null なら True
		DEF_FORCE_INLINE bool IsNullptr(void) SLIB_CVR_NOEXCEPT
		{
			return (m_Pointer == nullptr) ? true : false;
		};

		//生ポインタとして取得します。
		//deleteしないように注意してください。
#if !defined(SLIB_SMTPTR_ASSERTOFF)
#if defined(__GNUC__) || defined(__clang__)
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT __attribute__((warning("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n")))

#elif defined(_MSC_VER) 
		//【コンパイル影響無し】生ポインタの取得がコールされました。取得先でのdeleteに強く注意してください。
		// 警告を無効化するには SLIB_SMTPTR_ASSERTOFF を定義してください。
		__declspec(deprecated("【No compilation effect】A raw pointer get was called. Be very careful with deletes on the getter.\nPlease define \"SLIB_SMTPTR_ASSERTOFF\" to disable it.\n"))
			DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
#else
		DEF_FORCE_INLINE pType* GetPointer(void) SLIB_CVR_NOEXCEPT
#endif
		{
			return m_Pointer;
		};

		//持っているポインタの所有権のみを破棄します。
		//外部で責任をもって破棄してください。
		DEF_FORCE_INLINE pType* DestroyOwner(void) SLIB_CVR_NOEXCEPT
		{
			pType* l_pointer = m_Pointer;
			m_Pointer = nullptr;

			return l_pointer;
		};

		DEF_FORCE_INLINE const pType& operator[](uint64_t index) const
		{
			return m_Pointer[index];
		}

		DEF_FORCE_INLINE pType& operator[](uint64_t index)
		{
			return m_Pointer[index];
		};


		//クリエイタ
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType[]>& _out_)
		{
			AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocate;

			l_allocate.m_Pointer = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_allocate.m_Pointer == nullptr)
			{
				return false;
			};

			//l_allocate.m_Count = new(std::nothrow) std::atomic<unsigned int>(1);
			l_allocate.m_Count = new(std::nothrow) SonikLib::SonikAtomic<uint32_t>(1);
			if (l_allocate.m_Count == nullptr)
			{
				delete l_allocate.m_Pointer;
				return false;
			};


			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = std::move(l_allocate);

			return true;
		};

		//クリエイタ(オーバーロード
		DEF_FORCE_INLINE static bool SmartPointerCreate(pType* _transfer_managed_pointer_, UniqueSmtPtr<pType[]>& _out_, AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>& _allocate_)
		{
			//出力先へ
			_out_.m_Pointer = _transfer_managed_pointer_;
			_out_.m_Allocate = _allocate_;

			return true;
		};

	};

	//各Castの実装=====================================================================================================================================================================
	//SRC側のポインタを別の型に変換し、参照カウンタを増やします。
	template <class before, class after>
	DEF_FORCE_INLINE bool SharedCast_Dynamic(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_)
	{
		after* l_po = dynamic_cast<after*>(_src_.m_Pointer);
		if (l_po == nullptr)
		{
			return false;
		};

		_dst_.Release();
		_dst_.m_Pointer = l_po;
		_dst_.m_Count = _src_.m_Count;
		_dst_.m_Allocate = _src_.m_Allocate;

		_dst_.AddRef();

		return true;
	};

	template <class before, class after>
	DEF_FORCE_INLINE bool SharedCast_Dynamic(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_)
	{
		after* l_po = dynamic_cast<after*>(_src_.m_Pointer);
		if (l_po == nullptr)
		{
			return false;
		};

		_dst_.Release();
		_dst_.m_Pointer = l_po;
		_dst_.m_Count = _src_.m_Count;
		_dst_.m_Allocate = _src_.m_Allocate;

		_dst_.AddRef();
		return true;
	};

	template <class before, class after>
	DEF_FORCE_INLINE void SharedCast_Reinterpret(SharedSmtPtr<before>& _src_, SharedSmtPtr<after>& _dst_)
	{
		_dst_.Release();
		_dst_.m_Pointer = reinterpret_cast<after*>(_src_.m_Pointer);
		_dst_.m_Count = _src_.m_Count;
		_dst_.m_Allocate = _src_.m_Allocate;

		_dst_.AddRef();
	};

	template <class before, class after>
	DEF_FORCE_INLINE void SharedCast_Reinterpret(SharedSmtPtr<before[]>& _src_, SharedSmtPtr<after[]>& _dst_)
	{
		_dst_.Release();
		_dst_.m_Pointer = reinterpret_cast<after*>(_src_.m_Pointer);
		_dst_.m_Count = _src_.m_Count;
		_dst_.m_Allocate = _src_.m_Allocate;

		_dst_.AddRef();
	};

	//SRC側のポインタはnullptrになります。(唯一性によるキャスト移譲)
	template <class before, class after>
	DEF_FORCE_INLINE bool UniqueCast_Dynamic(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_)
	{
		after* l_po = dynamic_cast<after*>(_src_.m_pointer);
		if (l_po == nullptr)
		{
			return false;
		};

		_dst_.OwnerRelease();
		_dst_.m_pointer = l_po;
		_dst_.m_Allocate = std::move(_src_.m_Allocate);

		_src_.m_pointer = nullptr;

		return true;
	};

	template <class before, class after>
	DEF_FORCE_INLINE bool UniqueCast_Dynamic(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_)
	{
		after* l_po = dynamic_cast<after*>(_src_.m_pointer);
		if (l_po == nullptr)
		{
			return false;
		};

		_dst_.OwnerRelease();
		_dst_.m_pointer = l_po;
		_dst_.m_Allocate = std::move(_src_.m_Allocate);

		_src_.m_pointer = nullptr;

		return true;
	};

	template <class before, class after>
	DEF_FORCE_INLINE void UniqueCast_Reinterpret(UniqueSmtPtr<before>& _src_, UniqueSmtPtr<after>& _dst_)
	{
		_dst_.OwnerRelease();
		_dst_.m_pointer = reinterpret_cast<after*>(_src_.m_pointer);
		_dst_.m_Allocate = std::move(_src_.m_Allocate);

		_src_.m_pointer = nullptr;
	};

	template <class before, class after>
	DEF_FORCE_INLINE void UniqueCast_Reinterpret(UniqueSmtPtr<before[]>& _src_, UniqueSmtPtr<after[]>& _dst_)
	{
		_dst_.OwnerRelease();
		_dst_.m_pointer = reinterpret_cast<after*>(_src_.m_pointer);
		_dst_.m_Allocate = std::move(_src_.m_Allocate);

		_src_.m_pointer = nullptr;
	};

	//UniqueからSharedへの変換。片方向変換しかしません。なぜならUniqueへ変換してしまうとSharedの参照カウンタの整合性が取れなくなるからです。
	template <class PtrTemplateValueType>
	DEF_FORCE_INLINE void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType>& _unique_, SharedSmtPtr<PtrTemplateValueType>& _shared_)
	{
		//void* l_mblock = _unique_.m_Allocate.MemAlloc(sizeof(std::atomic<unsigned int>));
		void* l_mblock = _unique_.m_Allocate.MemAlloc(sizeof(SonikLib::SonikAtomic<uint32_t>));
		if (l_mblock == nullptr)
		{
			//失敗
			return;
		};

		//std::atomic<unsigned int>* l_count = new(l_mblock) std::atomic<unsigned int>(1);
		SonikLib::SonikAtomic<uint32_t>* l_count = new(l_mblock) SonikLib::SonikAtomic<uint32_t>(1);

		_shared_.Release();

		_shared_.m_Pointer = _unique_.m_pointer;
		_shared_.m_Allocate = _unique_.m_Allocate;
		_shared_.m_Count = l_count;

		_unique_.m_pointer = nullptr;

		//正常終了
	};

	template <class PtrTemplateValueType>
	DEF_FORCE_INLINE void SmtPtrConvert_UniqueToShared(UniqueSmtPtr<PtrTemplateValueType[]>& _unique_, SharedSmtPtr<PtrTemplateValueType[]>& _shared_)
	{
		//配列じゃない処理とかわらんとです。
		//void* l_mblock = _unique_.m_Allocate.MemAlloc(sizeof(std::atomic<unsigned int>));
		void* l_mblock = _unique_.m_Allocate.MemAlloc(sizeof(SonikLib::SonikAtomic<uint32_t>));
		if (l_mblock == nullptr)
		{
			//失敗
			return;
		};

		//std::atomic<unsigned int>* l_count = new(l_mblock) std::atomic<unsigned int>(1);
		SonikLib::SonikAtomic<uint32_t>* l_count = new(l_mblock) SonikLib::SonikAtomic<uint32_t>(1);

		_shared_.Release();

		_shared_.m_Pointer = _unique_.m_pointer;
		_shared_.m_Allocate = _unique_.m_Allocate;
		_shared_.m_Count = l_count;

		_unique_.m_pointer = nullptr;

		//正常終了
	};
};


#endif /* SMARTPOINTER_SONIK_SMARTPOINTER_HPP_ */
