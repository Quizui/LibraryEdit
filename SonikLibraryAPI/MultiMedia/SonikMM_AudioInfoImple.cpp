/*
 * SonikMM_AudioInfoImple.cpp
 *
 *  Created on: 2020/08/01
 *      Author: SONIC
 */

#include "SonikMM_AudioInfoImple.h"

namespace SonikLib
{
	AudioInfoImple::AudioInfoImple(void)
	:m_Bit(0)
	,m_SamplingRate(0)
	,m_Channels(0)
	{

	};

	AudioInfoImple::~AudioInfoImple(void)
	{

	};

	unsigned long AudioInfoImple::GetAudioInfo_Bit(void)
	{
		return m_Bit;
	};

	unsigned long AudioInfoImple::GetAudioInfo_SamplingRate(void)
	{
		return m_SamplingRate;
	};

	unsigned long AudioInfoImple::GetAudioInfo_Channels(void)
	{
		return m_Channels;
	};




};


