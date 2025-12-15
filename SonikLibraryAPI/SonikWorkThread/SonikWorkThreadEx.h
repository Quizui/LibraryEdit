
#ifndef SONIKWORKTHREADEX_H_
#define SONIKWORKTHREADEX_H_

 /*
  * このクラスはC++11のスレッドクラスを使用して
  * 一つのスレッドプールを形成するクラスです。
  * C++11対応コンパイラでないとコンパイル不可能です。
  *
  * また、実装はImpleイディオムにより隠蔽しています。
  *
  * 作成者の
  * コンパイル環境はEclipse + MINGW 32bit clang3.6.0コンパイラです。
  * gcc version 4.9.2 compiling
  *
  */

#include <SmartPointer/SonikSmartPointer.hpp>

namespace SonikLib
{
	template <long long>
	class WorkerThreadWaitingObject;
};

/*
namespace std
{
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
	namespace _LIBCPP_ABI_NAMESPACE
	{
		class condition_variable_any;
	};

#elif defined(_MSC_VER)
	class condition_variable_any;

#else
	class condition_variable_any;

#endif
};
*/

namespace SonikFunctionObjectDefines
{
	class FunctionObjectSystemInterface;
};

//using
namespace SonikLib
{
	using SonikFOSInterface = SonikFunctionObjectDefines::FunctionObjectSystemInterface;

	namespace Container
	{
		template<class QueueType>
		class SonikAtomicQueue;
	};
};

namespace SonikLib
{
	class WorkThreadEx
	{
	private:
		class pImplEx;
		pImplEx* ImplObject;

		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;
	private:
		//コピーとムーヴの禁止
		WorkThreadEx(const WorkThreadEx& _copy_) = delete;
		WorkThreadEx(WorkThreadEx&& _move_) = delete;
		WorkThreadEx& operator =(const WorkThreadEx& _copy_) = delete;
		WorkThreadEx& operator =(WorkThreadEx&& _move_) = delete;

	public:

		//本クラスのコンストラクタです。
		WorkThreadEx(bool DetachThread = false);
		WorkThreadEx(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, bool DetachThread = false);
		//外部のcondition_variable_anyオブジェクトを使用します。
		//外部のcondition_variable_anyオブジェクトが複数のスレッドに渡されていた場合、内部のスレッド起床関数コールが対象はすべてのスレッド対象になります。
		WorkThreadEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<LLONG_MAX>> &_cond_, bool DetachThread = false);
		WorkThreadEx(SonikLib::SharedSmtPtr<SonikLib::WorkerThreadWaitingObject<LLONG_MAX>>& _cond_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, bool DetachThread = false);

		//本クラスのデストラクタです。
		~WorkThreadEx(void);

		//関数オブジェクトポインタをセットします。
		//引数1: スレッドで実行する関数オブジェクトを設定します。
		//引数2: 実行関数の実行終了後、再度繰り返し実行させる場合はtrueを指定します。(default = false, １回コールのみ。)
		//
		//戻り値; スレッドが実行中の場合はfalseが返り、セットされません。
		//本関数はスレッドセーフです。
		//確実にセットしたい場合、前にセットされた関数があれば、それが終了し、関数がtrueを返却するまでループします。
		//別途QUEUEがセットされている場合、この関数は必ずfalseを返却します。
		//マルチスレッドにより、同時にキューセットと本関数が呼ばれた場合で、本関数が先にコールされた場合、本関数は、trueを返却します。
		bool SetCallFunction(SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> CallFunctionObject, bool _looped_ = false);

		//外部のキューをセットします。
		//本関数はSetCallFunctionと同時にコールされた場合で、SetCallFunctionが先に実行された場合、セットされた関数が終了するまで処理を返却しません。
		//本関数によりキューがセットされた後は、SetCallFunctionは無効となり、常にfalseを返却します。
		//本関数でセットしたキューにエンキューを行った場合、dispatchQueue関数をコールし、エンキューを行ったことを通知しなければデキュー処理を行いません。
		void Set_ExternalQueue(SonikLib::SharedSmtPtr<SonikLib::Container::SonikAtomicQueue<SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>>>& pSetQueue);

		//外部のキューをアンセットします。
		void UnSet_ExternalQueue(void);

		//スレッドにデキューの開始を通知します。
		void dispatchDeQueue(void);

		//スレッド実行中に設定を変更したい場合に使う関数群========
		//関数コールのループフラグのON/OFFへの設定を行います。 SetCallFunction（）の設定をtrueにし、後に変更したくなった場合に使用します。
		//現在のファンクションが終了し、判定箇所に来た場合に実行されます。
		//設定関数実行後、判定箇所をスレッドが通過済みの場合、次の設定関数コール終了後に判定が行われます。
		//Queueがセットされている場合はこの設定は常にOff状態となります。
		//また、Queueをアンセットされる際にはOffの状態のままとなりますので、Onに戻したい場合はOnをコールしてください。
		void SetFunctionloopEndFlagOn(void);
		void SetFunctionloopEndFlagOff(void);

		//スレッドの終了フラグをOnにします。
		//これはSetFunctionDeleteFlagOn()関数と同様の判定方法を行います。
		//よって、即時に終了されるわけではありません。
		void SetThreadExitFlagOn(void);

		//現在のスレッドがサスペンド状態にあるかどうかを取得します。
		//稼働中であればfalse, サスペンド状態であればTrueを返却します。
		bool GetThreadStatusSuspend(void);

	};

};



#endif /* SONIKWORKTHREADEX_H_ */