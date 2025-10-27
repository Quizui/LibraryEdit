/*
 * SonikRB_RawLinerSerchOperator.h
 *
 *  Created on: 2016/03/11
 *      Author: SONIK
 */

#ifndef SONIKBALANCEDTREE_RB_SONIKRB_RAWLINERSERCHOPERATOR_H_
#define SONIKBALANCEDTREE_RB_SONIKRB_RAWLINERSERCHOPERATOR_H_

#include "SonikRBTreeNode.hpp"

//前方宣言
namespace SonikLib
{
	template <class SetDataType>
	class SonikRBTree;
};

namespace SonikLib
{
	template <class TreeType>
	class SonikLinerOperator_RBTree
	{
		template <class SetDataType>
		friend class SonikLib::SonikRBTree;

	private:
		//SonikBalanceTree::SonikRBTreeNodeRaw<TreeType>* pNodes_;
		SonikLib_Node::SonikRBTreeNodeBase* pNodes_;

	public:
		SonikLinerOperator_RBTree(void)
		:pNodes_(0)
		{

		};

		~SonikLinerOperator_RBTree(void)
		{

		};

		bool NullPtrCheck(void)
		{
			if( pNodes_ == 0 )
			{
				return true;
			};

			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->NodeValue == 0xFFFFFFFF)
			{
				return false;
			}

			return false;
		}

		//現在のアクセスポインタを現在のポインタのNext値にします。
		bool NextMove(void)
		{
			pNodes_ = pNodes_->Next;
			if(pNodes_ == 0)
			{
				return false;
			};

			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->NodeValue == 0xFFFFFFFF)
			{
				return false;
			}


			return true;
		};

		//現在のアクセスポインタを現在のポインタのPrev値にします。
		bool PrevMove(void)
		{
			pNodes_ = pNodes_->Prev;
			if(pNodes_ == 0)
			{
				return false;
			};
			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->Priority == 0xFFFFFFFF)
			{
				return false;
			}

			return true;
		};

		//前置インクリメント オーバーロード
		//NextMove関数をコールするのと同義
		bool operator ++(void)
		{
			pNodes_ = pNodes_->Next;
			if(pNodes_ == 0)
			{
				return false;
			};
			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->Priority == 0xFFFFFFFF)
			{
				return false;
			}

			return true;
		};
		//後置インクリメント オーバーロード
		//NextMove関数をコールするのと同義
		bool operator ++(int n)
		{
			pNodes_ = pNodes_->Next;
			if(pNodes_ == 0)
			{
				return false;
			};
			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->Priority == 0xFFFFFFFF)
			{
				return false;
			}

			return true;
		};

		//前置デクリメント オーバーロード
		//PrevMove関数をコールするのと同義
		bool operator --(void)
		{
			pNodes_ = pNodes_->Prev;
			if(pNodes_ == 0)
			{
				return false;
			};
			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->Priority == 0xFFFFFFFF)
			{
				return false;
			}

			return true;
		};
		//後置デクリメントオーバーロード
		//PrevMove関数をコールするのと同義
		bool operator --(int n)
		{
			pNodes_ = pNodes_->Prev;
			if(pNodes_ == 0)
			{
				return false;
			};
			if(reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->Priority == 0xFFFFFFFF)
			{
				return false;
			}

			return true;
		};

		//比較演算子オーバーロード
		bool operator ==(TreeType& CompareObject)
		{
			if(pNodes_ == 0)
			{
				return false;
			};

			if( reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->m_data == CompareObject )
			{
				return true;
			};

			return false;
		};

		bool operator !=(TreeType& CompareObject)
		{
			if(pNodes_ == 0)
			{
				return false;
			};

			if( reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->m_data != CompareObject )
			{
				return true;
			};

			return false;
		};

		TreeType& operator *(void)
		{
			return reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->m_data;
		};

		//現在のオブジェクトにアクセスします。
		TreeType& operator ->(void)
		{
			return reinterpret_cast<SonikLib_Node::SonikRBTreeNode<TreeType>*>(pNodes_)->m_data;
		};

	};


};



#endif /* SONIKBALANCEDTREE_RB_SONIKRB_RAWLINERSERCHOPERATOR_H_ */
