
#ifndef WORKER_FUNCTOR_SONIKLIBRARY_
#define WORKER_FUNCTOR_SONIKLIBRARY_

#include <SmartPointer/SonikSmartPointer.hpp>

//前方宣言
namespace SonikFunctionObjectDefines
{
	class FunctionObjectSystemInterface;
	//	template<class Rtype>
	//	class FunctionObjectSystemTemplateInterface;
};

//using
namespace SonikLib
{
	using SonikFOSInterface = SonikFunctionObjectDefines::FunctionObjectSystemInterface;
	//	template <class Rtype>
	//	using SonikFOSTemplateInterface = SonikFunctionObjectDefines::FunctionObjectSystemTemplateInterface<Rtype>;

};



namespace SonikFunctionObjectDefines
{
	//関数ポインタを同じ型で扱うためのベースクラスです
	//戻り値がある関数は指定できまてん。
	//Weak 及び Strong のnext はSonikLibのWorkerThread内で意味を持ちます。
	//それぞれ、Weak は次のタスク実行の前にスレッドをリスタートします。(関数と関数の間にスレッドの終了等の余地を設けます)
	//Strong はスレッドをリスタートせず、その場でStrong_nextに設定されているrunをコールします。(関数と関数の間にスレッド終了等の余地を設けません)
	class FunctionObjectSystemInterface
	{
	protected:
		//メソッドステータス
		bool MethodStatus;

		//メソッドプライオリティ
		unsigned long MethodPriority;

		//保持したオブジェクトを破棄するかどうかのフラグ
		bool Destroy_;

		//弱い依存の単方向next
		SonikLib::SharedSmtPtr<FunctionObjectSystemInterface> weak_next;
		//強い依存の単方向next
		SonikLib::SharedSmtPtr<FunctionObjectSystemInterface> strong_next;

		//アロケータ
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	public:
		//コンストラクタ
		inline FunctionObjectSystemInterface(void)
			:MethodStatus(false)
			, MethodPriority(0)
			, Destroy_(true)
		{
			//member value initialize only
		};

		inline virtual ~FunctionObjectSystemInterface(void)
		{
		};

		//弱い依存でnextのタスクを設定します。
		inline bool SetNext_Weak(FunctionObjectSystemInterface* _SetSmtPtr_)
		{
			return SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>::SmartPointerCreate(_SetSmtPtr_, weak_next, m_allocator);
		};

		inline bool SetNext_Weak(SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>& _SetSmtPtr_)
		{
			weak_next = _SetSmtPtr_;
			return true;
		};

		inline bool SetNext_Weak(SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>&& _SetSmtPtr_)
		{
			weak_next = _SetSmtPtr_;
			return true;
		};

		//強い依存でnextのタスクを設定します。
		inline bool SetNext_Strong(FunctionObjectSystemInterface* _SetSmtPtr_)
		{
			return SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>::SmartPointerCreate(_SetSmtPtr_, strong_next, m_allocator);
		};

		inline bool SetNext_Strong(SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>& _SetSmtPtr_)
		{
			strong_next = _SetSmtPtr_;
			return true;
		};

		inline bool SetNext_Strong(SonikLib::SharedSmtPtr<FunctionObjectSystemInterface>&& _SetSmtPtr_)
		{
			strong_next = _SetSmtPtr_;
			return true;
		};

		//弱い依存のnextタスクを取得します。
		inline SonikLib::SharedSmtPtr<FunctionObjectSystemInterface> GetNext_weak(void)
		{
			return weak_next;
		};

		//強い依存のnextタスクを取得します。
		inline SonikLib::SharedSmtPtr<FunctionObjectSystemInterface> GetNext_strong(void)
		{
			return strong_next;
		};

		//メソッドステータスの取得
		inline const bool& Get_MethodStatus(void)
		{

			return MethodStatus;
		};

		//メソッドステータスの状態をオフにします。
		inline void Set_MethodStatusOff(void)
		{
			MethodStatus = false;
		};


		//メソッドプライオリティの取得
		inline const unsigned long& Get_MethodPriority(void)
		{
			return MethodPriority;

		};

		//メソッドプライオリティの変更
		inline void Set_MethodPriority(unsigned long SetPriority)
		{

			MethodPriority = SetPriority;
		};

		//保持オブジェクトの破棄フラグ
		inline void Set_DestroyObjectFlag(bool SetValue)
		{
			Destroy_ = SetValue;

		};

		inline const bool& Get_DestroyObjectFlag(void)
		{
			return Destroy_;
		}

		virtual void Run(void) = 0;
	};


};//end namespace


#endif //WORKER_FUNCTOR_SONICREATIVE_