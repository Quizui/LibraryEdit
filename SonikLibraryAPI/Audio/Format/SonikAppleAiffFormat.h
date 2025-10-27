/*
 * SonikAppleAiffFormat.h
 *
 *  Created on: 2020/04/23
 *      Author: SONIC
 */

//Apple で使用される汎用PCM のAIFFフォーマットを表現します。

#ifndef AUDIOFORMAT_SONIKAPPLEAIFFFORMAT_H_
#define AUDIOFORMAT_SONIKAPPLEAIFFFORMAT_H_

//#include "SonikAudioFormat.h"
#include "./SonikAudioFormat_InnerUse.h"

namespace SonikAudioFormatPlatForm
{
	//Appleへの移植をするかわからんので未実装です。
	class AppleAiffFormat : public SonikAudioFormat::SonikAudioFormat_InnerSet
	{
	private:


	public:
		//constructor
		AppleAiffFormat(void);
		//Destructor
		~AppleAiffFormat(void);

		//Pure Function Implement===============================
		//FMTチャンク情報のフォーマットのサイズを設定します。
		void Set_FormatSize(unsigned long SetSize);
		//FMTチャンク情報のフォーマットのコード(Type)を設定します。
		void Set_FormatCode(unsigned short SetCode);
		//FMTチャンク情報のチャンネル数を設定します。
		void Set_FormatChannel(unsigned short SetCh);
		//FMTチャンク情報のサンプリング周波数を設定します。
		void Set_FormatSamplingRate(unsigned long SetSamplingRate);
		//FMTチャンク情報の1秒間の処理に必要なバイト数を設定します。
		void Set_FormatProcSec(unsigned long SetProcSec);
		//FMTチャンク情報のブロック教会情報を設定します。
		void Set_FormatBlockBaundary(unsigned short SetBlockBaundary);
		//FMTチャンク情報の1サンプルに必要なビット数を設定します。
		void Set_FormatBitsPerSample(unsigned short SetBitsPerSample);

		//フォーマットタイプを設定します（WAVE や AIFF構造などの把握)
		void Set_AudioFormatType(unsigned int SetFormatType);
		//波形データのサイズを設定します。
		void Set_WaveDataSize(unsigned long SetDataSize);
		//波形データの先頭ポインタを設定します。
		void Set_WaveData(char* SetWaveDataPointer);


		//FMTチャンク情報のフォーマットのサイズを取得します。
		unsigned long Get_FormatSize(void) const;
		//FMTチャンク情報のフォーマットのコード(Type)を取得します。
		unsigned short Get_FormatCode(void) const;
		//FMTチャンク情報のチャンネル数を取得します。
		unsigned short Get_FormatChannel(void) const;
		//FMTチャンク情報のサンプリング周波数を取得します。
		unsigned long Get_FormatSamplingRate(void) const;
		//FMTチャンク情報の1秒間の処理に必要なバイト数を取得します。
		unsigned long Get_FormatProcSec(void) const;
		//FMTチャンク情報のブロック教会情報を取得します。
		unsigned short Get_FormatBlockBaundary(void) const;
		//FMTチャンク情報の1サンプルに必要なビット数を取得します。
		unsigned short Get_FormatBitsPerSample(void) const;
	};

};


#endif /* AUDIOFORMAT_SONIKAPPLEAIFFFORMAT_H_ */
