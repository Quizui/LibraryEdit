//C++11実装
#include "SonikWorkThreadEx.h"

#include <FunctionObject/FunctionObjectSystemInterface.hpp>
#include <SonikCAS/SonikAtomicLock.h>
#include <Container/SonikAtomicQueue.hpp>
#include <SonikWorkThread/SonikThreadWaitingObject.hpp>
#include <PlatFormDefinitions.h>
#include <CPPGrammarDefines.h>
#include <CompilersPreProcesser.h>

#include <new>

//利用スレッドアーキテクチャ切り替えdefine
#if defined(SLIB_PLATFORM_DEFS_WINDOWS)
	// Windows環境

	// MSVCコンパイラの場合
	#if defined(SLIB_COMPILER_DEF_MSVC)
		#if (_MSC_VER >= 1900) 
			// VS2015(1900)以降なら std::thread が安定して使える
			#define __USE_THREAD_ARCHITECT_STD__
		#else
			// それ未満なら Windows CRT (_beginthreadex)
			#define __USE_THREAD_ARCHITECT_WINDOWS_CRT__
		#endif

	#else 
		// MSVC以外のコンパイラ（MinGW/Clang等）の場合
		#if (__cplusplus >= 201103L)
			// C++11以上なら std::thread
			#define __USE_THREAD_ARCHITECT_STD__
		#else
			// C++11未満なら、MinGW等でも pthread_create 等を検討
			#define __USE_THREAD_ARCHITECT_POSIX__ 
		#endif
	#endif

#elif defined(SLIB_PLATFORM_DEFS_MAC) || defined(SLIB_PLATFORM_DEFS_POSIX)
	// MAC or POSIX環境
	#if (__cplusplus >= 201103L)
		//C++11以上ならstd
		#define __USE_THREAD_ARCHITECT_STD__
	#else
		//以下ならPOSIX
		#define __USE_THREAD_ARCHITECT_POSIX__
	#endif

#else
	//最終フォールバック
	//さしあたりstd::thread
	#define __USE_THREAD_ARCHITECT_STD__


#endif

//スレッド利用ヘッダインクルード
#if defined(__USE_THREAD_ARCHITECT_STD__)

//使用アーキテクチャ：STD
#include <thread>

#elif defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__)

//使用アーキテクチャ：CRT(C Runtime Library)
#include <process.h>
#include <windows.h>

#define THREAD_WRAPPER_FUNCTYPE unsigned int __stdcall

#elif defined(__USE_THREAD_ARCHITECT_POSIX__)

//使用アーキテクチャ：POSIX Thread
#include <pthread.h>
#define THREAD_WRAPPER_FUNCTYPE  void*

#endif



namespace SonikLib
{

	//クラス定義=====================================
	//WorkThred本体
	class WorkThreadEx::pImplEx
	{
	private:
		//スレッドオブジェクトの定義(環境によって切り替え)
#if defined(__USE_THREAD_ARCHITECT_STD__) 			//使用アーキテクチャ：STD
		SLIB_CVR_USING(SLibThreadHandle, std::thread);

#elif defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__)	//使用アーキテクチャ：CRT(C Runtime Library)
		SLIB_CVR_USING(SLibThreadHandle, HANDLE);

#elif defined(__USE_THREAD_ARCHITECT_POSIX__)		//使用アーキテクチャ：POSIX Thread
		SLIB_CVR_USING(SLibThreadHandle, pthread_t);

#endif
		//スレッドオブジェクト
		SLibThreadHandle threads_;

		//実際にコールする関数オブジェクト
		SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> FuncObj_;


		//関数パックのキューオブジェクトへのポインタ
		SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>> FuncQueue_;

		//CASロックオブジェクト
		SonikLib::S_CAS::SonikAtomicLock atmlock_;

		//静的関数で使う、条件変数
		SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>> cond_;

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
		//スレッドのメイン関数。ここでタスクの実行とタスク待ちが行われる。
		DEF_FORCE_INLINE static void SonikWorkThreadMainEx(WorkThreadEx::pImplEx* ClassObject);
		//WindowsやPOSIXのOSスレッドを使う時だけコール規則や戻り値が違うのでラッパ関数経由でコールしないとクラッシュする危険性があるため定義
#if defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__) || defined(__USE_THREAD_ARCHITECT_POSIX__) //OSアーキテクチャ使用が有効ならラッパ関数定義

		static THREAD_WRAPPER_FUNCTYPE ThreadMain_Wrapper(void* p); //関数ポインタ経由でCRTからコールされるためinlineしない。しても意味がない。

#endif

	public:
		//コンストラクタです。
		pImplEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& _cond_, bool DetachThread);

		//デストラクタ
		~pImplEx(void);

		//コールする関数オブジェクトをセットします。
		bool SetCallFunction(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& CallFunctionObject, bool _looped_ = false);

		//静的関数内で使用
		SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& GetFunctionPointer(void);
		SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& GetConditionVariable(void);
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

	//OSスレッド用のラッパ関数実装
#if defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__) || defined(__USE_THREAD_ARCHITECT_POSIX__) //OSアーキテクチャ使用が有効ならラッパ関数実装

	THREAD_WRAPPER_FUNCTYPE WorkThreadEx::pImplEx::ThreadMain_Wrapper(void* p)
	{
		SonikWorkThreadMainEx(reinterpret_cast<WorkThreadEx::pImplEx*>(p)); //main関数はthisを受け取るのでキャスト。

#if defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__)
		return 0;

#else
		return reinterpret_cast<void*>(0); //nullptr はC++11のキーワードなので。
#endif

	};

#endif

	//静的メソッドの定義
	void WorkThreadEx::pImplEx::SonikWorkThreadMainEx(WorkThreadEx::pImplEx* ClassObject)
	{
		uint32_t& RefFlag = ClassObject->GetThreadFlag();
		SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& RefCond = ClassObject->GetConditionVariable();
		SonikLib::S_CAS::SonikAtomicLock& RefLock = ClassObject->GetCASLockObject();
		std::mutex localmtx;
		SonikLib::SharedSmtPtr<SonikFOSInterface>& RefFuncObj = ClassObject->GetFunctionPointer();
		SonikLib::SharedSmtPtr<SonikFOSInterface> RunTask;
		SonikLib::SharedSmtPtr<SonikFOSInterface> StrongTask;

		while (1)
		{
			if (RunTask.IsNullptr())
			{
				while (1)
				{
					//セマフォループで使用。
					std::unique_lock<std::mutex> lock(localmtx);

					//ThreadMainEx関数終了フラグか、デキュー停止フラグが立っていればサスペンド解除し、後続の処理へ。。
					if (((RefFlag & 0x08) != 0) || ((RefFlag & 0x01) != 0))
					{
						ClassObject->SetThreadStatus_Suspend(false);
						lock.unlock();
						break;
					};

					//関数(タスク)取得
					ClassObject->UpdateQueue();

					//取れてたらサスペンド解除して後続処理へ。
					if (!RefFuncObj.IsNullptr())
					{
						RunTask = RefFuncObj;
						ClassObject->SetThreadStatus_Suspend(false);
						lock.unlock();
						break;
					};

					//取れなければサスペンド状態にして再度最初からチェック。
					ClassObject->SetThreadStatus_Suspend(true);
					lock.unlock();

					RefCond->acquire();
				};

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
	WorkThreadEx::pImplEx::pImplEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& _cond_, bool DetachThread)
		: FuncQueue_()
		, cond_(_cond_)
		, ThreadFlag(0)
		, DetachFlag(DetachThread)
	{

#if defined(__USE_THREAD_ARCHITECT_STD__)

		//使用アーキテクチャ：STD
		threads_ = std::thread(&WorkThreadEx::pImplEx::SonikWorkThreadMainEx, this);
		if (DetachThread)
		{
			threads_.detach();
		};

#elif defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__)

		//使用アーキテクチャ：CRT(C Runtime Library)
		threads_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, WorkThreadEx::pImplEx::ThreadMain_Wrapper, this, 0, NULL));
		if (threads_ != NULL)
		{
			if (DetachThread)
			{
				// Detach時はハンドルを閉じて管理から外す
				CloseHandle(threads_);
				threads_ = NULL;
			};

		}else
		{
			throw std::exception();
		};

#elif defined(__USE_THREAD_ARCHITECT_POSIX__)

		//使用アーキテクチャ：POSIX Thread
		int32_t ret = pthread_create(&threads_, NULL, ThreadMain_Wrapper, this);
		if (ret == 0)
		{
			if (DetachFlag)
			{
				pthread_detach(threads_);
			};

		}else
		{
			throw std::exception();
		};

#endif

	};

	//デストラクタ
	WorkThreadEx::pImplEx::~pImplEx(void)
	{
		SetThreadExitFlag(true);

		//完全終了フラグを立たせるためにスレッドを起床させる。
		cond_->release(1);

#if defined(__USE_THREAD_ARCHITECT_STD__)

		//使用アーキテクチャ：STD
		if (!DetachFlag && threads_.joinable())
		{
			//終了待ち。
			threads_.join();
		};

#elif defined(__USE_THREAD_ARCHITECT_WINDOWS_CRT__)

		//使用アーキテクチャ：CRT(C Runtime Library)
		if (!DetachFlag && threads_ != NULL)
		{
			// スレッドの終了を無限に待機
			WaitForSingleObject(threads_, INFINITE);
			// ハンドルを閉じる
			CloseHandle(threads_);
			threads_ = NULL;
		};

#elif defined(__USE_THREAD_ARCHITECT_POSIX__)

		//使用アーキテクチャ：POSIX Thread
		if (!DetachFlag)
		{
			pthread_join(threads_, NULL);
		};
#endif

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
		cond_->release(1);
		//ミューテックスのアンロックは静的関数内で行う。
		return true;
	};

	SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>& WorkThreadEx::pImplEx::GetFunctionPointer(void)
	{
		return FuncObj_;
	};

	SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& WorkThreadEx::pImplEx::GetConditionVariable(void)
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
			cond_->release(1);
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
		SonikLib::SLibAllocateInterface* l_defalloc;
		try
		{
			l_defalloc = new SonikLib::SLibAllocateInterface;
			if (!SonikLib::AllocatorSharedSmtPtr<SLibAllocateInterface>::SmartPointerCreate(l_defalloc, m_allocator))
			{
				delete l_defalloc;
				throw std::bad_alloc();
			};

			SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>> l_cond;
			if (!SonikLib::WorkerThreadWaitingObject<>::CreateWaitingObject(l_cond, m_allocator))
			{
				throw std::bad_alloc();
			};

			void* l_allocbuffer = l_defalloc->memal(sizeof(WorkThreadEx::pImplEx));
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
		SonikLib::WorkerThreadWaitingObject<>* lp_cond = nullptr;
		try
		{
			SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>> l_cond;
			
			if (!SonikLib::WorkerThreadWaitingObject<>::CreateWaitingObject(l_cond, _allocator_))
			{
				throw std::bad_alloc();
			};

			void* l_allocbuffer = _allocator_->memal(sizeof(WorkThreadEx::pImplEx));
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

	WorkThreadEx::WorkThreadEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& _cond_, bool DetachThread)
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
	WorkThreadEx::WorkThreadEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<>>& _cond_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, bool DetachThread)
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
			//ImplObject->~pImplEx();
			m_allocator->memdel(ImplObject);

			ImplObject = nullptr;
		};

	};

	//関数オブジェクトポインタをセットします。
	//引数1: スレッドで実行する関数オブジェクトを設定します。
	//引数2: 実行関数の実行終了後、再度繰り返し実行させる場合はtrueを指定します。(default = false, １回コールのみ。)
	//
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
		ImplObject->GetConditionVariable()->release();
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


//被りはしないだろうけど念の為依存性を切っておく。
#undef __USE_THREAD_ARCHITECT_STD__
#undef __USE_THREAD_ARCHITECT_WINDOWS_CRT__
#undef __USE_THREAD_ARCHITECT_POSIX__
#undef THREAD_WRAPPER_FUNCTYPE

