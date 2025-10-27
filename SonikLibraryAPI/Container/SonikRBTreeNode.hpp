/*
 * SonikRBTreeNodeRaw.h
 *
 *  Created on: 2016/03/12
 *      Author: SONIK
 */

#ifndef SONIKBALANCEDTREE_RB_SONIKRBTREENODE_H_
#define SONIKBALANCEDTREE_RB_SONIKRBTREENODE_H_

//前方宣言
namespace SonikLib
{
	template <class TreeType>
	class SonikLinerOperator_RBTree;

	template <class SetDataType>
	class SonikRBTree;
};



namespace SonikLib_Node
{

	//ノードベース
	class SonikRBTreeNodeBase
	{
		template <class SetDataType>
		friend class SonikLib::SonikRBTree;

		template <class TreeType>
		friend class SonikLib::SonikLinerOperator_RBTree;

	public:
		//色 列挙
		typedef enum _S_TREE_COLOR
		{
			TreeBlack = 0,
			TreeRed,
		}SonikTreeColor;

	protected:
		//本ノードの色
		unsigned long colors;
		//根であるか？
		bool root_;
		//ノードの値
		unsigned long NodeValue;

		//線形操作用の相互ノードポインタ
		SonikRBTreeNodeBase* Next;
		SonikRBTreeNodeBase* Prev;

		//自分の前ノード
		SonikRBTreeNodeBase* Parent;
		//左の子ノード
		SonikRBTreeNodeBase* left_;
		//右の子ノード
		SonikRBTreeNodeBase* right_;

	public:
		SonikRBTreeNodeBase(void)
	:colors(TreeBlack)
	,root_(false)
	,NodeValue(0)
	,Next(0)
	,Prev(0)
	,Parent(0)
	,left_(0)
	,right_(0)
	{

	};

		virtual ~SonikRBTreeNodeBase(void)
		{

		};

		SonikTreeColor GetLeftColor(void)
		{
			if( left_ != 0 )
			{
				return static_cast<SonikTreeColor>(left_->colors);
			};

			return TreeBlack;
		};

		SonikTreeColor GetRightColor(void)
		{
			if( right_ != 0 )
			{
				return static_cast<SonikTreeColor>(right_->colors);
			};

			return TreeBlack;
		};


	};

	//ノード本体
	template<class DataValueType>
	class SonikRBTreeNode : public SonikRBTreeNodeBase
	{
		template <class SetDataType>
		friend class SonikLib::SonikRBTree;

		template <class TreeType>
		friend class SonikLib::SonikLinerOperator_RBTree;

	private:
		//ノードが持っている外部入力データ
		DataValueType m_data;

	public:
		SonikRBTreeNode(void)
		{

		};

		~SonikRBTreeNode(void)
		{
			m_data.~DataValueType();

		};

	};

};



#endif /* SONIKBALANCEDTREE_RB_SONIKRBTREENODE_H_ */
