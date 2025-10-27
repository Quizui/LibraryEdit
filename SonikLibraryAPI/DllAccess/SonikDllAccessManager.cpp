
#include <map>

#include "SonikDllAccessManager.h"
#include "SonikDllLoader.h"

#include "../SonikAssignFreeNumber/SonikAssignFreeNumber.h"

namespace SonikLib
{

#if defined(_WIN32) || defined(_WIN64)

	//InnerClassImplement
	class SonikDllHandleManager::DllObjectImple : public SonikDllOptions::SonikDllObject
	{
	private:
		SONIK_DLL_ACCESS_MANAGER_POINTER m_handle;
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	public:
		//Constructor
		DllObjectImple(SONIK_DLL_ACCESS_MANAGER_POINTER SetHandle, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		:m_handle(SetHandle)
		,m_allocator(_allocator_)
		{
			//no process
		};

		//Destructor
		~DllObjectImple(void)
		{
			if (m_handle != 0)
			{
				SonikLibLoderFree(m_handle);
				m_handle = nullptr;
			};
		};

		//PureFunctions
		uintptr_t GetDllProcAddress(const char* ProcName)
		{
			SonikLib::SonikString str(ProcName, m_allocator);

			return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(m_handle, str.str_c()));
		};

		uintptr_t GetDllProcAddress(const char16_t* ProcName)
		{
			SonikLib::SonikString str(ProcName, m_allocator);

			return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(m_handle, str.str_c()));
		}

		uintptr_t GetDllProcAddress(const wchar_t* ProcName)
		{
			SonikLib::SonikString str(ProcName, m_allocator);

			return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(m_handle, str.str_c()));
		}

	};

	//InnerClassImplement
	class SonikDllHandleManager::DllPairState
	{
	public:
		int64_t KeyNumber;
		SonikLib::SonikString dllpath;
		SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle;

		DllPairState* next;
		DllPairState* prev;

	public:
		inline DllPairState(void)
		:KeyNumber(-1)
		,dllpath("")
		,dllhandle(nullptr)
		,next(nullptr)
		,prev(nullptr)
		{
			//no process
		};
		inline DllPairState(SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
			:KeyNumber(-1)
			, dllpath("", _allocator_)
			, dllhandle(nullptr)
			, next(nullptr)
			, prev(nullptr)
		{
			//no process
		};
		inline DllPairState(SonikLib::SonikString& _str_, int64_t _key_ = -1, SONIK_DLL_ACCESS_MANAGER_POINTER _handle_ = nullptr, DllPairState* _next_ = nullptr, DllPairState* _prev_ = nullptr)
		:KeyNumber(_key_)
		,dllpath(_str_)
		,dllhandle(_handle_)
		,next(_next_)
		,prev(_prev_)
		{
			//no process
		};

		inline ~DllPairState()
		{
			SonikLibLoderFree(dllhandle);
			dllhandle = nullptr;
		};
	};


	//Destructor
	SonikDllHandleManager::~SonikDllHandleManager(void)
	{
		if(DllHandleList != nullptr)
		{
			
			DllPairState* sent_s = SentinelNode_Start->next;
			DllPairState* sent_e = SentinelNode_End;
			DllPairState* destrcuto_call_obj = sent_s;

			while (sent_s != sent_e)
			{
				sent_s = sent_s->next;
				destrcuto_call_obj->~DllPairState();
			};

			m_allocator->memdel(DllHandleList);

			m_allocator->CallDestructor(SentinelNode_Start);
			m_allocator->CallDestructor(SentinelNode_End);

			//メモリ確保時はStartとEnd分一括で取得しており、その領域の戦闘ポインタはStartなのでStartポインタをdeleteとして指定。
			m_allocator->memdel(SentinelNode_Start); 

			DllHandleList = nullptr;
			SentinelNode_Start = nullptr;
			SentinelNode_End = nullptr;
		};
	};

	bool SonikDllHandleManager::CreateManager(SonikLib::SharedSmtPtr<SonikDllHandleManager>& _out_, uint64_t _dll_list_cnt_)
	{
		//アロケータ生成
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

		void* l_allocbuffer[2]{};
		l_allocbuffer[0] = l_defalloc->memal(sizeof(SonikDllHandleManager));
		if (l_allocbuffer[0] == nullptr)
		{
			return false;
		};
		l_allocbuffer[1] = l_defalloc->memal(sizeof(SonikDllHandleManager::DllPairState) * _dll_list_cnt_);
		if (l_allocbuffer[1] == nullptr)
		{
			return false;
		};

		uint8_t* l_sentinel_object_buffer_control = reinterpret_cast<uint8_t*>(l_defalloc->memal(sizeof(SonikDllHandleManager::DllPairState) * 2));
		if (l_sentinel_object_buffer_control == nullptr)
		{
			return false;
		};

		SonikLib::SonikDllHandleManager* l_dhm = new(l_allocbuffer[0]) SonikLib::SonikDllHandleManager;
		l_dhm->m_allocator = l_allocSmtPtr;
		l_dhm->DllHandleList = reinterpret_cast<DllPairState*>(l_allocbuffer[1]);

		//配列管理用番兵を作成
		l_dhm->SentinelNode_Start = new(l_sentinel_object_buffer_control) SonikDllHandleManager::DllPairState();
		l_sentinel_object_buffer_control += sizeof(SonikDllHandleManager::DllPairState);
		l_dhm->SentinelNode_End = new(l_sentinel_object_buffer_control) SonikDllHandleManager::DllPairState();

		l_dhm->DllHandleListCnt = _dll_list_cnt_;

		//番兵同士をつなぎ合わせる
		l_dhm->SentinelNode_Start->next = l_dhm->SentinelNode_End;
		l_dhm->SentinelNode_Start->prev = l_dhm->SentinelNode_End;
		l_dhm->SentinelNode_End->next = l_dhm->SentinelNode_Start;
		l_dhm->SentinelNode_End->prev = l_dhm->SentinelNode_Start;

		if (!SonikLib::SonikAssignFreeNumber::CreateObject(l_dhm->m_asigned, l_allocSmtPtr))
		{
			l_dhm->~SonikDllHandleManager();
			l_defalloc->memdel(l_dhm);
			return false;
		};

		if (!SonikLib::SharedSmtPtr<SonikDllHandleManager>::SmartPointerCreate(l_dhm, _out_, l_allocSmtPtr))
		{
			l_dhm->~SonikDllHandleManager();
			l_defalloc->memdel(l_dhm);
			return false;
		};

		return true;
	};

	bool SonikDllHandleManager::CreateManager(SonikLib::SharedSmtPtr<SonikDllHandleManager>& _out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint64_t _dll_list_cnt_)
	{
		void* l_allocbuffer[2]{};
		l_allocbuffer[0] = _allocator_->memal(sizeof(SonikDllHandleManager));
		if (l_allocbuffer[0] == nullptr)
		{
			return false;
		};
		l_allocbuffer[1] = _allocator_->memal(sizeof(SonikDllHandleManager::DllPairState) * _dll_list_cnt_);
		if (l_allocbuffer[1] == nullptr)
		{
			return false;
		};

		uint8_t* l_sentinel_object_buffer_control = reinterpret_cast<uint8_t*>(_allocator_->memal(sizeof(SonikDllHandleManager::DllPairState) * 2));
		if (l_sentinel_object_buffer_control == nullptr)
		{
			return false;
		};

		SonikLib::SonikDllHandleManager* l_dhm = new(l_allocbuffer[0]) SonikLib::SonikDllHandleManager;
		l_dhm->m_allocator = _allocator_;
		l_dhm->DllHandleList = reinterpret_cast<DllPairState*>(l_allocbuffer[1]);
		l_dhm->DllHandleListCnt = _dll_list_cnt_;

		//配列管理用番兵を作成
		l_dhm->SentinelNode_Start = new(l_sentinel_object_buffer_control) SonikDllHandleManager::DllPairState(_allocator_);
		l_sentinel_object_buffer_control += sizeof(SonikDllHandleManager::DllPairState);
		l_dhm->SentinelNode_End = new(l_sentinel_object_buffer_control) SonikDllHandleManager::DllPairState(_allocator_);

		//番兵同士をつなぎ合わせる
		l_dhm->SentinelNode_Start->next = l_dhm->SentinelNode_End;
		l_dhm->SentinelNode_Start->prev = l_dhm->SentinelNode_End;
		l_dhm->SentinelNode_End->next = l_dhm->SentinelNode_Start;
		l_dhm->SentinelNode_End->prev = l_dhm->SentinelNode_Start;

		if (!SonikLib::SonikAssignFreeNumber::CreateObject(l_dhm->m_asigned, _allocator_))
		{
			l_dhm->~SonikDllHandleManager();
			_allocator_->memdel(l_dhm);
			return false;
		};

		if (!SonikLib::SharedSmtPtr<SonikDllHandleManager>::SmartPointerCreate(l_dhm, _out_, _allocator_))
		{
			l_dhm->~SonikDllHandleManager();
			_allocator_->memdel(l_dhm);
			return false;
		};

		return true;
	};


	bool SonikDllHandleManager::DllGetLoad(const char* _DllPath_, SDllHandle& _GetDllObject_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

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

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, l_allocSmtPtr);
		//dll_str = _DllPath_;

		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = l_defalloc->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};


		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			l_defalloc->memdel(l_allocbuffer);
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, l_allocSmtPtr);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_))
		{
			l_imple->~DllObjectImple();
			l_defalloc->memdel(l_imple);
			return false;
		};

		return true;
	};
	bool SonikDllHandleManager::DllGetLoad(const char* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		//アロケータ指定バージョン
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, _allocator_);
		//dll_str = _DllPath_;
		
		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = _allocator_->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == 0)
		{
			return false;
		};

		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, _allocator_);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_, _allocator_))
		{
			l_imple->~DllObjectImple();
			_allocator_->memdel(l_imple);
			return false;
		};

		return true;
	};

	bool SonikDllHandleManager::DllGetLoad(const char16_t* _DllPath_, SDllHandle& _GetDllObject_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

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

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, l_allocSmtPtr);
		//dll_str = _DllPath_;

		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = l_defalloc->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};


		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			l_defalloc->memdel(l_allocbuffer);
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, l_allocSmtPtr);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_))
		{
			l_imple->~DllObjectImple();
			l_defalloc->memdel(l_imple);
			return false;
		};

		return true;
	};
	bool SonikDllHandleManager::DllGetLoad(const char16_t* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		//アロケータ指定バージョン
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, _allocator_);
		//dll_str = _DllPath_;

		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = _allocator_->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == 0)
		{
			return false;
		};

		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, _allocator_);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_, _allocator_))
		{
			l_imple->~DllObjectImple();
			_allocator_->memdel(l_imple);
			return false;
		};

		return true;
	};

	bool SonikDllHandleManager::DllGetLoad(const wchar_t* _DllPath_, SDllHandle& _GetDllObject_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

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

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, l_allocSmtPtr);
		//dll_str = _DllPath_;

		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = l_defalloc->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == nullptr)
		{
			return false;
		};


		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			l_defalloc->memdel(l_allocbuffer);
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, l_allocSmtPtr);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_))
		{
			l_imple->~DllObjectImple();
			l_defalloc->memdel(l_imple);
			return false;
		};

		return true;

	};
	bool SonikDllHandleManager::DllGetLoad(const wchar_t* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
	{
		//アロケータ指定バージョン
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;

		//Dllパスを汎用文字列処理クラスに。
		SonikLib::SonikString dll_str(_DllPath_, _allocator_);
		//dll_str = _DllPath_;

		//DllObjectImple用領域の確保。ライブラリロード前にしておかないとエラー時ライブラリのフリーが必要になりそちらのほうがコストかかりそうなので..。
		void* l_allocbuffer = _allocator_->memal(sizeof(DllObjectImple));
		if (l_allocbuffer == 0)
		{
			return false;
		};

		//とりあえずANSI形式で文字列を指定
		handles_ = SonikLibLoder(dll_str.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			return false;
		};

		DllObjectImple* l_imple = new(l_allocbuffer) DllObjectImple(handles_, _allocator_);

		if (!SDllHandle::SmartPointerCreate(l_imple, _GetDllObject_, _allocator_))
		{
			l_imple->~DllObjectImple();
			_allocator_->memdel(l_imple);
			return false;
		};

		return true;
	};

	bool SonikDllHandleManager::DllLoad(const char* _DllPath_, uint64_t& _out_dll_reg_number_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;
		SonikLib::SonikString findstr(_DllPath_, m_allocator);
		//findstr = _DllPath_;
		
		DllPairState* l_CheckPoint = SentinelNode_Start->next;
		DllPairState* l_EndPoint = SentinelNode_End;

		m_lock.lock();

		//重複チェック
		while (l_CheckPoint != l_EndPoint)
		{
			if (l_CheckPoint->dllpath == findstr)
			{
				//重複しているため読み込まず重複しているアイテムの番号を返却する
				//ロードはしていないが、すでにロード済みの番号を返せているので結果的にはtrue。
				_out_dll_reg_number_ = static_cast<uint64_t>(l_CheckPoint->KeyNumber);
				m_lock.unlock();
				return true;
			};

			l_CheckPoint = l_CheckPoint->next;
		};

		//重複無しのためロード処理
		handles_ = SonikLibLoder(findstr.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			m_lock.unlock();
			return false;
		};

		int64_t l_lendnum = m_asigned->LendNumber();
		if ((l_lendnum < 0))
		{
			//借用番号がマイナス値なら借用失敗。
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;

		}else if (static_cast<uint64_t>(l_lendnum) > (DllHandleListCnt -1))
		{
			//借用番号が配列数を超えていたら失敗(一旦返す)
			m_asigned->ReturnNumber(l_lendnum);
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;
		};

		//領域を使ってオブジェクト生成
		DllPairState* l_obj = new(DllHandleList + static_cast<uint64_t>(l_lendnum)) DllPairState(findstr, l_lendnum, handles_, SentinelNode_Start->next, SentinelNode_Start);

		//双方向リストの調整。追加オブジェクトはコンストラクタで調整済みなので番兵からの宛先のみ調整
		//常に番兵のとなりに挿入。新しいものほどS側に近い。
		SentinelNode_Start->next->prev = l_obj;
		SentinelNode_Start->next = l_obj;

		_out_dll_reg_number_ = static_cast<uint64_t>(l_lendnum);

		m_lock.unlock();
		return true;
	};

	bool SonikDllHandleManager::DllLoad(const char16_t* _DllPath_, uint64_t& _out_dll_reg_number_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;
		
		SonikLib::SonikString findstr(_DllPath_, m_allocator);
		//findstr = _DllPath_;

		DllPairState* l_CheckPoint = SentinelNode_Start->next;
		DllPairState* l_EndPoint = SentinelNode_End;

		m_lock.lock();

		//重複チェック
		while (l_CheckPoint != l_EndPoint)
		{
			if (l_CheckPoint->dllpath == findstr)
			{
				//重複しているため読み込まず重複しているアイテムの番号を返却する
				//ロードはしていないが、すでにロード済みの番号を返せているので結果的にはtrue。
				_out_dll_reg_number_ = static_cast<uint64_t>(l_CheckPoint->KeyNumber);
				m_lock.unlock();
				return true;
			};

			l_CheckPoint = l_CheckPoint->next;
		};

		//重複無しのためロード処理
		handles_ = SonikLibLoder(findstr.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			m_lock.unlock();
			return false;
		};

		int64_t l_lendnum = m_asigned->LendNumber();
		if ((l_lendnum < 0))
		{
			//借用番号がマイナス値なら借用失敗。
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;

		}
		else if (static_cast<uint64_t>(l_lendnum) > (DllHandleListCnt - 1))
		{
			//借用番号が配列数を超えていたら失敗(一旦返す)
			m_asigned->ReturnNumber(l_lendnum);
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;
		};

		//領域を使ってオブジェクト生成
		DllPairState* l_obj = new(DllHandleList + static_cast<uint64_t>(l_lendnum)) DllPairState(findstr, l_lendnum, handles_, SentinelNode_Start->next, SentinelNode_Start);

		//双方向リストの調整。追加オブジェクトはコンストラクタで調整済みなので番兵からの宛先のみ調整
		//常に番兵のとなりに挿入。新しいものほどS側に近い。
		SentinelNode_Start->next->prev = l_obj;
		SentinelNode_Start->next = l_obj;

		_out_dll_reg_number_ = static_cast<uint64_t>(l_lendnum);

		m_lock.unlock();
		return true;
	};

	bool SonikDllHandleManager::DllLoad(const wchar_t* _DllPath_, uint64_t& _out_dll_reg_number_)
	{
		SONIK_DLL_ACCESS_MANAGER_POINTER handles_;
		
		SonikLib::SonikString findstr(_DllPath_, m_allocator);
		//findstr = _DllPath_;

		DllPairState* l_CheckPoint = SentinelNode_Start->next;
		DllPairState* l_EndPoint = SentinelNode_End;

		m_lock.lock();

		//重複チェック
		while (l_CheckPoint != l_EndPoint)
		{
			if (l_CheckPoint->dllpath == findstr)
			{
				//重複しているため読み込まず重複しているアイテムの番号を返却する
				//ロードはしていないが、すでにロード済みの番号を返せているので結果的にはtrue。
				_out_dll_reg_number_ = static_cast<uint64_t>(l_CheckPoint->KeyNumber);
				m_lock.unlock();
				return true;
			};

			l_CheckPoint = l_CheckPoint->next;
		};

		//重複無しのためロード処理
		handles_ = SonikLibLoder(findstr.str_c(), SonikDllLoader::LibLoderFlag::LOAD_DEFAULT);
		if (handles_ == 0)
		{
			m_lock.unlock();
			return false;
		};

		int64_t l_lendnum = m_asigned->LendNumber();
		if ((l_lendnum < 0))
		{
			//借用番号がマイナス値なら借用失敗。
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;

		}
		else if (static_cast<uint64_t>(l_lendnum) > (DllHandleListCnt - 1))
		{
			//借用番号が配列数を超えていたら失敗(一旦返す)
			m_asigned->ReturnNumber(l_lendnum);
			SonikLibLoderFree(handles_);
			m_lock.unlock();
			return false;
		};

		//領域を使ってオブジェクト生成
		DllPairState* l_obj = new(DllHandleList + static_cast<uint64_t>(l_lendnum)) DllPairState(findstr, l_lendnum, handles_, SentinelNode_Start->next, SentinelNode_Start);

		//双方向リストの調整。追加オブジェクトはコンストラクタで調整済みなので番兵からの宛先のみ調整
		//常に番兵のとなりに挿入。新しいものほどS側に近い。
		SentinelNode_Start->next->prev = l_obj;
		SentinelNode_Start->next = l_obj;

		_out_dll_reg_number_ = static_cast<uint64_t>(l_lendnum);

		m_lock.unlock();
		return true;
	};

	void SonikDllHandleManager::FreeDll(uint64_t _reg_number_)
	{
		if(_reg_number_ > (DllHandleListCnt - 1))
		{
			//配列数より大きい値なら何もしない。
			return;
		};

		DllPairState* l_target = (DllHandleList + _reg_number_);

		m_lock.lock();

		//デストラクタコール(デストラクタでハンドルを開放してる。)
		l_target->~DllPairState();
		//双方向リストの調整
		l_target->next->prev = l_target->prev;
		l_target->prev->next = l_target->next;

		//番号返却
		m_asigned->ReturnNumber(static_cast<int64_t>(_reg_number_));

		m_lock.unlock();
		return;
	};

	uintptr_t SonikDllHandleManager::GetDllProcAddress(SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const char* ProcName)
	{
		SonikLib::SonikString str(ProcName, m_allocator);

		return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(dllhandle, str.str_c()));
	};

	uintptr_t SonikDllHandleManager::GetDllProcAddress(SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const char16_t* ProcName)
	{
		SonikLib::SonikString str(ProcName, m_allocator);

		return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(dllhandle, str.str_c()));
	};

	uintptr_t SonikDllHandleManager::GetDllProcAddress(SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const wchar_t* ProcName)
	{
		SonikLib::SonikString str(ProcName, m_allocator);

		return reinterpret_cast<uintptr_t>(SonikLibLoderProcAddress(dllhandle, str.str_c()));
	};

	SONIK_DLL_ACCESS_MANAGER_POINTER SonikDllHandleManager::GetHandle(uint64_t _reg_number_)
	{
		if (_reg_number_ > (DllHandleListCnt - 1))
		{
			//配列数より大きい値なら何もしない。
			return nullptr;
		};

		SONIK_DLL_ACCESS_MANAGER_POINTER ret = nullptr;
		DllPairState* l_obj = (DllHandleList + _reg_number_);
		m_lock.lock();

		ret = l_obj->dllhandle;

		m_lock.unlock();

		return ret;

	};

#elif defined(__linux__)

#endif

};