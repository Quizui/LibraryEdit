/*
 * SonikAudioFormat_InnerUse.h
 *
 *  Created on: 2020/05/07
 *      Author: SONIC
 */

#ifndef AUDIOFORMAT_SONIKAUDIOFORMAT_INNERUSE_H_
#define AUDIOFORMAT_SONIKAUDIOFORMAT_INNERUSE_H_

#include "./SonikAudioFormat.h"

//このライブラリ内だけで受け渡すためのフォーマットクラスです。
//外部公開するものとは別にセットを所持しています。
namespace SonikAudioFormat
{
	class SonikAudioFormat_InnerSet : public SonikAudioFormat::SonikAudioFormat
	{
	public:
		SonikAudioFormat_InnerSet(void)
		{
			// no process
		};
		virtual ~SonikAudioFormat_InnerSet(void)
		{
			// no process
		};

		//FMTチャンク情報のフォーマットのサイズを設定します。
		virtual void Set_FormatSize(unsigned long) =0;
		//FMTチャンク情報のフォーマットのコード(Type)を設定します。
		virtual void Set_FormatCode(unsigned short) =0;
		//FMTチャンク情報のチャンネル数を設定します。
		virtual void Set_FormatChannel(unsigned short) =0;
		//FMTチャンク情報のサンプリング周波数を設定します。
		virtual void Set_FormatSamplingRate(unsigned long) =0;
		//FMTチャンク情報の1秒間の処理に必要なバイト数を設定します。
		virtual void Set_FormatProcSec(unsigned long) =0;
		//FMTチャンク情報のブロック教会情報を設定します。
		virtual void Set_FormatBlockBaundary(unsigned short) =0;
		//FMTチャンク情報の1サンプルに必要なビット数を設定します。
		virtual void Set_FormatBitsPerSample(unsigned short) =0;

		//フォーマットタイプを設定します（WAVE や AIFF構造などの把握)
		virtual void Set_AudioFormatType(unsigned int) =0;
		//波形データのサイズを取得します。
		virtual void Set_WaveDataSize(unsigned long) =0;
		//波形データの先頭ポインタを取得します。
		virtual void Set_WaveData(char*) =0;

	};

};

#endif /* AUDIOFORMAT_SONIKAUDIOFORMAT_INNERUSE_H_ */
