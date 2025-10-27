/*
 * SonikWindowsWaveFormat.cpp
 *
 *  Created on: 2020/04/23
 *      Author: SONIC
 */

#include "./SonikWindowsWaveFormat.h"

namespace SonikAudioFormatPlatForm
{

	//constructor
	WindowsWaveFormat::WindowsWaveFormat(void)
	{
		//no process
	};

	//Destructor
	WindowsWaveFormat::~WindowsWaveFormat(void)
	{
		//no process
	}

	//Set_Get Values
	//RIFF_Set
	void WindowsWaveFormat::SetRIFF_Format(char* SetFormat)
	{
		riff_.Format[0] = SetFormat[0];
		riff_.Format[1] = SetFormat[1];
		riff_.Format[2] = SetFormat[2];
		riff_.Format[3] = SetFormat[3];
		riff_.Format[4] = '\0';

	};

	void WindowsWaveFormat::SetRIFF_FileSize(unsigned long SetSize)
	{
		riff_.AllFileSize = SetSize;

	};

	void WindowsWaveFormat::SetRIFF_Type(char* SetType)
	{
		riff_.Type[0] = SetType[0];
		riff_.Type[1] = SetType[1];
		riff_.Type[2] = SetType[2];
		riff_.Type[3] = SetType[3];
		riff_.Type[4] = '\0';

	};

	//RIFF_Get
	const char* WindowsWaveFormat::GetRIFF_Format(void) const
	{
		return riff_.Format;
	};

	unsigned long	WindowsWaveFormat::GetRIFF_FileSize(void) const
	{
		return riff_.AllFileSize;
	};

	const char* WindowsWaveFormat::GetRIFF_Type(void) const
	{
		return riff_.Type;
	};




	//FMT_Set
	void WindowsWaveFormat::SetFMT_Format(char* SetFormat)
	{
		fmt_.Format[0] = SetFormat[0];
		fmt_.Format[1] = SetFormat[1];
		fmt_.Format[2] = SetFormat[2];
		fmt_.Format[3] = SetFormat[3];
		fmt_.Format[4] = '\0';

	};

	void WindowsWaveFormat::SetFMT_FormatSize(unsigned long SetFormatSize)
	{
		fmt_.FormatSize = SetFormatSize;

	};

	void WindowsWaveFormat::SetFMT_FormatCode(unsigned short SetFormatCode)
	{
		fmt_.FormatCode = SetFormatCode;

	};

	void WindowsWaveFormat::SetFMT_Channels(unsigned short SetChannels)
	{
		fmt_.Channel = SetChannels;

	};

	void WindowsWaveFormat::SetFMT_SamplingRate(unsigned long SetSamplingRate)
	{
		fmt_.SamplingRate = SetSamplingRate;

	};

	void WindowsWaveFormat::SetFMT_ByteProcSec(unsigned long SetProcSec)
	{
		fmt_.ProcSec = SetProcSec;

	};

	void WindowsWaveFormat::SetFMT_BlockBoundary(unsigned short SetBlockBoundary)
	{
		fmt_.BlockBaundary = SetBlockBoundary;

	};

	void WindowsWaveFormat::SetFMT_BitsPerSample(unsigned short SetPerSample)
	{
		fmt_.BitsPerSample = SetPerSample;

	};

	//FMT_Get
	const char* WindowsWaveFormat::GetFMT_Format(void) const
	{
		return fmt_.Format;
	};

	unsigned long WindowsWaveFormat::GetFMT_FormatSize(void) const
	{
		return fmt_.FormatSize;
	};

	unsigned short WindowsWaveFormat::GetFMT_FormatCode(void) const
	{
		return fmt_.FormatCode;
	};

	unsigned short WindowsWaveFormat::GetFMT_Channels(void) const
	{
		return fmt_.Channel;
	};

	long long WindowsWaveFormat::GetFMT_SamplingRate(void) const
	{
		return fmt_.SamplingRate;
	};

	unsigned long WindowsWaveFormat::GetFMT_ByteProcSec(void) const
	{
		return fmt_.ProcSec;
	};

	unsigned short WindowsWaveFormat::GetFMT_BlockBoundary(void) const
	{
		return fmt_.BlockBaundary;
	};

	unsigned short WindowsWaveFormat::GetFMT_BitsPerSample(void) const
	{
		return fmt_.BitsPerSample;
	};

	//DATA_Set
	void WindowsWaveFormat::SetDATA_Format(char* SetFormat)
	{
		data_.Format[0] = SetFormat[0];
		data_.Format[1] = SetFormat[1];
		data_.Format[2] = SetFormat[2];
		data_.Format[3] = SetFormat[3];
		data_.Format[4] = '\0';

	};

	void WindowsWaveFormat::SetDATA_FileSize(unsigned long SetDataFileSize)
	{
		WaveDataSize = SetDataFileSize;

	};

	void WindowsWaveFormat::SetDATA_WaveData(char* SetWaveData)
	{
		WaveData = SetWaveData;

	};

	//DATA_Get
	const char* WindowsWaveFormat::GetDATA_Format(void) const
	{
		return data_.Format;
	};

	//Pure Function Implement===============================
	//FMTチャンク情報のフォーマットのサイズを設定します。
	void WindowsWaveFormat::Set_FormatSize(unsigned long SetSize)
	{
		fmt_.FormatSize = SetSize;
	};

	//FMTチャンク情報のフォーマットのコード(Type)を設定します。
	void WindowsWaveFormat::Set_FormatCode(unsigned short SetCode)
	{
		fmt_.FormatCode = SetCode;
	};

	//FMTチャンク情報のチャンネル数を設定します。
	void WindowsWaveFormat::Set_FormatChannel(unsigned short SetCh)
	{
		fmt_.Channel = SetCh;
	};

	//FMTチャンク情報のサンプリング周波数を設定します。
	void WindowsWaveFormat::Set_FormatSamplingRate(unsigned long SetSamplingRate)
	{
		fmt_.SamplingRate = SetSamplingRate;
	};

	//FMTチャンク情報の1秒間の処理に必要なバイト数を設定します。
	void WindowsWaveFormat::Set_FormatProcSec(unsigned long SetProcSec)
	{
		fmt_.ProcSec = SetProcSec;
	};

	//FMTチャンク情報のブロック教会情報を設定します。
	void WindowsWaveFormat::Set_FormatBlockBaundary(unsigned short SetBlockBaundary)
	{
		fmt_.BlockBaundary = SetBlockBaundary;
	};

	//FMTチャンク情報の1サンプルに必要なビット数を設定します。
	void WindowsWaveFormat::Set_FormatBitsPerSample(unsigned short SetBitsPerSample)
	{
		fmt_.BitsPerSample = SetBitsPerSample;
	};

	//フォーマットタイプを設定します（WAVE や AIFF構造などの把握)
	void WindowsWaveFormat::Set_AudioFormatType(unsigned int SetFormatType)
	{
		AudioFormatType = SetFormatType;
	};

	//波形データのサイズを設定します。
	void WindowsWaveFormat::Set_WaveDataSize(unsigned long SetDataSize)
	{
		WaveDataSize = SetDataSize;
	};

	//波形データの先頭ポインタを設定します。
	void WindowsWaveFormat::Set_WaveData(char* SetWaveDataPointer)
	{
		WaveData = SetWaveDataPointer;
	};


	//FMTチャンク情報のフォーマットのサイズを取得します。
	unsigned long WindowsWaveFormat::Get_FormatSize(void) const
	{

		return fmt_.FormatSize;
	};

	//FMTチャンク情報のフォーマットのコード(Type)を取得します。
	unsigned short WindowsWaveFormat::Get_FormatCode(void) const
	{

		return fmt_.FormatCode;
	};

	//FMTチャンク情報のチャンネル数を取得します。
	unsigned short WindowsWaveFormat::Get_FormatChannel(void) const
	{

		return fmt_.Channel;
	};

	//FMTチャンク情報のサンプリング周波数を取得します。
	unsigned long WindowsWaveFormat::Get_FormatSamplingRate(void) const
	{

		return fmt_.SamplingRate;
	};

	//FMTチャンク情報の1秒間の処理に必要なバイト数を取得します。
	unsigned long WindowsWaveFormat::Get_FormatProcSec(void) const
	{

		return fmt_.ProcSec;
	};

	//FMTチャンク情報のブロック教会情報を取得します。
	unsigned short WindowsWaveFormat::Get_FormatBlockBaundary(void) const
	{

		return fmt_.BlockBaundary;
	};

	//FMTチャンク情報の1サンプルに必要なビット数を取得します。
	unsigned short WindowsWaveFormat::Get_FormatBitsPerSample(void) const
	{

		return fmt_.BitsPerSample;
	};



};


