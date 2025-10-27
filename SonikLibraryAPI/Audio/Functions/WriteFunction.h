/*
 * WriteFunction.h
 *
 *  Created on: 2020/04/20
 *      Author: SONIC
 */

//Audioデータを書き出します。

#ifndef LOADFUNCTION_WRITEFUNCTION_H_
#define LOADFUNCTION_WRITEFUNCTION_H_

#include "../AudioLoadTypedef.h"

namespace SonikAudioWriteFunction
{
	//char型で指定したファイル名に、RefGet_のデータを書き出します。
	bool Write_WaveFile(const char* WriteFilePass, SonikAudio::SAudioFormat& WriteAudioData);

	//char16_t型で指定したファイル名に、RefGet_のデータを書き出します。
	bool Write_WaveFile(const char16_t* WriteFilePass, const SonikAudio::SAudioFormat& WriteAudioData);

};





#endif /* LOADFUNCTION_WRITEFUNCTION_H_ */
