/*
 * WaveBitConvert.h
 *
 *  Created on: 2018/05/06
 *      Author: SONIC
 */

#ifndef LOADFUNCTION_WAVEBITCONVERT_H_
#define LOADFUNCTION_WAVEBITCONVERT_H_

#include <stdint.h>
#include "../AudioLoadTypedef.h"


namespace SonikAudioFormat
{
	class SonikAudioFormat_InnerSet;
};

namespace SonikAudioWAVEBitConvert
{

//	bool SonikWAVEConvert(SonikAudioFormat_WAVE::WaveFormat* RefGet_, SonikBitMode::AudioBitMode BitMode);

	bool SonikWAVEConvertSampling(SonikAudioFormat::SonikAudioFormat_InnerSet* ConvTarget, int64_t ConvertSamplingRate);

};



#endif /* LOADFUNCTION_WAVEBITCONVERT_H_ */
