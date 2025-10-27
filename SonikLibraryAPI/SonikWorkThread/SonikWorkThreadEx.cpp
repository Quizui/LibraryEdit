
#include "SonikWorkThreadEx.h"

#include "../FunctionObject/FunctionObjectSystemInterface.hpp"
#include "../SonikCAS/SonikAtomicLock.h"
#include "../Container/SonikAtomicQueue.hpp"

#include <thread>
#include <new>
#include <condition_variable>

namespace SonikLib
{

	//クラス定義=====================================
	//WorkThred本体
	class WorkThreadEx::pImplEx
	{
	private:
		//スレッドオブジェクト
		std::thread threads_;

		//実際にコールする関数オブジェクト
		SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> FuncObj_;


		//関数パックのキューオブジェクトへのポインタ
		SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>> FuncQueue_;

		//CASロックオブジェクト
		SonikLib::S_CAS::SonikAtomicLock atmlock_;

		//静的関数で使う、条件変数
		SonikLib::SharedSmtPtr<std::condition_variable_any> cond_;

		//1ビット目(0x01): 静的関数終了フラグ
		//2ビット目(0x02): Queueセット時のセット関数終了フラグの固定フラグ。(0x04ビットを常に立ったままにまします。)
		//3ビット目(0x04): セット関数の終了フラグ(途中でセット内容を変えたい場合に使用。)
		//		       					ただし、セットしてある関数が終了しないと本フラグは意味をなさない。
		//4ビット目(0x08): デキュー停止フラグ。キューのアンセットに使用します。
		//5ビット目(0x10): スレッドが処理するジョブがなくて休止状態であるかどうか。
		//6ビット目(0x20): 各フラグのセット通知(一括変更通知でないと特定タイミングでフラグが無視されるため)
		//31ビット目(0x80000000): 静的関数完了フラグ(静的関数がreturnするときのフラグ。完全終了告知)
		uint32_t ThreadFlag;

		bool DetachFlag;

	private:
		static void SonikWorkThreadMainEx(WorkThreadEx::pImplEx* ClassObject);

	public:
		//コンストラクタです。
		pImplEx(SonikLib::SharedSmtPtr<std::condition_variable_any>& _cond_, bool DetachThread);

		//デストラクタ
		~pImplEx(void);

		//コールする関数オブジェクトをセットします。
		bool SetCallFunction(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& CallFunctionObject, bool _looped_ = false);

		//静的関数内で使用
		SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& GetFunctionPointer(void);
		SonikLib::SharedSmtPtr<std::condition_variable_any>& GetConditionVariable(void);
		SonikLib::S_CAS::SonikAtomicLock& GetCASLockObject(void);
		uint32_t& GetThreadFlag(void);
		//キューのあんせっと
		void InnerUnsetQueue(void);



		//キューポインタが設定されていればデキューを行います。
		void UpdateQueue(void);

		//外部からコールする関数
		//スレッド終了フラグのセット
		void SetThreadExitFlag(bool Setflag);
		//セット関数の終了フラグ(途中でセット内容を変えたい場合に使用。)
		void SetChangeSetFuncFlag(bool Setflag);
		//キューアンセットフラグのセット
		void SetQueueUnsetFlag(bool Setflag);
		//サスペンド状態かどうかのフラグセット
		void SetThreadStatus_Suspend(bool Setfalg);

		//キューポインタをセットします。
		void SetFunctionQueue(SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>& pSetQueue);
		//キューポインタをアンセットします。
		void UnSetFunctionQueue(void);

	};

	//静的メソッドの定義
	void WorkThreadEx::pImplEx::SonikWorkThreadMainEx(WorkThreadEx::pImplEx* ClassObject)
	{
		uint32_t& RefFlag = ClassObject->GetThreadFlag();
		SonikLib::SharedSmtPtr<std::condition_variable_any>& RefCond = ClassObject->GetConditionVariable();
		SonikLib::S_CAS::SonikAtomicLock& RefLock = ClassObject->GetCASLockObject();
		std::mutex localmtx;
		SonikLib::SharedSmtPtr<SonikFOSInterface>& RefFuncObj = ClassObject->GetFunctionPointer();
		SonikLib::SharedSmtPtr<SonikFOSInterface> RunTask;
		SonikLib::SharedSmtPtr<SonikFOSInterface> StrongTask;

		while (1)
		{
			if (RunTask.IsNullptr())
			{
				//condition_variable_anyのwaitで使用。
				std::unique_lock<std::mutex> lock(localmtx);
				//RefFuncが0以外になればロック解除。
				//ラムダ式使用。
				RefCond->wait(lock,
					[&RefFuncObj, &RunTask, &ClassObject, &RefFlag]
					{
						//ThreadMainEx関数終了フラグか、デキュー停止フラグが立っていればサスペンド解除し、後続の処理へ。。
						if (((RefFlag & 0x08) != 0) || ((RefFlag & 0x01) != 0))
						{
							ClassObject->SetThreadStatus_Suspend(false);
							return true;
						};

						//関数(タスク)取得
						ClassObject->UpdateQueue();

						//取れてたらサスペンド解除して後続処理へ。
						if (!RefFuncObj.IsNullptr())
						{
							RunTask = RefFuncObj;
							ClassObject->SetThreadStatus_Suspend(false);
							return true;
						};

						//取れなければサスペンド状態にして再度最初からチェック。
						ClassObject->SetThreadStatus_Suspend(true);
						return false;
					}
					);
			};

			if ((RefFlag & 0x08) != 0)
			{
				ClassObject->InnerUnsetQueue();
				//関数終了フラグも立っていればそのままbreak処理。
				if ((RefFlag & 0x01) != 0)
				{
					break; //while(1) break;
				};
				continue;
			};

			//関数終了なら総合処理へ。
			if ((RefFlag & 0x01) != 0)
			{
				break;
			};

			//指定された関数コール。
			RunTask->Run();

			//stribg_next loop でRunを実施
			StrongTask = RunTask->GetNext_strong();
			while (!StrongTask.IsNullptr())
			{
				StrongTask->Run();
				StrongTask = StrongTask->GetNext_strong();
			};

			//weak_nextポインタをセットして次のタスクへ
			RunTask = RunTask->GetNext_weak();
			if (!RunTask.IsNullptr())
			{
				continue;
			};

			/*ファンクションの変更フラグが立っていれば、再生終了後、保持してるポインタ値を破棄*/
			//Queue固定時の特殊フラグが立っていてもポインタ値を破棄。
			if ((RefFlag & 0x04) != 0 || (RefFlag & 0x02) != 0)
			{
				RefFuncObj = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>(); //nullptr 代入
				RefFlag &= (~0x04);
			};

			//変更フラグが立っていなければ、RefFuncを再度代入して最初からスタート
			//もし変更フラグが立っていればif文にて、nullptrのRefFuncがセットされる。
			RunTask = RefFuncObj;

			RefLock.unlock();

		};

		//総合終了===================
		//RefFuncObj.ResetPointer(nullptr);
		RefFuncObj = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>(); //nullptr 代入
		RefLock.unlock();
		RefFlag = 0x80000000; //31ビット目のみ立てる。
	};

	//クラス実装=============================================
	WorkThreadEx::pImplEx::pImplEx(SonikLib::SharedSmtPtr<std::condition_variable_any>& _cond_, bool DetachThread)
		:threads_(&WorkThreadEx::pImplEx::SonikWorkThreadMainEx, this)
		, FuncQueue_()
		, cond_(_cond_)
		, ThreadFlag(0)
		, DetachFlag(DetachThread)
	{
		if (DetachThread)
		{
			threads_.detach();
		};
	};

	//デストラクタ
	WorkThreadEx::pImplEx::~pImplEx(void)
	{
		SetThreadExitFlag(true);

		//完全終了フラグが立つまでひたすら起こしまくる。(たまにデッドロックするときの処置)
		while (((ThreadFlag & 0x80000000) == 0))
		{
			cond_->notify_all();
		};

		if (!DetachFlag)
		{
			threads_.join();
		};

	};

	//コールする関数オブジェクトをセットします。
	//本関数は成功した場合、スレッドにセットされた関数が終了するまで、再セットは行いません。
	bool WorkThreadEx::pImplEx::SetCallFunction(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& CallFunctionObject, bool _looped_)
	{
		if (!atmlock_.try_lock())
		{
			return false;
		};

		if (!FuncQueue_.IsNullptr())
		{
			return false;
		};

		if (CallFunctionObject.IsNullptr())
		{
			return false;
		};

		//セット
		FuncObj_ = CallFunctionObject;
		SetChangeSetFuncFlag(_looped_);
		cond_->notify_all();
		//ミューテックスのアンロックは静的関数内で行う。
		return true;
	};

	SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& WorkThreadEx::pImplEx::GetFunctionPointer(void)
	{
		return FuncObj_;
	};

	SonikLib::SharedSmtPtr<std::condition_variable_any>& WorkThreadEx::pImplEx::GetConditionVariable(void)
	{
		return cond_;
	};

	SonikLib::S_CAS::SonikAtomicLock& WorkThreadEx::pImplEx::GetCASLockObject(void)
	{
		return atmlock_;
	};

	uint32_t& WorkThreadEx::pImplEx::GetThreadFlag(void)
	{
		return ThreadFlag;
	};

	//キューポインタが設定されていればデキューを行います。
	void WorkThreadEx::pImplEx::UpdateQueue(void)
	{
		if (FuncQueue_.IsNullptr())
		{
			return;
		};

		FuncQueue_->DeQueue(FuncObj_);
	};

	//スレッド終了フラグのセット
	void WorkThreadEx::pImplEx::SetThreadExitFlag(bool Setflag)
	{
		(Setflag == true) ? (ThreadFlag = ThreadFlag | 0x01) : (ThreadFlag = ThreadFlag & (~0x01));

	};

	//セット関数の終了フラグ(途中でセット内容を変えたい場合に使用。)
	void WorkThreadEx::pImplEx::SetChangeSetFuncFlag(bool Setflag)
	{
		(Setflag == true) ? (ThreadFlag = ThreadFlag | 0x04) : (ThreadFlag = ThreadFlag & (~0x04));
	};

	//キューアンセットフラグのセット
	void WorkThreadEx::pImplEx::SetQueueUnsetFlag(bool Setflag)
	{
		(Setflag == true) ? (ThreadFlag = ThreadFlag | 0x08) : (ThreadFlag = ThreadFlag & (~0x08));
	};

	//サスペンド状態かどうかのフラグセット
	void WorkThreadEx::pImplEx::SetThreadStatus_Suspend(bool Setflag)
	{
		(Setflag == true) ? (ThreadFlag = ThreadFlag | 0x10) : (ThreadFlag = ThreadFlag & (~0x10));
	};


	//キューのあんせっと
	void WorkThreadEx::pImplEx::InnerUnsetQueue(void)
	{
		//nullptr初期化
		//FuncQueue_.ResetPointer(nullptr);
		FuncQueue_ = SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>();
		ThreadFlag &= (~0x02);
		SetQueueUnsetFlag(false);
	};

	//キューポインタをセットします。
	void WorkThreadEx::pImplEx::SetFunctionQueue(SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>& pSetQueue)
	{
		atmlock_.lock();

		FuncQueue_ = pSetQueue;
		SetChangeSetFuncFlag(false);
		ThreadFlag |= 0x02;

		atmlock_.unlock();
	};

	//キューポインタをアンセットします。
	void WorkThreadEx::pImplEx::UnSetFunctionQueue(void)
	{
		ThreadFlag |= 0x08;

		while ((ThreadFlag & 0x08) != 0)
		{
			cond_->notify_all();
		};

	};


};

namespace SonikLib
{

	//============================================================================================
	//Implクラスと外部をつなぐクラス実装
	//============================================================================================
	//本クラスのコンストラクタです。
	WorkThreadEx::WorkThreadEx(bool DetachThread)
	:ImplObject(nullptr)
	{
		std::condition_variable_any* lp_cond = nullptr;
		SonikLib::SLibAllocateInterface* l_defalloc;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			SonikLib::SharedSmtPtr<std::condition_variable_any> l_cond;

			void* l_allocbuffer = l_defalloc->memal(sizeof(std::condition_variable_any));
			if(l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			lp_cond = new(l_allocbuffer) std::condition_variable_any;
			if (!SonikLib::SharedSmtPtr<std::condition_variable_any>::SmartPointerCreate(lp_cond, l_cond, m_allocator))
			{
				lp_cond->~condition_variable_any();
				l_defalloc->memdel(lp_cond);
				throw std::bad_alloc();
			};

			l_allocbuffer = l_defalloc->memal(sizeof(WorkThreadEx::pImplEx));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			ImplObject = new(l_allocbuffer) WorkThreadEx::pImplEx(l_cond, DetachThread);

		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	WorkThreadEx::WorkThreadEx(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, bool DetachThread)
	:ImplObject(nullptr)
	,m_allocator(_allocator_)
	{
		std::condition_variable_any* lp_cond = nullptr;
		try
		{
			SonikLib::SharedSmtPtr<std::condition_variable_any> l_cond;

			void* l_allocbuffer = _allocator_->memal(sizeof(std::condition_variable_any));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			lp_cond = new(l_allocbuffer) std::condition_variable_any;
			if (!SonikLib::SharedSmtPtr<std::condition_variable_any>::SmartPointerCreate(lp_cond, l_cond, _allocator_))
			{
				lp_cond->~condition_variable_any();
				_allocator_->memdel(lp_cond);
				throw std::bad_alloc();
			};

			l_allocbuffer = _allocator_->memal(sizeof(WorkThreadEx::pImplEx));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			ImplObject = new(l_allocbuffer) WorkThreadEx::pImplEx(l_cond, DetachThread);

		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};

	WorkThreadEx::WorkThreadEx(SonikLib::SharedSmtPtr<std::condition_variable_any>& _cond_, bool DetachThread)
	:ImplObject(nullptr)
	{
		SonikLib::SLibAllocateInterface* l_defalloc;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(WorkThreadEx::pImplEx));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			ImplObject = new(l_allocbuffer) WorkThreadEx::pImplEx(_cond_, DetachThread);

		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};
	WorkThreadEx::WorkThreadEx(SonikLib::SharedSmtPtr<std::condition_variable_any>& _cond_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, bool DetachThread)
	:ImplObject(nullptr)
	,m_allocator(_allocator_)
	{
		try
		{
			void* l_allocbuffer = _allocator_->memal(sizeof(WorkThreadEx::pImplEx));
			if (l_allocbuffer == nullptr)
			{
				throw std::bad_alloc();
			};

			ImplObject = new(l_allocbuffer) WorkThreadEx::pImplEx(_cond_, DetachThread);

		}
		catch (std::bad_alloc&)
		{
			throw;
		};
	};


	//本クラスのデストラクタです。
	WorkThreadEx::~WorkThreadEx(void)
	{
		if (ImplObject != 0)
		{
			ImplObject->~pImplEx();
			m_allocator->memdel(ImplObject);

			ImplObject = nullptr;
		};

	};

	//関数オブジェクトポインタをセットします。
	//引数1: スレッドで実行する関数オブジェクトを設定します。
	//引数2: 実行関数の実行終了後、再度繰り返し実行させる場合はtrueを指定します。(default = false, １回コールのみ。)
	//			  この引数をtrueにした状態のままで関数
	//戻り値; スレッドが実行中の場合はfalseが返り、セットされません。
	//本関数はスレッドセーフです。
	//確実にセットしたい場合、前にセットされた関数があれば、それが終了し、関数がtrueを返却するまでループします。
	//別途QUEUEがセットされている場合、この関数は必ずfalseを返却します。
	//マルチスレッドにより、同時にキューセットと本関数が呼ばれた場合で、本関数が先にコールされた場合、本関数は、trueを返却します。
	bool WorkThreadEx::SetCallFunction(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> CallFunctionObject, bool _looped_)
	{
		bool l_looped_ = _looped_;
		return ImplObject->SetCallFunction(CallFunctionObject, (!l_looped_));
	};

	//外部のキューをセットします。
	//本関数はSetCallFunctionと同時にコールされた場合で、SetCallFunctionが先に実行された場合、セットされた関数が終了するまで処理を返却しません。
	//本関数によりキューがセットされた後は、SetCallFunctionは無効となり、常にfalseを返却します。
	//本関数でセットしたキューにエンキューを行った場合、dispatchQueue関数をコールし、エンキューを行ったことを通知しなければデキュー処理を行いません。
	void WorkThreadEx::Set_ExternalQueue(SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>& pSetQueue)
	{
		ImplObject->SetFunctionQueue(pSetQueue);
	};

	//外部のキューをアンセットします。
	void WorkThreadEx::UnSet_ExternalQueue(void)
	{
		ImplObject->UnSetFunctionQueue();
	};

	//スレッドにデキューの開始を通知します。
	void WorkThreadEx::dispatchDeQueue(void)
	{
		ImplObject->GetConditionVariable()->notify_all();
	};

	//スレッド実行中に設定を変更したい場合に使う関数群========
	//関数コールのループフラグのON/OFFへの設定を行います。 SetCallFunction（）の設定をtrueにし、後に変更したくなった場合に使用します。
	//現在のファンクションが終了し、判定箇所に来た場合に実行されます。
	//設定関数実行後、判定箇所をスレッドが通過済みの場合、次の設定関数コール終了後に判定が行われます。
	//Queueがセットされている場合はこの設定は常にOff状態となります。
	//また、Queueをアンセットされる際にはOffの状態のままとなりますので、Onに戻したい場合はOnをコールしてください。
	void WorkThreadEx::SetFunctionloopEndFlagOn(void)
	{

		ImplObject->SetChangeSetFuncFlag(true);
	};
	void WorkThreadEx::SetFunctionloopEndFlagOff(void)
	{
		//立ってると終了。
		ImplObject->SetChangeSetFuncFlag(false);
	};


	//スレッドの終了フラグをOnにします。
	//これはSetFunctionDeleteFlagOn()関数と同様の判定方法を行います。
	//よって、即時に終了されるわけではありません。
	void WorkThreadEx::SetThreadExitFlagOn(void)
	{
		ImplObject->SetThreadExitFlag(true);
	};

	//現在のスレッドがサスペンド状態にあるかどうかを取得します。
	//稼働中であればfalse, サスペンド状態であればTrueを返却します。
	bool WorkThreadEx::GetThreadStatusSuspend(void)
	{
		if ((ImplObject->GetThreadFlag() & (0x10)) != 0)
		{
			return true;
		};

		return false;
	};




}; //end namespace