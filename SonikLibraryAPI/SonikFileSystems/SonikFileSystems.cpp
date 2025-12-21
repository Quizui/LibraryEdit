
#include "SonikFileSystems.h"

#include <Container/SonikAtomicQueue.hpp>

#if defined(_WIN64)
//WindowsAPIFunction
#include <Windows.h>

#elif defined(__linux__)
//LinuxAPIFunction

#endif

namespace SonikLibFileSystems
{
	namespace FileSystemGlobal
	{
		//指定したフォルダにあるファイル数を取得します。(サブディレクトリ及び、サブディレクトリ内のファイルはカウントに含まれません。
		uint64_t GetDirectoryInFileCount(SonikLib::SonikStringDefault  _directoryPath_)
		{
			//環境依存していない共通カウント変数
			uint64_t l_cnt = 0;

#if defined(_WIN64)
			//WindowsAPIFunction
			WIN32_FIND_DATAW l_data;
			HANDLE l_handle;

			_directoryPath_ += "/*";

			l_handle = FindFirstFileW(reinterpret_cast<LPCWSTR>(_directoryPath_.str_wchar()), &l_data);

			if (l_handle == INVALID_HANDLE_VALUE)
			{
				//Error
				return 0xFFFFFFFFFFFFFFFF;
			};

			while (FindNextFileW(l_handle, &l_data))
			{
				//ディレクトリ属性ならカウントしない。
				if ((l_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				};
				++l_cnt;
			};

			//Close
			FindClose(l_handle);

#elif defined(__linux__)
			//LinuxAPIFunction

#endif

//返却
			return l_cnt;
		};

		//指定したフォルダにあるファイル名を列挙します。
		bool GetDirectoryInFileNameEnumeration(SonikLib::SonikStringDefault  _directoryPath_, SonikLib::Container::SonikAtomicQueue<SonikLib::SonikStringDefault>& retEnums)
		{
#if defined(_WIN64)
			//WindowsAPIFunction
			WIN32_FIND_DATAW l_data;
			HANDLE l_handle;

			_directoryPath_ += "/*";

			l_handle = FindFirstFileW(reinterpret_cast<LPCWSTR>(_directoryPath_.str_wchar()), &l_data);
			if (l_handle == INVALID_HANDLE_VALUE)
			{
				//Error
				return false;
			};

			SonikLib::SonikStringDefault l_filename;
			l_filename = l_data.cFileName;

			retEnums.EnQueue(l_filename);

			while (FindNextFileW(l_handle, &l_data))
			{
				//ディレクトリ属性ならカウントしない。
				if ((l_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				};

				l_filename = l_data.cFileName;
				retEnums.EnQueue(l_filename);
			};

			//Close
			FindClose(l_handle);

#elif defined(__linux__)
			//LinuxAPIFunction

#endif

			return true;
		};
		bool GetDirectoryInFileNameEnumeration(SonikLib::SonikStringDefault  _directoryPath_, SonikLib::Container::SonikAtomicQueue<SonikLib::SonikStringDefault>& retEnums, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_)
		{
#if defined(_WIN64)
			//WindowsAPIFunction
			WIN32_FIND_DATAW l_data;
			HANDLE l_handle;

			_directoryPath_ += "/*";

			l_handle = FindFirstFileW(reinterpret_cast<LPCWSTR>(_directoryPath_.str_wchar()), &l_data);
			if (l_handle == INVALID_HANDLE_VALUE)
			{
				//Error
				return false;
			};

			SonikLib::SonikStringDefault l_filename(_allocator_);
			l_filename = l_data.cFileName;

			retEnums.EnQueue(l_filename);

			while (FindNextFileW(l_handle, &l_data))
			{
				//ディレクトリ属性ならカウントしない。
				if ((l_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				};

				l_filename = l_data.cFileName;
				retEnums.EnQueue(l_filename);
			};

			//Close
			FindClose(l_handle);

#elif defined(__linux__)
			//LinuxAPIFunction

#endif

			return true;
		};
	};
};