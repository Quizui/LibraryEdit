/*
 * SonikFrameControl.cpp
 *
 *  Created on: 2016/05/23
 *      Author: SONIK
 */

#ifdef __SONIK_x86_64__

#include "SonikFrameControl.h"

#include <new>
#include <chrono>
#include <thread>

namespace SonikLib
{

	//メインクラスの実装==================
	SonikFrameControl::SonikFrameControl(void)
	:FrameRate(60)
	,FrameCount(0)
	,FrameStartTime(0)
	,OneFrameStartTime(0)
	,BeforFrameTime(0)
	{
//		p_OneFrameTime = new(std::nothrow) SonikFrameControl::FrameStamp[SampleNum];
//		if(p_OneFrameTime == 0)
//		{
//			throw std::bad_alloc();
//		};

	};

	SonikFrameControl::SonikFrameControl(uint64_t SetFrameRate)
	:FrameRate(SetFrameRate)
	,FrameCount(0)
//	,SampleNum(30)
//	,SampleAllNum(0)
//	,InsertPoint(0)
	,FrameStartTime(0)
	,OneFrameStartTime(0)
	,BeforFrameTime(0)
	{

//		if(SetFrameRate < SampleNum)
//		{
//			SampleNum = SetFrameRate;
//		};
//
//		p_OneFrameTime = new(std::nothrow) SonikFrameControl::FrameStamp[SampleNum];
//		if(p_OneFrameTime == 0)
//		{
//			throw std::bad_alloc();
//		};

	};

	SonikFrameControl::SonikFrameControl(uint64_t SetFrameRate, uint64_t SetSampleCount)
	:FrameRate(SetFrameRate)
	,FrameCount(0)
	,FrameStartTime(0)
	,OneFrameStartTime(0)
	,BeforFrameTime(0)
	{

//		if(SetFrameRate < SetSampleCount)
//		{
//			SampleNum = SetFrameRate;
//		};
//
//		p_OneFrameTime = new(std::nothrow) SonikFrameControl::FrameStamp[SampleNum];
//		if(p_OneFrameTime == 0)
//		{
//			throw std::bad_alloc();
//		};

	};

	SonikFrameControl::~SonikFrameControl(void)
	{
//		if(p_OneFrameTime !=0)
//		{
//			delete[] p_OneFrameTime;
//		};
	};

//	double SonikFrameControl::GetFps(void)
//	{
//		return static_cast<double>(SampleAllNum) / static_cast<double>(SampleNum);
//	};

	void SonikFrameControl::FrameSleep(void)
	{
		std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );
		int64_t waitVal;

		if(FrameCount == 0)
		{
			if(BeforFrameTime == 0)
			{
				waitVal = 0;

			}else
			{
				waitVal = FrameStartTime + 1000 - milli.count();
			};

		}else
		{
			waitVal = static_cast<long long>(FrameStartTime + FrameCount * (1000.0 / FrameRate)) - milli.count();

		};

		if(waitVal > 0)
		{
			std::this_thread::sleep_for( std::chrono::milliseconds(waitVal) );
			milli = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() );
		};

		if(FrameCount == 0)
		{
			FrameStartTime = milli.count();
		};

//		SampleAllNum -= p_OneFrameTime[InsertPoint].FrameVal;
//		p_OneFrameTime[InsertPoint].FrameVal = 1000.0 / (milli.count() - BeforFrameTime);
		BeforFrameTime = milli.count();

//		SampleAllNum += p_OneFrameTime[InsertPoint].FrameVal;
//
//		++InsertPoint;
//		if(InsertPoint == SampleNum)
//		{
//			InsertPoint = 0;
//		};

		++FrameCount;
		FrameCount = FrameCount % FrameRate;
	};

};

#endif
