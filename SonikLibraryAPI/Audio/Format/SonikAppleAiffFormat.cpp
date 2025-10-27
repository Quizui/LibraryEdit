/*
 * SonikAppleAiffFormat.cpp
 *
 *  Created on: 2020/04/23
 *      Author: SONIC
 */

#include "./SonikAppleAiffFormat.h"

namespace SonikAudioFormatPlatForm
{
//Appleへの移植をするかわからんので未実装です。

	//constructor
	AppleAiffFormat::AppleAiffFormat(void)
	{

	};

	//Destructor
	AppleAiffFormat::~AppleAiffFormat(void)
	{

	};

	//Pure Function Implement===============================
	//FMTチャンク情報のフォーマットのサイズを設定します。
	void AppleAiffFormat::Set_FormatSize(unsigned long SetSize)
	{

	};

	//FMTチャンク情報のフォーマットのコード(Type)を設定します。
	void AppleAiffFormat::Set_FormatCode(unsigned short SetCode)
	{

	};

	//FMTチャンク情報のチャンネル数を設定します。
	void AppleAiffFormat::Set_FormatChannel(unsigned short SetCh)
	{

	};

	//FMTチャンク情報のサンプリング周波数を設定します。
	void AppleAiffFormat::Set_FormatSamplingRate(unsigned long SetSamplingRate)
	{

	};

	//FMTチャンク情報の1秒間の処理に必要なバイト数を設定します。
	void AppleAiffFormat::Set_FormatProcSec(unsigned long SetProcSec)
	{

	};

	//FMTチャンク情報のブロック教会情報を設定します。
	void AppleAiffFormat::Set_FormatBlockBaundary(unsigned short SetBlockBaundary)
	{

	};

	//FMTチャンク情報の1サンプルに必要なビット数を設定します。
	void AppleAiffFormat::Set_FormatBitsPerSample(unsigned short SetBitsPerSample)
	{

	};

	//フォーマットタイプを設定します（WAVE や AIFF構造などの把握)
	void AppleAiffFormat::Set_AudioFormatType(unsigned int SetFormatType)
	{
		AudioFormatType = SetFormatType;
	};

	//波形データのサイズを設定します。
	void AppleAiffFormat::Set_WaveDataSize(unsigned long SetDataSize)
	{
		WaveDataSize = SetDataSize;
	};

	//波形データの先頭ポインタを設定します。
	void AppleAiffFormat::Set_WaveData(char* SetWaveDataPointer)
	{
		WaveData = SetWaveDataPointer;
	};


	//FMTチャンク情報のフォーマットのサイズを取得します。
	unsigned long AppleAiffFormat::Get_FormatSize(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報のフォーマットのコード(Type)を取得します。
	unsigned short AppleAiffFormat::Get_FormatCode(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報のチャンネル数を取得します。
	unsigned short AppleAiffFormat::Get_FormatChannel(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報のサンプリング周波数を取得します。
	unsigned long AppleAiffFormat::Get_FormatSamplingRate(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報の1秒間の処理に必要なバイト数を取得します。
	unsigned long AppleAiffFormat::Get_FormatProcSec(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報のブロック教会情報を取得します。
	unsigned short AppleAiffFormat::Get_FormatBlockBaundary(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

	//FMTチャンク情報の1サンプルに必要なビット数を取得します。
	unsigned short AppleAiffFormat::Get_FormatBitsPerSample(void) const
	{
		return 0; //警告消すためにさしあたり。
	};

};

