/*
 * SonikPriorityListNode.h
 *
 *  Created on: 2016/03/15
 *      Author: SONIK
 */

#ifndef SONIKPRIORITYLIST_SONIKPRIORITYLISTNODE_H_
#define SONIKPRIORITYLIST_SONIKPRIORITYLISTNODE_H_

#include <stdint.h>

namespace SonikLib
{
	template <class ListType>
	class SonikLinerOperator_PriorityList;
};

namespace SonikLib
{
	template <class ListType>
	class SonikPriorityList;
};
//前方宣言 END=======================

namespace SonikLib_Node
{
	template <class NodeType>
	class SonikPriorityListNode
	{
		template <class ListType>
		friend class SonikLib::SonikLinerOperator_PriorityList;

		template <class ListType>
		friend class SonikLib::SonikPriorityList;

	private:
		NodeType m_data;
		SonikPriorityListNode<NodeType>* Next;
		SonikPriorityListNode<NodeType>* Prev;

		uint32_t Priority;

		//番兵フラグ
		bool Sentinel_;

	public:
		SonikPriorityListNode(void)
		:Next(nullptr)
		,Prev(nullptr)
		,Priority(0xFFFFFFFF)
		,Sentinel_(true)
		{

		};

		~SonikPriorityListNode(void)
		{

		};

	};

	template <class Type>
	class S_Inner_PListQSortNode
	{
	public:
		SonikPriorityListNode<Type>* Point;
		uint32_t PosNum;
	};

}; // end namespace SonikLib



#endif /* SONIKPRIORITYLIST_SONIKPRIORITYLISTNODE_H_ */
