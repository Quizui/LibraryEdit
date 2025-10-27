/*
 * SonikMM_AudioInfo.h
 *
 *  Created on: 2020/08/01
 *      Author: SONIC
 */

#ifndef MULTIMEDIA_SONIKMM_AUDIOINFO_H_
#define MULTIMEDIA_SONIKMM_AUDIOINFO_H_

namespace SonikLib
{

	class SonikMMAudioInfo
	{
	public:
		SonikMMAudioInfo(void)
		{
			//no process;
		};

		virtual ~SonikMMAudioInfo(void)
		{
			//no process;
		};

		//pure virtual fanction
		virtual unsigned long GetAudioInfo_Bit(void) =0;
		virtual unsigned long GetAudioInfo_SamplingRate(void) =0;
		virtual unsigned long GetAudioInfo_Channels(void) =0;

	};

}



#endif /* MULTIMEDIA_SONIKMM_AUDIOINFO_H_ */
