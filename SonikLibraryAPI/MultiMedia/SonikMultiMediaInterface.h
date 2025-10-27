/*
 * SonikMultiMediaInterface.h
 *
 *  Created on: 2020/07/25
 *      Author: SONIC
 */

#ifndef MULTIMEDIA_SONIKMULTIMEDIAINTERFACE_H_
#define MULTIMEDIA_SONIKMULTIMEDIAINTERFACE_H_

//c 共通ヘッダ
#include "../SmartPointer/SonikSmartPointer.hpp"
#include "../SonikCAS/SonikAtomicLock.h"
#include "SonikMM_AudioInfo.h"


namespace SonikLib
{

#if defined(_WIN32) || defined(_WIN64)
	//Windows用

	class SonikMultiMedia
	{
	private:
		S_CAS::SonikAtomicLock m_lock;
		long ComState;

	public:
		SonikMultiMedia(void);
		~SonikMultiMedia(void);

		//OSから受け取る情報をそのまま返却します。
		SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> Get_Hard_AudioInfo_OSSetting(void);
		//OpenALが最大限使用できるフォーマットで返却します。
		SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> Get_Hard_AudioInfo_OpenALSetting(void);

	};



#elif defined(__linux__)
	//linux用ヘッダ

#endif



};



#endif /* MULTIMEDIA_SONIKMULTIMEDIAINTERFACE_H_ */
