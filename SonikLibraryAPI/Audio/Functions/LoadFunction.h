/*
 * LoadFunction.h
 *
 *  Created on: 2017/10/10
 *      Author: SONIC
 */

#ifndef AUDIOLOADFUNCTION_LOADFUNCTION_H_
#define AUDIOLOADFUNCTION_LOADFUNCTION_H_

#include "../AudioLoadTypedef.h"

namespace SonikAudioLoadFunction
{
	//char型でファイル名を指定し、WAVEファイルを読み込みます。
	bool Load_WaveFile(const char* LoadFilePass, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg = false);

	//メモリからWAVEファイルとして読み込みます。
	bool Load_WaveFileInMemory(const char* LoadFileData, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg = false);

	//wchar_t型でファイル名を指定し、WAVEファイルを読み込みます。
	bool Load_WaveFile(const char16_t* LoadFilePass, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg = false);

};



#endif /* AUDIOLOADFUNCTION_LOADFUNCTION_H_ */
