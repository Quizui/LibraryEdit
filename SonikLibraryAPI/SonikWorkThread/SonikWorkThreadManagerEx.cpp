
#include "SonikWorkThreadManagerEx.h"
#include "SonikWorkThreadEx.h"

#include "../Container/SonikAtomicQueue.hpp"
#include "../FunctionObject/FunctionObjectSystemInterface.hpp"

#include <new>
#include <condition_variable>

namespace SonikLib
{
	//コンストラクタ
	SonikThreadManagerEx::SonikThreadManagerEx(void)
	:ManagedThreadNum(0)
	,m_pThreads(nullptr)
	{

	};

	//デストラクタ
	SonikThreadManagerEx::~SonikThreadManagerEx(void)
	{
		//スレッド始末。
		if (m_pThreads != nullptr)
		{
			SonikLib::WorkThreadEx* lp_control = m_pThreads;
			for (uint32_t i = 0; i < ManagedThreadNum; ++i)
			{
				lp_control->~WorkThreadEx();
				++lp_control;
			};

			m_allocator->memdel(m_pThreads);
			m_pThreads = nullptr;
			ManagedThreadNum = 0;
		};

		m_cond.~SharedSmtPtr();
		JobQueue.~SharedSmtPtr();
		m_allocator.~AllocatorSharedSmtPtr();
	};

	//クリエイタ
	bool SonikThreadManagerEx::CreateThraedManager(SonikLib::SharedSmtPtr<SonikThreadManagerEx> _out_mng_, uint32_t UseThreadNum, uint32_t JobBufferSize)
	{
		SonikLib::SLibAllocateInterface* l_defalloc = new(std::nothrow) SonikLib::SLibAllocateInterface;
		if (l_defalloc == nullptr)
		{
			return false;
		};
		
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
		if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_defalloc, l_allocSmtPtr))
		{
			delete l_defalloc;
			return false;
		};

		void* l_allocbuffer = l_defalloc->memal(sizeof(SonikThreadManagerEx));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		SonikThreadManagerEx* lp_mngobj = new(l_allocbuffer) SonikThreadManagerEx;
		lp_mngobj->m_allocator = l_allocSmtPtr;

		l_allocbuffer = l_defalloc->memal(sizeof(std::condition_variable_any));
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_mngobj);
			return false;
		};

		std::condition_variable_any* lp_cond = new(l_allocbuffer) std::condition_variable_any;

		if (!SonikLib::SharedSmtPtr<std::condition_variable_any>::SmartPointerCreate(lp_cond, lp_mngobj->m_cond, l_allocSmtPtr))
		{
			lp_cond->~condition_variable_any();
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_cond); //スマートポインタに格納できていないためここでmemdelする必要がある。ここ以降はクラス内デストラクタでスマートポインタにより破棄される。
			l_defalloc->memdel(lp_mngobj);
			return false;
		};

		l_allocbuffer = l_defalloc->memal(sizeof(SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>));
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_mngobj);
			return false;
		};

		SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>* lp_jobqueue = new(l_allocbuffer) SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>(l_allocSmtPtr, JobBufferSize);

		if (!SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>::SmartPointerCreate(lp_jobqueue, lp_mngobj->JobQueue, l_allocSmtPtr))
		{
			lp_jobqueue->~SonikAtomicQueue();
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_jobqueue); //スマートポインタに格納できていないためここでmemdelする必要がある。ここ以降はクラス内デストラクタでスマートポインタにより破棄される。
			l_defalloc->memdel(lp_mngobj);
			return false;
		};

		//スレッド生成(開放とか含めちょっと複雑)
		l_allocbuffer = l_defalloc->memal(sizeof(SonikLib::WorkThreadEx) * UseThreadNum);
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_mngobj);
			return false;
		};

		SonikLib::WorkThreadEx* lp_controlPointer = reinterpret_cast<SonikLib::WorkThreadEx*>(l_allocbuffer);
		lp_mngobj->m_pThreads = lp_controlPointer;

		for (uint32_t i = 0; i < UseThreadNum; ++i)
		{
			if ( (new(lp_controlPointer)  SonikLib::WorkThreadEx(lp_mngobj->m_cond, l_allocSmtPtr) ) == nullptr)
			{
				lp_controlPointer = lp_mngobj->m_pThreads;
				for (uint32_t j = 0; j < i; ++j)
				{
					lp_controlPointer->~WorkThreadEx();
				};

				l_defalloc->memdel(lp_mngobj->m_pThreads);
				lp_mngobj->m_pThreads = nullptr;
				lp_mngobj->~SonikThreadManagerEx();
				l_defalloc->memdel(lp_mngobj);
				return false;
			};

			lp_controlPointer->Set_ExternalQueue(lp_mngobj->JobQueue);
			++lp_controlPointer;
		};

		lp_mngobj->ManagedThreadNum = UseThreadNum;

		//スレッドプールマネージャークラス本体をスマートポインタへ
		if (!SonikLib::SharedSmtPtr<SonikThreadManagerEx>::SmartPointerCreate(lp_mngobj, _out_mng_, l_allocSmtPtr))
		{
			lp_mngobj->~SonikThreadManagerEx();
			l_defalloc->memdel(lp_mngobj);
			return false;
		}

		return true;
	};
	bool SonikThreadManagerEx::CreateThraedManager(SonikLib::SharedSmtPtr<SonikThreadManagerEx> _out_mng_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint32_t UseThreadNum, uint32_t JobBufferSize)
	{
		void* l_allocbuffer = _allocator_->memal(sizeof(SonikThreadManagerEx));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};

		SonikThreadManagerEx* lp_mngobj = new(l_allocbuffer) SonikThreadManagerEx;
		lp_mngobj->m_allocator = _allocator_;

		l_allocbuffer = _allocator_->memal(sizeof(std::condition_variable_any));
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_ ->memdel(lp_mngobj);
			return false;
		};

		std::condition_variable_any* lp_cond = new(l_allocbuffer) std::condition_variable_any;

		if (!SonikLib::SharedSmtPtr<std::condition_variable_any>::SmartPointerCreate(lp_cond, lp_mngobj->m_cond, _allocator_))
		{
			lp_cond->~condition_variable_any();
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_->memdel(lp_cond); //スマートポインタに格納できていないためここでmemdelする必要がある。ここ以降はクラス内デストラクタでスマートポインタにより破棄される。
			_allocator_->memdel(lp_mngobj);
			return false;
		};

		l_allocbuffer = _allocator_->memal(sizeof(SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>));
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_->memdel(lp_mngobj);
			return false;
		};

		SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>* lp_jobqueue = new(l_allocbuffer) SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>(_allocator_, JobBufferSize);

		if (!SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>::SmartPointerCreate(lp_jobqueue, lp_mngobj->JobQueue, _allocator_))
		{
			lp_jobqueue->~SonikAtomicQueue();
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_->memdel(lp_jobqueue); //スマートポインタに格納できていないためここでmemdelする必要がある。ここ以降はクラス内デストラクタでスマートポインタにより破棄される。
			_allocator_->memdel(lp_mngobj);
			return false;
		};

		//スレッド生成(開放とか含めちょっと複雑)
		l_allocbuffer = _allocator_->memal(sizeof(SonikLib::WorkThreadEx) * UseThreadNum);
		if (l_allocbuffer == nullptr)
		{
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_->memdel(lp_mngobj);
			return false;
		};

		SonikLib::WorkThreadEx* lp_controlPointer = reinterpret_cast<SonikLib::WorkThreadEx*>(l_allocbuffer);
		lp_mngobj->m_pThreads = lp_controlPointer;

		for (uint32_t i = 0; i < UseThreadNum; ++i)
		{
			if ((new(lp_controlPointer)  SonikLib::WorkThreadEx(lp_mngobj->m_cond, _allocator_)) == nullptr)
			{
				lp_controlPointer = lp_mngobj->m_pThreads;
				for (uint32_t j = 0; j < i; ++j)
				{
					lp_controlPointer->~WorkThreadEx();
				};

				_allocator_->memdel(lp_mngobj->m_pThreads);
				lp_mngobj->m_pThreads = nullptr;
				lp_mngobj->~SonikThreadManagerEx();
				_allocator_->memdel(lp_mngobj);
				return false;
			};

			lp_controlPointer->Set_ExternalQueue(lp_mngobj->JobQueue);
			++lp_controlPointer;
		};

		lp_mngobj->ManagedThreadNum = UseThreadNum;

		//スレッドプールマネージャークラス本体をスマートポインタへ
		if (!SonikLib::SharedSmtPtr<SonikThreadManagerEx>::SmartPointerCreate(lp_mngobj, _out_mng_, _allocator_))
		{
			lp_mngobj->~SonikThreadManagerEx();
			_allocator_->memdel(lp_mngobj);
			return false;
		}

		return true;
	};


	//スレッド数の取得
	uint32_t SonikThreadManagerEx::Get_ManagedThreadNum(void)
	{
		return ManagedThreadNum;
	};

	//タスクセット
	bool SonikThreadManagerEx::EnqueueJob(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& _in_task_)
	{
		return  JobQueue->EnQueue(_in_task_);
	};
	//タスクゲット（余り時間処理参加用)
	bool SonikThreadManagerEx::DequeueJob(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& _out_task_)
	{
		return JobQueue->DeQueue(_out_task_);
	};

};//end namespace;