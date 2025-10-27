#pragma once //有効ではないコンパイラの場合は無視される。

#ifndef SONIKDLLACCESSMANAGER_H_
#define SONIKDLLACCESSMANAGER_H_

#include <stdint.h>
#include "../SonikString/SonikStringDefinition.h"
#include "../SmartPointer/SonikSmartPointer.hpp"
#include "../SonikCAS/SonikAtomicLock.h"

#if defined(__linux__)
using SONIK_DLL_ACCESS_MANAGER_POINTER = void*;

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
using SONIK_DLL_ACCESS_MANAGER_POINTER = HINSTANCE__*;

#endif

//前方宣言
namespace SonikLib
{
	class SonikAssignFreeNumber;

};


namespace SonikDllOptions
{
	class SonikDllObject
	{
	public:
		//Constructor
		SonikDllObject(void)
		{
			//no process;
		};

		//Destructor
		virtual ~SonikDllObject(void)
		{
			//no process;
		};

		//pure functions
		virtual uintptr_t GetDllProcAddress(const char* ProcName) = 0;
		virtual uintptr_t GetDllProcAddress(const char16_t* ProcName) = 0;
		virtual uintptr_t GetDllProcAddress(const wchar_t* ProcName) = 0;
	};

}; //end namespace SonikDllOptions

namespace SonikLib
{
	using SDllHandle = SonikLib::SharedSmtPtr<SonikDllOptions::SonikDllObject>;

	class SonikDllHandleManager
	{
	private:
		//InnerClass宣言
		class DllObjectImple;
		class DllPairState;

	private:
		SonikLib::S_CAS::SonikAtomicLock m_lock;
		SonikLib::SharedSmtPtr<SonikAssignFreeNumber> m_asigned;
		DllPairState* DllHandleList;
		DllPairState* SentinelNode_Start;
		DllPairState* SentinelNode_End;
		uint64_t DllHandleListCnt;
		//アロケータ
		SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> m_allocator;

	private:
		//constructor
		inline SonikDllHandleManager(void)
		:DllHandleList(nullptr)
		,SentinelNode_Start(nullptr)
		,SentinelNode_End(nullptr)
		,DllHandleListCnt(0)
		{/*no process*/};

		//copy constructor
		SonikDllHandleManager(const SonikDllHandleManager& _copy_) = delete;
		SonikDllHandleManager(SonikDllHandleManager&& _move_) = delete;
		//override operator equal
		SonikDllHandleManager& operator =(const SonikDllHandleManager& _copy_) = delete;
		SonikDllHandleManager& operator =(SonikDllHandleManager&& _move_) = delete;

	public:
		//Destructor
		~SonikDllHandleManager(void);

		static bool CreateManager(SonikLib::SharedSmtPtr<SonikDllHandleManager>& _out_, uint64_t _dll_list_cnt_ = 100);
		static bool CreateManager(SonikLib::SharedSmtPtr<SonikDllHandleManager>& _out_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_, uint64_t _dll_list_cnt_ = 100);

		//マネージャ自体が管理しないDLLロードを行います。スマートポインタカウンタ0で読み込まれたDLLは自動的にFreeされます。
		//マネージャ自体が管理しないため、この関数後にGetHandleをしてもnullptrが帰ります。（というかそもそも読み込み番号返さないから引数が指定できないんだけどね)
		static bool DllGetLoad(const char* _DllPath_, SDllHandle& _GetDllObject_);
		static bool DllGetLoad(const char* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		static bool DllGetLoad(const char16_t* _DllPath_, SDllHandle& _GetDllObject_);
		static bool DllGetLoad(const char16_t* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);
		static bool DllGetLoad(const wchar_t* _DllPath_, SDllHandle& _GetDllObject_);
		static bool DllGetLoad(const wchar_t* _DllPath_, SDllHandle& _GetDllObject_, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);


		bool DllLoad(const char* _DllPath_, uint64_t& _out_dll_reg_number_);
		bool DllLoad(const char16_t* _DllPath_, uint64_t& _out_dll_reg_number_);
		bool DllLoad(const wchar_t* _DllPath_, uint64_t& _out_dll_reg_number_);

		void FreeDll(uint64_t _reg_number_);

		uintptr_t GetDllProcAddress(const SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const char* ProcName);
		uintptr_t GetDllProcAddress(const SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const char16_t* ProcName);
		uintptr_t GetDllProcAddress(const SONIK_DLL_ACCESS_MANAGER_POINTER dllhandle, const wchar_t* ProcName);

		SONIK_DLL_ACCESS_MANAGER_POINTER GetHandle(uint64_t _reg_number_);

	};

};



#endif /* SONIKDLLACCESSMANAGER_H_ */