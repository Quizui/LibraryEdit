/*
 * SonikPriorityListLinearSearch.h
 *
 *  Created on: 2016/03/17
 *      Author: SONIK
 */

#ifndef SONIKPRIORITYLIST_SONIKPRIORITYLISTLINEARSEARCH_H_
#define SONIKPRIORITYLIST_SONIKPRIORITYLISTLINEARSEARCH_H_

#include "../SonikCAS/SonikAtomicLock.h"
#include "SonikPriorityListNode.hpp"

namespace SonikLib
{
	template <class ListType>
	class SonikPriorityList;

};

namespace SonikLib
{
	template <class ListTmplateType>
	class SonikLinerOperator_PriorityList
	{
		template <class ListType>
		friend class SonikPriorityList;

	private:
		//SonikBalanceTree::SonikRBTreeNodeRaw<TreeType>* pNodes_;
		SonikLib_Node::SonikPriorityListNode<ListTmplateType>* pNodes_;
		SonikLib::S_CAS::SonikAtomicLock* m_lock;

	public:
		SonikLinerOperator_PriorityList(void)
		:pNodes_(nullptr)
		,m_lock(nullptr)
		{

		};

		~SonikLinerOperator_PriorityList(void)
		{

		};

		bool NullPtrCheck(void)
		{
			if( pNodes_ == nullptr)
			{
				return true;
			};

			if(pNodes_->Priority == 0xFFFFFFFF )
			{
				return true;
			};

			return false;
		};

		//現在のアクセスポインタを現在のポインタのNext値にします。
		bool NextMove(void)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Next;
			if( pNodes_ == nullptr)
			{
				m_lock->unlock();
				return false;
			};

			if( pNodes_->Priority == 0xFFFFFFFF)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};

		//現在のアクセスポインタを現在のポインタのPrev値にします。
		bool PrevMove(void)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Prev;
			if(pNodes_ == nullptr)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};

		//前置インクリメント オーバーロード
		//NextMove関数をコールするのと同義
		bool operator ++(void)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Next;
			if( pNodes_->Priority == 0xFFFFFFFF || pNodes_ == nullptr)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};
		//後置インクリメント オーバーロード
		//NextMove関数をコールするのと同義
		bool operator ++(int n)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Next;
			if( pNodes_->Priority == 0xFFFFFFFF || pNodes_ == nullptr)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};

		//前置デクリメント オーバーロード
		//PrevMove関数をコールするのと同義
		bool operator --(void)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Prev;
			if(pNodes_ == 0)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};
		//後置デクリメントオーバーロード
		//PrevMove関数をコールするのと同義
		bool operator --(int n)
		{
			m_lock->lock();

			pNodes_ = pNodes_->Prev;
			if(pNodes_ == nullptr)
			{
				m_lock->unlock();
				return false;
			};

			m_lock->unlock();
			return true;
		};

		//比較演算子オーバーロード
		bool operator ==(ListTmplateType& CompareObject)
		{
			if( pNodes_ == nullptr || pNodes_->Priority == 0xFFFFFFFF)
			{
				return false;
			};

			if( pNodes_->m_data == CompareObject )
			{
				return true;
			};

			return false;
		};

		bool operator !=(ListTmplateType& CompareObject)
		{
			if( pNodes_ == nullptr || pNodes_->Priority == 0xFFFFFFFF)
			{
				return false;
			};

			if( pNodes_->m_data != CompareObject )
			{
				return true;
			};

			return false;
		};

		ListTmplateType GetOperatorValue(void)
		{
			return pNodes_->m_data;
		};

		//現在のオブジェクトにアクセスします。
		ListTmplateType& operator ->(void)
		{
			return pNodes_->m_data;
		};

	};

};//end namespace SonikLib



#endif /* SONIKPRIORITYLIST_SONIKPRIORITYLISTLINEARSEARCH_H_ */
