/*
 * SonikAudioFormatBridge.h
 *
 *  Created on: 2017/11/07
 *      Author: SONIC
 */

#ifndef AUDIOFORMAT_SONIKAUDIOFORMAT_H_
#define AUDIOFORMAT_SONIKAUDIOFORMAT_H_


namespace SonikAudioFormat
{
	enum AFT
	{
		AFT_Unknown = 0x00,
		AFT_Wave,
		AFT_Aiff,
	};

	class SonikAudioFormat
	{
	protected:
		unsigned long WaveDataSize;
		unsigned int AudioFormatType;
		char* WaveData;


	public:
		SonikAudioFormat(void)
		:WaveDataSize(0)
		,AudioFormatType(AFT::AFT_Unknown)
		,WaveData(nullptr)
		{
			//no process
		};

		virtual ~SonikAudioFormat(void)
		{
			if(WaveData == nullptr)
			{
				delete[] WaveData;
			}
		};

		//FMTチャンク情報のフォーマットのサイズを取得します。
		virtual unsigned long Get_FormatSize(void) const =0;
		//FMTチャンク情報のフォーマットのコード(Type)を取得します。
		virtual unsigned short Get_FormatCode(void) const =0;
		//FMTチャンク情報のチャンネル数を取得します。
		virtual unsigned short Get_FormatChannel(void) const =0;
		//FMTチャンク情報のサンプリング周波数を取得します。
		virtual unsigned long Get_FormatSamplingRate(void) const =0;
		//FMTチャンク情報の1秒間の処理に必要なバイト数を取得します。
		virtual unsigned long Get_FormatProcSec(void) const =0;
		//FMTチャンク情報のブロック教会情報を取得します。
		virtual unsigned short Get_FormatBlockBaundary(void) const =0;
		//FMTチャンク情報の1サンプルに必要なビット数を取得します。
		virtual unsigned short Get_FormatBitsPerSample(void) const =0;

		//フォーマットタイプを取得します（WAVE や AIFF構造などの把握)
		unsigned int Get_AudioFormatType(void) const
		{
			return AudioFormatType;
		};

		//波形データのサイズを取得します。
		unsigned long Get_WaveDataSize(void) const
		{
			return WaveDataSize;
		};

		//波形データの先頭ポインタを取得します。
		const char* Get_WaveData(void) const
		{
			return WaveData;
		};


	};

};



#endif /* AUDIOFORMAT_SONIKAUDIOFORMAT_H_ */
