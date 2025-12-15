#pragma once

#ifndef SONIKFILESYSTEMS_H_
#define SONIKFILESYSTEMS_H_

#include <SmartPointer/SonikSmartPointer.hpp>
#include <SonikString/SonikString.h>

#include <cstdint>

 //前方宣言
namespace SonikLib
{
	namespace Container
	{
		template<class QueueType>
		class SonikAtomicQueue;
	};
};

namespace SonikLib
{

	namespace FileSystemGlobal
	{
		//指定したフォルダにあるファイル数を取得します。(サブディレクトリ及び、サブディレクトリ内のファイルはカウントに含まれません。
		uint64_t GetDirectoryInFileCount(SonikLib::SonikString  _directoryPath_);

		//指定したフォルダにあるファイル名を列挙します。
		bool GetDirectoryInFileNameEnumeration(SonikLib::SonikString  _directoryPath_, SonikLib::Container::SonikAtomicQueue<SonikLib::SonikString>& retEnums);
		bool GetDirectoryInFileNameEnumeration(SonikLib::SonikString  _directoryPath_, SonikLib::Container::SonikAtomicQueue<SonikLib::SonikString>& retEnums, SonikLib::AllocatorSharedSmtPtr<SonikLib::SLibAllocateInterface> _allocator_);

	};
};



#endif /* SONIKFILESYSTEMS_H_ */