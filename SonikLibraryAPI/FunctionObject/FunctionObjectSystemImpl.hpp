#ifndef WORKER_FUNCTOR_DEFINITION_SONIKLIBRARY_
#define WORKER_FUNCTOR_DEFINITION_SONIKLIBRARY_

#include "FunctionObjectSystemInterface.hpp"
#include <SmartPointer/SonikSmartPointer.hpp>

#include <CPPGrammarDefines.h>
#include <CompilersPreProcesser.h>

//引数を10個まで取れるテンプレートクラスを定義します。
//それぞれクラスのメンバ関数を登録する場合において、ポインタのポインタ型は指定できません。
//指定しようとした場合は型不一致のコンパイルエラーが出ます。
//インスタンス作成時のテンプレートのTypeに対して、オブジェクトのポインタ型は指定できません。
//同じく指定しようとした場合は型不一致のコンパイルエラーが出ます。

//戻り値があるものについてはベースクラスは FunctionObjectSystemInterface となっています
//継承元がテンプレートクラスであるため、一括して扱うことはできません。
//ただし、戻り値の型が同一の場合は一括で扱うことが可能です。構文例は下記の通り
//FunctionSystemTemplate_Base<int>* 等... <型名>はたとえポインタであっても必要です。

//戻り値が無いものについてはベースクラスは FunctionObjectSystemTemplateInterface となっています。
//継承元は非テンプレートであるため、型は同一の物として一括で扱うことが可能です。

//基本的にインターフェースが提供するのはメソッドを実行する。という機能だけになります。

//Typeがvoid型の特殊化引数については、グローバル関数用の実装となります。
//ベースクラスは戻り値があるもの、無いもので上記二つと一緒です。
//いずれのクラスもローカル変数として生成し、スレッドに渡されてほしくないため、
//NEW関数を経由しての作成を強制しています。

namespace SonikLib
{
	//テンプレート再実装を試みる
	//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
	//▶　クラスメンバ関数オブジェクト　 ◀
	//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
	//引数 10個==================================================================
	template <class Cls_T, class arg_T1 = void, class arg_T2 = void, class arg_T3 = void, class arg_T4 = void, class arg_T5 = void, class arg_T6 = void, class arg_T7 = void, class arg_T8 = void, class arg_T9 = void, class arg_T10 = void>
	class FunctionObject_ClsMember : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;
		arg_T9 m_arg9;
		arg_T10 m_arg10;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_, arg_T9 _argval9_, arg_T10 _argval10_)
			: BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
			, m_arg9(_argval9_)
			, m_arg10(_argval10_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			//FuncRetType == voidの場合は戻り値void関数として展開
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9, m_arg10);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  9個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7, class arg_T8, class arg_T9>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;
		arg_T9 m_arg9;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_, arg_T9 _argval9_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
			, m_arg9(_argval9_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  8個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7, class arg_T8>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  7個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  6個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  5個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  4個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3, class arg_T4>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3, arg_T4));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  3個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2, class arg_T3>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2, arg_T3));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2, m_arg3);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>;;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>;;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>;;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>;;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  2個==================================================================
	template <class Cls_T, class arg_T1, class arg_T2>
	class FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2, void, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1, arg_T2));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_, arg_T2 _argval2_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1, m_arg2);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  1個==================================================================
	template <class Cls_T, class arg_T1>
	class FunctionObject_ClsMember<Cls_T, arg_T1, void, void, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)(arg_T1));

	private:
		arg_T1 m_arg1;

		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(arg_T1 _argval1_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)(m_arg1);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_)
		{
			
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_);

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType(Val1_);

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, arg_T1 Val1_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T, arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType(Val1_);

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  0個==================================================================
	template <class Cls_T>
	class FunctionObject_ClsMember<Cls_T, void, void, void, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (Cls_T::*)());

	private:
		SonikLib::SharedSmtPtr<Cls_T> m_object;
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_ClsMember(void)
		:BaseInterfaceCls()
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_ClsMember(void)
		{
			if (this->Destroy_ == false)
			{
				//オブジェクト削除フラグがfalseなら勝手にdeleteしてはいけないので
				//スマートポインタからオーナー権限を除去
				m_object.DestroyOwner();
			};
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			if (m_object.IsNullptr())
			{
				return;
			};

			(m_object->*m_p_mfunc)();
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(スマートポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType();

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(スマートポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(SonikLib::SharedSmtPtr<Cls_T> _SetObj_, FUNCNAMEDEF _set_func_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType();

			lp->m_allocator = _allocator_;
			lp->m_object = _SetObj_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};

		//クリエイタ(Rawポインタ指定(アロケータ無し))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//通常newのアロケータを生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface();
			if (l_alloc_obj == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//アロケータnew(通常new)
			lp = new(l_allocbuffer) CreateClsType();

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocsmtptr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocsmtptr))
			{

				l_alloc_obj->memdel(lp);
				delete l_alloc_obj;

				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, l_allocsmtptr))
			{
				//失敗時はnullptrとして返却
				l_allocsmtptr->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = l_allocsmtptr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocsmtptr))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType();
				l_allocsmtptr->memdel(lp);
			};

			return ret_sp;
		};
		//クリエイタ(Rawポインタ指定(アロケータ有り))
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(Cls_T* _SetObj_, FUNCNAMEDEF _set_func_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_ClsMember<Cls_T>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_ClsMember<Cls_T>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			if (_SetObj_ == nullptr)
			{
				//Global版ではないのにnullptrを指定されたらそのまま返却。
				return RetSmtPtrType();
			};

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			};

			lp = new(alloc_buffer) CreateClsType();

			//クラスセット
			if (!SonikLib::SharedSmtPtr<Cls_T>::SmartPointerCreate(_SetObj_, lp->m_object, _allocator_))
			{
				//失敗時はnullptrとして返却
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
				return RetSmtPtrType();
			};

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				//スマートポインタへの設定時にエラーの際はオブジェクトdelete。ret_spはnullptrのままなので、そのまま返却。
				lp->~CreateClsType(); //デストラクタコール(アロケータのCallDestructorも同じ処理のため直接呼べるなら呼んでおく。関数コールの時間節約)
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};


	//▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼
	//▶　  グローバル関数オブジェクト　 ◀
	//▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲
	//引数 10個==================================================================
	template <class arg_T1 = void, class arg_T2 = void, class arg_T3 = void, class arg_T4 = void, class arg_T5 = void, class arg_T6 = void, class arg_T7 = void, class arg_T8 = void, class arg_T9 = void, class arg_T10 = void>
	class FunctionObject_Global : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;
		arg_T9 m_arg9;
		arg_T10 m_arg10;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_, arg_T9 _argval9_, arg_T10 _argval10_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
			, m_arg9(_argval9_)
			, m_arg10(_argval10_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9, m_arg10);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, arg_T10 Val10_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, arg_T10>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_, Val10_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  9個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7, class arg_T8, class arg_T9>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;
		arg_T9 m_arg9;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_, arg_T9 _argval9_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
			, m_arg9(_argval9_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8, m_arg9);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, arg_T9 Val9_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, arg_T9>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_, Val9_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  8個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7, class arg_T8>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;
		arg_T8 m_arg8;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_, arg_T8 _argval8_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
			, m_arg8(_argval8_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7, m_arg8);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, arg_T8 Val8_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, arg_T8>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_, Val8_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  7個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6, class arg_T7>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;
		arg_T7 m_arg7;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_, arg_T7 _argval7_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
			, m_arg7(_argval7_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6, m_arg7);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, arg_T7 Val7_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, arg_T7>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_, Val7_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  6個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5, class arg_T6>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;
		arg_T6 m_arg6;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_, arg_T6 _argval6_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
			, m_arg6(_argval6_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, arg_T6 Val6_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, arg_T6>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_, Val6_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  5個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4, class arg_T5>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4, arg_T5));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;
		arg_T5 m_arg5;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_, arg_T5 _argval5_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
			, m_arg5(_argval5_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, arg_T5 Val5_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, arg_T5>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_, Val5_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  4個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3, class arg_T4>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3, arg_T4));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;
		arg_T4 m_arg4;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_, arg_T4 _argval4_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
			, m_arg4(_argval4_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3, m_arg4);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);
			
			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, arg_T4 Val4_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3, arg_T4>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_, Val4_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  3個==================================================================
	template <class arg_T1, class arg_T2, class arg_T3>
	class FunctionObject_Global<arg_T1, arg_T2, arg_T3, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2, arg_T3));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;
		arg_T3 m_arg3;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_, arg_T3 _argval3_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
			, m_arg3(_argval3_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2, m_arg3);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_, Val3_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, arg_T3 Val3_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2, arg_T3>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_, Val3_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  2個==================================================================
	template <class arg_T1, class arg_T2>
	class FunctionObject_Global<arg_T1, arg_T2, void, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1, arg_T2));

	private:
		arg_T1 m_arg1;
		arg_T2 m_arg2;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_, arg_T2 _argval2_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
			, m_arg2(_argval2_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1, m_arg2);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_, Val2_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, arg_T2 Val2_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1, arg_T2>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1, arg_T2>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_, Val2_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  1個==================================================================
	template <class arg_T1>
	class FunctionObject_Global<arg_T1, void, void, void, void, void, void, void, void, void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(arg_T1));

	private:
		arg_T1 m_arg1;

		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(arg_T1 _argval1_)
			:BaseInterfaceCls()
			, m_arg1(_argval1_)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)(m_arg1);
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType(Val1_);

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, arg_T1 Val1_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<arg_T1>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<arg_T1>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType(Val1_);

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

	//引数  0個==================================================================
	template <>
	class FunctionObject_Global<void> : public SonikFunctionObjectDefines::FunctionObjectSystemInterface
	{
		//名前長いので短く！
		//親クラス名
		SLIB_CVR_USING(BaseInterfaceCls, SonikFunctionObjectDefines::FunctionObjectSystemInterface);
		//関数名
		SLIB_CVR_USING(FUNCNAMEDEF, void (*)(void));

	private:
		FUNCNAMEDEF m_p_mfunc;

	private:
		//コンストラクタ
		DEF_FORCE_INLINE FunctionObject_Global(void)
			:BaseInterfaceCls()
			, m_p_mfunc(nullptr)
		{
			//no process
		};

	public:
		//デストラクタ
		DEF_FORCE_INLINE ~FunctionObject_Global(void)
		{
			//グローバル関数はメンバ関数と違ってオブジェクトは必要ないのでそのまま何もしない。
		};

		//関数実行Run
		DEF_FORCE_INLINE void Run(void)
		{
			(*m_p_mfunc)();
			this->MethodStatus = true;

			return;
		};

		//クリエイタ(アロケータ無し)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<void>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<void>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//通常newをするデフォルトアロケータの生成
			SonikLib::SLibAllocateInterface* l_alloc_obj = new(std::nothrow) SonikLib::SLibAllocateInterface;
			if (l_alloc_obj == nullptr)
			{
				return RetSmtPtrType();
			};

			void* l_allocbuffer = l_alloc_obj->memal(sizeof(CreateClsType));
			if (l_allocbuffer == nullptr)
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			}

			SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> l_allocSmtPtr;
			if (!SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface>::SmartPointerCreate(l_alloc_obj, l_allocSmtPtr))
			{
				delete l_alloc_obj;
				return RetSmtPtrType();
			};

			//placement new
			lp = new(l_allocbuffer) CreateClsType();

			lp->m_allocator = l_allocSmtPtr;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, l_allocSmtPtr))
			{
				lp->~CreateClsType();
				l_alloc_obj->memdel(lp); //スマートポインタ経由だとコールオーバーヘッドがかかるのでポインタから直呼び
			};

			return ret_sp;
		};
		//クリエイタ(アロケータ有り)
		//クラスオブジェクトが無いため、スマートポインタ版とRaw版に分ける必要がない。
		static DEF_FORCE_INLINE SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface> New(FUNCNAMEDEF _set_func_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
			//using CreateClsType = SonikLib::FunctionObject_Global<void>;
			SLIB_CVR_USING(CreateClsType, SonikLib::FunctionObject_Global<void>);
			//using RetSmtPtrType = SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>;
			SLIB_CVR_USING(RetSmtPtrType, SonikLib::SharedSmtPtr<SonikLib::SonikFOSInterface>);

			CreateClsType* lp = nullptr;

			//アロケータnew
			void* alloc_buffer = _allocator_->memal(sizeof(CreateClsType));
			if (alloc_buffer == nullptr)
			{
				//失敗時はnullptrとして返却
				return RetSmtPtrType();
			}

			lp = new(alloc_buffer) CreateClsType();

			lp->m_allocator = _allocator_;
			lp->m_p_mfunc = _set_func_;

			RetSmtPtrType ret_sp;
			if (!RetSmtPtrType::SmartPointerCreate(reinterpret_cast<SonikLib::SonikFOSInterface*>(lp), ret_sp, _allocator_))
			{
				lp->~CreateClsType();
				_allocator_->memdel(lp);
			};

			return ret_sp;
		};
	};

};

#endif