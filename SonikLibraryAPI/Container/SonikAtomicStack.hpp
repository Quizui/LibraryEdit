#pragma once

#ifndef SONIKATOMICSTACK_H_
#define SONIKATOMICSTACK_H_

//一般的なStack(LIFO)機構を提供します。

#include <cstdint>
#include <new>

#include <SonikCAS/SonikAtomicLock.h>
#include <SmartPointer/SonikSmartPointer.hpp>
#include <CPPGrammarDefines.h>
#include <CompilersPreProcesser.h>

namespace SonikLib
{
	namespace Container
	{
		//ポインタ型を指定した場合は使用者側の所有権としています。内部ではdeleteしません。
		template<class StackType>
		class SonikAtomicStack
		{
		private:
			//インナークラス。ノードを外に出す実装はないので...。
			template<class ClsType>
			class StackNode
			{
			public:
				ClsType m_TemplateObject;						//実際のオブジェクト
				StackNode<ClsType>* Next;	//次のStackへのポインタ

			public:
				//construcotr
				DEF_FORCE_INLINE StackNode(void) SLIB_CVR_NOEXCEPT
					:Next(0)
				{
				};
				//destructor
				DEF_FORCE_INLINE ~StackNode(void) SLIB_CVR_NOEXCEPT
				{

				};
			};

			//using TypeNode = StackNode<StackType>;
			SLIB_CVR_USING(TypeNode, StackNode<StackType>);

		private:
			//フリーアイテムリスト
			TypeNode* m_StackAllocArea; //配列先頭のポインタ。
			TypeNode* m_Free; // リンクリストを操作するポインタ。

			//StackされたNode
			TypeNode* _first;
			TypeNode* _last;

			//設定した最大キュー数
			uint32_t Stack_RoundCount;

			//アトミックブロッククラス。マルチスレッド対応にするための排他制御クラスです。
			SonikLib::S_CAS::SonikAtomicLock shortblock;
			//アロケータ
			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

		private:
			SonikAtomicStack(const SonikAtomicStack& _copy_) = delete;
			SonikAtomicStack(SonikAtomicStack&& _move_) = delete;
			SonikAtomicStack& operator =(const SonikAtomicStack& _copy_) = delete;
			SonikAtomicStack& operator =(SonikAtomicStack&& _move_) = delete;

		public:
			DEF_FORCE_INLINE SonikAtomicStack(uint32_t StackItemNum = 100)
				:_first(nullptr)
				, _last(nullptr)
				, Stack_RoundCount(StackItemNum)
			{

				if (Stack_RoundCount >= UINT32_MAX)
				{
					--Stack_RoundCount;
				};

				SonikLib::SLibAllocateInterface* l_alloc_obj = nullptr;

				try
				{
					//アロケータオブジェクトの生成(実質デフォルトnewをする)
					l_alloc_obj = new SonikLib::SLibAllocateInterface;
					if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, m_allocator))
					{
						throw std::bad_alloc();
					};

					void* l_allocbuffer = m_allocator->memalArray_Exception(sizeof(TypeNode), Stack_RoundCount);
					//オブジェクトをカウント分ヒープからnewして再利用しまくる。
					m_StackAllocArea = new(l_allocbuffer) TypeNode[Stack_RoundCount];


				}
				catch (std::bad_alloc&)
				{
					if (m_allocator.IsNullptr())
					{
						delete l_alloc_obj;
					};
					throw;
				};

				//リンクをつなげる。
				m_Free = m_StackAllocArea;
				for (uint32_t i = 0; i < (Stack_RoundCount - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

			};
			DEF_FORCE_INLINE SonikAtomicStack(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint32_t StackItemNum = 100)
				:_first(nullptr)
				, _last(nullptr)
				, Stack_RoundCount(StackItemNum)
				, m_allocator(_allocator_)
			{

				if (Stack_RoundCount >= UINT32_MAX)
				{
					--Stack_RoundCount;
				};

				try
				{
					void* l_allocbuffer = m_allocator->memalArray_Exception(sizeof(TypeNode), Stack_RoundCount);
					//オブジェクトをカウント分ヒープからnewして再利用しまくる。
					m_StackAllocArea = new(l_allocbuffer) TypeNode[Stack_RoundCount];


				}
				catch (std::bad_alloc&)
				{
					throw;
				};

				//リンクをつなげる。
				m_Free = m_StackAllocArea;
				for (uint32_t i = 0; i < (Stack_RoundCount - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

			};

			DEF_FORCE_INLINE ~SonikAtomicStack(void)
			{
				//Stack内のオブジェクトの破棄までの責任は持たないので、そのまま配列delete
				if (m_StackAllocArea != nullptr)
				{
					//m_allocator->CallDestructor_Array(m_StackAllocArea, Stack_RoundCount);
					m_allocator->memdelArray(m_StackAllocArea, Stack_RoundCount);
				};

			};

			DEF_FORCE_INLINE bool Initialize(uint32_t StackItemMax) SLIB_CVR_NOEXCEPT
			{
				if (StackItemMax >= UINT32_MAX)
				{
					--StackItemMax;
				};

				//新しい領域を確保
				void* l_allocbuffer = m_allocator->memalArray(sizeof(TypeNode), StackItemMax);
				if (l_allocbuffer == nullptr)
				{
					return false;
				};

				//元々の領域を破棄。
				if (m_StackAllocArea != nullptr)
				{
					m_allocator->CallDestructor_Array(m_StackAllocArea);
					m_allocator->memdelArray(m_StackAllocArea);

					m_StackAllocArea = nullptr;
				};

				//オブジェクトをカウント分newして再利用しまくる。
				m_StackAllocArea = new(l_allocbuffer) TypeNode[StackItemMax];

				//リンクをつなげる。
				m_Free = m_StackAllocArea;
				for (uint32_t i = 0; i < (StackItemMax - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

				//変数セットして終了
				Stack_RoundCount = StackItemMax;
				return true;
			};

			//書き込み(プッシュ)
			DEF_FORCE_INLINE bool Push(StackType SetData) SLIB_CVR_NOEXCEPT
			{
				TypeNode* lp_ptr;

				shortblock.lock();

				if (m_Free == nullptr)
				{
					//フリーリストがなかったら失敗。
					shortblock.unlock();
					return false;
				};

				//フリーリストから取得。
				lp_ptr = m_Free;
				m_Free = m_Free->Next;

				//LIFO実装
				//新しいノードの Next を、現在の先頭ノード (_first) に繋げる。
				lp_ptr->Next = _first;

				//新しいノード (lp_ptr) を、新しい先頭ノード (_first) とする。
				_first = lp_ptr;

				//_last の更新: リストが空だった場合にのみ更新する。(リストが空のときは、新しい要素が最初で最後の要素となるため)
				if (_last == nullptr)
				{
					_last = lp_ptr;
				};


				//値設定。
				lp_ptr->m_TemplateObject = SetData;

				//ポインタの付替えは終わったのでブロック解除
				shortblock.unlock();

				//終了
				return true;
			};

			//取得（Pop)
			//TryLock版(ロックが取れなければすぐに処理が返ります。
			DEF_FORCE_INLINE bool TryPop(StackType& GetValue) SLIB_CVR_NOEXCEPT
			{

				if (!shortblock.try_lock())
				{
					return false;
				};

				if (_first == nullptr && _last == nullptr)
				{
					//要素がないよ。
					shortblock.unlock();
					return false;
				};

				//要素がある場合。
				TypeNode* pTmp;

				//とりあえず取得してポインタを進める。
				pTmp = _first;

				//先頭を次のノードへ進める
				_first = _first->Next;

				//_firstがnullptrになった場合（リストが空になった場合）、_lastもクリアする
				if (_first == nullptr)
				{
					_last = nullptr;
				}

				//Popしたものはフリーブロックへ移動させる。
				pTmp->Next = m_Free;
				m_Free = pTmp;

				//値取得
				//すでにフリーリストに入っているのでブロック解除のタイミングで値が代入されてしまう可能性があるため
				//ブロック解除前のタイミングで取得する。
				GetValue = pTmp->m_TemplateObject;

				//ブロック解除
				shortblock.unlock();

				//正常終了。
				return true;
			};

			//取得（Pop)
			//通常ロック版(ビジーループでロックが取れるまで待ちます。)
			DEF_FORCE_INLINE bool Pop(StackType& GetValue) SLIB_CVR_NOEXCEPT
			{

				shortblock.lock();

				if (_first == nullptr && _last == nullptr)
				{
					//要素がないよ。
					shortblock.unlock();
					return false;
				};

				//要素がある場合。
				TypeNode* pTmp;

				//とりあえず取得してポインタを進める。
				pTmp = _first;

				//先頭を次のノードへ進める
				_first = _first->Next;

				//_firstがnullptrになった場合（リストが空になった場合）、_lastもクリアする
				if (_first == nullptr)
				{
					_last = nullptr;
				}

				//Popしたものはフリーブロックへ移動させる。
				pTmp->Next = m_Free;
				m_Free = pTmp;

				//値取得
				//すでにフリーリストに入っているのでブロック解除のタイミングで値が代入されてしまう可能性があるため
				//ブロック解除前のタイミングで取得する。
				GetValue = pTmp->m_TemplateObject;

				//ブロック解除
				shortblock.unlock();

				//正常終了。
				return true;
			};
		};

	}; //end namespace Container

}; //end namespace SonikLib


#endif /* SONIKATOMICSTACK_H_ */