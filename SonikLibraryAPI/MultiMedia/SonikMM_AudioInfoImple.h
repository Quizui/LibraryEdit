/*
 * SonikMM_AudioInfo_Win.h
 *
 *  Created on: 2020/08/01
 *      Author: SONIC
 */

#ifndef MULTIMEDIA_SONIKMM_AUDIOINFOIMPLE_H_
#define MULTIMEDIA_SONIKMM_AUDIOINFOIMPLE_H_

#include "SonikMM_AudioInfo.h"

namespace SonikLib
{
	class AudioInfoImple : public SonikLib::SonikMMAudioInfo
	{
	public:
		unsigned long m_Bit;
		unsigned long m_SamplingRate;
		unsigned long m_Channels;

	public:
		AudioInfoImple(void);
		~AudioInfoImple(void);

		unsigned long GetAudioInfo_Bit(void);
		unsigned long GetAudioInfo_SamplingRate(void);
		unsigned long GetAudioInfo_Channels(void);


	};

}


#endif /* MULTIMEDIA_SONIKMM_AUDIOINFOIMPLE_H_ */
