/*
 * SonikFrameControl.h
 *
 *  Created on: 2016/05/23
 *      Author: SONIK
 */

#ifndef SONIKFRAMECONTROL_SONIKFRAMECONTROL_H_
#define SONIKFRAMECONTROL_SONIKFRAMECONTROL_H_

#include <cstdint>

namespace SonikLib
{
#ifdef __SONIK_I686__
	class SonikFrameControl
	{
	private:
		unsigned long FrameRate;	//フレームレート
		unsigned long FrameCount;	//フレームカウント

		unsigned long long FrameStartTime;	//1秒間のスタートタイム。
		unsigned long long OneFrameStartTime;	//1フレームのスタートタイム。
		unsigned long long BeforFrameTime;		//1フレーム前の時間。

	public:
		SonikFrameControl(void);
		SonikFrameControl(unsigned long SetFrameRate);
		SonikFrameControl(unsigned long SetFrameRate, unsigned long SetSampleCount);

		~SonikFrameControl(void);

//		double GetFps(void);

		void FrameSleep(void);

	};

#elif defined(__SONIK_x86_64__)
	class SonikFrameControl
	{
	private:
		uint64_t FrameRate;	//フレームレート
		uint64_t FrameCount;	//フレームカウント

		uint64_t FrameStartTime;	//1秒間のスタートタイム。
		uint64_t OneFrameStartTime;	//1フレームのスタートタイム。
		uint64_t BeforFrameTime;		//1フレーム前の時間。

	public:
		SonikFrameControl(void);
		SonikFrameControl(uint64_t SetFrameRate);
		SonikFrameControl(uint64_t SetFrameRate, uint64_t SetSampleCount);

		~SonikFrameControl(void);

//		double GetFps(void);

		void FrameSleep(void);

	};

#endif

};



#endif /* SONIKFRAMECONTROL_SONIKFRAMECONTROL_H_ */
