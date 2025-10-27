/*
 * SonikWindowsWaveFormat.h
 *
 *  Created on: 2020/04/23
 *      Author: SONIC
 */

//Windowsで使用される汎用PCMのWaveフォーマットを表現します。

#ifndef AUDIOFORMAT_SONIKWINDOWSWAVEFORMAT_H_
#define AUDIOFORMAT_SONIKWINDOWSWAVEFORMAT_H_

#include "./SonikAudioFormat_InnerUse.h"

namespace SonikAudioFormatPlatForm
{
	class WindowsWaveFormat : public SonikAudioFormat::SonikAudioFormat_InnerSet
	{
	private:
		class RIFF_chank
		{
		public:
				char			Format[5];		//RIFF　とか。
				unsigned long	AllFileSize;	//本チャンクを含めた総ファイルサイズ　- 8Byte らしい。
				char			Type[5];		//WAVE とか。
		};
		class FMT_chank
		{
		public:
			char				Format[5];		//'fmt ' とか。
			unsigned long		FormatSize;		//フォーマットサイズ(デフォルト16らしい)
			unsigned short	FormatCode;		//フォーマットコード (非圧縮PCMは、１)
			unsigned short	Channel;		//チャンネル数モノラル =1, ステレオ =2
			unsigned long		SamplingRate;	//サンプリングレート (44.1kHz なら 44100)
			unsigned long		ProcSec;		//バイト/秒	１秒間に録音に必要なバイト数
			unsigned short	BlockBaundary;	//ブロック境界	ステレオ16bit なら 16bit * 2 = 32bit = 4Byte
			unsigned short	BitsPerSample;	//ビット/サンプル	1サンプルに必要なビット数
		};
		class DATA_chank
		{
		public:
			char 			Format[5];				//'data' とか
			//親クラスが持っているので。
//			unsigned long DataFileSize;	//総ファイルサイズ -126 らしい。
//			char* 			WaveData;				//波形データ本体
		};

	private:
		RIFF_chank	riff_;
		FMT_chank	fmt_;
		DATA_chank	data_;


	public:
		//constructor
		WindowsWaveFormat(void);
		//Destructor
		virtual ~WindowsWaveFormat(void);

		//Set_Get Values
		//RIFF_Set
		void SetRIFF_Format(char* SetFormat);
		void SetRIFF_FileSize(unsigned long SetSize);
		void SetRIFF_Type(char* SetType);
		//RIFF_Get
		const char*	GetRIFF_Format(void) const ;
		unsigned long	GetRIFF_FileSize(void) const;
		const char*	GetRIFF_Type(void) const;

		//FMT_Set
		void SetFMT_Format(char* SetFormat);
		void SetFMT_FormatSize(unsigned long SetFormatSize);
		void SetFMT_FormatCode(unsigned short SetFormatCode);
		void SetFMT_Channels(unsigned short SetChannels);
		void SetFMT_SamplingRate(unsigned long SetSamplingRate);
		void SetFMT_ByteProcSec(unsigned long SetProcSec);
		void SetFMT_BlockBoundary(unsigned short SetBlockBoundary);
		void SetFMT_BitsPerSample(unsigned short SetPerSample);
		//FMT_Get
		const char*		GetFMT_Format(void) const;
		unsigned long		GetFMT_FormatSize(void) const;
		unsigned short	GetFMT_FormatCode(void) const;
		unsigned short	GetFMT_Channels(void) const;
		long long		GetFMT_SamplingRate(void) const;
		unsigned long		GetFMT_ByteProcSec(void) const;
		unsigned short	GetFMT_BlockBoundary(void) const;
		unsigned short	GetFMT_BitsPerSample(void) const;


		//DATA_Set
		void SetDATA_Format(char* SetFormat);
		void SetDATA_FileSize(unsigned long SetDataFileSize);
		void SetDATA_WaveData(char* SetWaveData);
		//DATA_Get
		const char*	GetDATA_Format(void) const;



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


}



#endif /* AUDIOFORMAT_SONIKWINDOWSWAVEFORMAT_H_ */
