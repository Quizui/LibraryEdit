#pragma once

#ifndef SONIKATOMICQUEUE_H_
#define SONIKATOMICQUEUE_H_

 //一般的なQueue機構を提供します。

#include <stdint.h>
#include <new>

#include "../SonikCAS/SonikAtomicLock.h"
#include "../SmartPointer/SonikSmartPointer.hpp"

namespace SonikLib
{
	namespace Container
	{
		//ポインタ型を指定した場合は使用者側の所有権としています。内部ではdeleteしません。
		template<class QueueType>
		class SonikAtomicQueue
		{
		private:
			//インナークラス。ノードを外に出す実装はないので...。
			template<class ClsType>
			class QueueNode
			{
			public:
				ClsType m_TemplateObject;						//実際のオブジェクト
				QueueNode<ClsType>* Next;	//次のQueueへのポインタ

			public:
				//construcotr
				inline QueueNode(void) noexcept
					:Next(0)
				{
				};
				//destructor
				inline ~QueueNode(void) noexcept
				{

				};
			};

			using TypeNode = QueueNode<QueueType>;

		private:
			//フリーアイテムリスト
			TypeNode* m_QueueAllocArea; //配列先頭のポインタ。
			TypeNode* m_Free; // リンクリストを操作するポインタ。

			//QueueされたNode
			TypeNode* _first;
			TypeNode* _last;

			//設定した最大キュー数
			uint32_t Queue_RoundCount;

			//アトミックブロッククラス。マルチスレッド対応にするための排他制御クラスです。
			SonikLib::S_CAS::SonikAtomicLock shortblock;
			//アロケータ
			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

		private:
			SonikAtomicQueue(const SonikAtomicQueue& _copy_) = delete;
			SonikAtomicQueue(SonikAtomicQueue&& _move_) = delete;
			SonikAtomicQueue& operator =(const SonikAtomicQueue& _copy_) = delete;
			SonikAtomicQueue& operator =(SonikAtomicQueue&& _move_) = delete;

		public:
			inline SonikAtomicQueue(uint32_t QueueItemNum = 100)
				:_first(nullptr)
				, _last(nullptr)
				, Queue_RoundCount(QueueItemNum)
			{

				if (Queue_RoundCount >= UINT32_MAX)
				{
					--Queue_RoundCount;
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

					void* l_allocbuffer = m_allocator->memalArray_Exception(sizeof(TypeNode), Queue_RoundCount);
					//オブジェクトをカウント分ヒープからnewして再利用しまくる。
					m_QueueAllocArea = new(l_allocbuffer) TypeNode[Queue_RoundCount];


				}catch (std::bad_alloc&)
				{
					if( m_allocator.IsNullptr() )
					{
						delete l_alloc_obj;
					};
					throw;
				};

				//リンクをつなげる。
				m_Free = m_QueueAllocArea;
				for (uint32_t i = 0; i < (Queue_RoundCount - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

			};
			inline SonikAtomicQueue(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint32_t QueueItemNum = 100)
			:_first(nullptr)
			, _last(nullptr)
			, Queue_RoundCount(QueueItemNum)
			,m_allocator(_allocator_)
			{

				if (Queue_RoundCount >= UINT32_MAX)
				{
					--Queue_RoundCount;
				};

				try
				{
					void* l_allocbuffer = m_allocator->memalArray_Exception(sizeof(TypeNode), Queue_RoundCount);
					//オブジェクトをカウント分ヒープからnewして再利用しまくる。
					m_QueueAllocArea = new(l_allocbuffer) TypeNode[Queue_RoundCount];


				}
				catch (std::bad_alloc&)
				{
					throw;
				};

				//リンクをつなげる。
				m_Free = m_QueueAllocArea;
				for (uint32_t i = 0; i < (Queue_RoundCount - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

			};

			inline ~SonikAtomicQueue(void)
			{
				//Queue内のオブジェクトの破棄までの責任は持たないので、そのまま配列delete
				if (m_QueueAllocArea != nullptr)
				{
					//m_allocator->CallDestructor_Array(m_QueueAllocArea, Queue_RoundCount);
					m_allocator->memdelArray(m_QueueAllocArea, Queue_RoundCount);
				};

			};

			inline bool Initialize(uint32_t QueueItemMax) noexcept
			{
				if (QueueItemMax >= UINT32_MAX)
				{
					--QueueItemMax;
				};

				//新しい領域を確保
				void* l_allocbuffer = m_allocator->memalArray(sizeof(TypeNode), QueueItemMax);
				if(l_allocbuffer == nullptr)
				{
					return false;
				};

				//元々の領域を破棄。
				if (m_QueueAllocArea != nullptr)
				{
					m_allocator->CallDestructor_Array(m_QueueAllocArea);
					m_allocator->memdelArray(m_QueueAllocArea);

					m_QueueAllocArea = nullptr;
				};

				//オブジェクトをカウント分newして再利用しまくる。
				m_QueueAllocArea = new(l_allocbuffer) TypeNode[QueueItemMax];

				//リンクをつなげる。
				m_Free = m_QueueAllocArea;
				for (uint32_t i = 0; i < (QueueItemMax - 1); ++i)
				{
					m_Free[i].Next = &m_Free[i + 1];
				};

				//変数セットして終了
				Queue_RoundCount = QueueItemMax;
				return true;
			};

			//書き込み(エンキュー)
			inline bool EnQueue(QueueType SetData) noexcept
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

				//nextをnullptrに初期化。
				lp_ptr->Next = nullptr;

				if (_first == nullptr && _last == nullptr)
				{
					//初回ならそれぞれ挿入。
					_first = lp_ptr;
					_last = lp_ptr;
				}
				else
				{
					//すでにオブジェクトがあるなら最後尾に挿入
					//フリーリストから取得。
					_last->Next = lp_ptr;
					_last = lp_ptr;
				};

				//値設定。
				lp_ptr->m_TemplateObject = SetData;

				//ポインタの付替えは終わったのでブロック解除
				shortblock.unlock();

				//終了
				return true;
			};

			//取得（デキュー)
			//TryLock版(ロックが取れなければすぐに処理が返ります。
			inline bool TryDeQueue(QueueType& GetValue) noexcept
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

				if (_first == _last)
				{
					//要素トップのポインタとラストのポインタが一緒なら最終要素のため、lastを更新。
					_last = nullptr;
				}

				_first = _first->Next;

				//DeQueueしたものはフリーブロックへ移動させる。
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

			//取得（デキュー)
			//通常ロック版(ビジーループでロックが取れるまで待ちます。)
			inline bool DeQueue(QueueType& GetValue) noexcept
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

				if (_first == _last)
				{
					//要素トップのポインタとラストのポインタが一緒なら最終要素のため、lastを更新。
					_last = nullptr;
				}

				_first = _first->Next;

				//DeQueueしたものはフリーブロックへ移動させる。
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


#endif /* SONIKATOMICQUEUE_H_ */