/*
 * WriteFunction.cpp
 *
 *  Created on: 2020/04/20
 *      Author: SONIC
 */

#include <fstream>
#include <iostream>

#include "./WriteFunction.h"

#include "../../SonikString/SonikString.h"
#include "../Format/SonikAudioFormat.h"
#include "../Format/SonikWindowsWaveFormat.h"


namespace SonikAudioWriteFunction
{

	//char型で指定したファイル名に、RefGet_のデータを書き出します。
	bool Write_WaveFile(const char* WriteFilePass, SonikAudio::SAudioFormat& WriteAudioData)
	{
		std::fstream local_fst;

		if( WriteAudioData->Get_AudioFormatType() != SonikAudioFormat::AFT::AFT_Wave )
		{
			return false;
		};

		//ファイルを読み取りモードのみでオープン試行
		local_fst.open(WriteFilePass, std::ios::in);
		if(local_fst.is_open())
		{
			//成功すればファイルが存在しているということなので、ファイルを一旦閉じて、読み書き追記モードで再度オープンする。
			local_fst.close();
			local_fst.open(WriteFilePass, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);

		}else
		{
			//失敗であれば、ファイルが存在していないので、(念のため)クローズをコールした後、新規作成 + 読み書きモードで再度オープンする。
			local_fst.close();
			local_fst.open(WriteFilePass, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
		};

		//その後何らかの理由でオープンが失敗すればfalseを返却
		if( !local_fst.is_open() )
		{
			return false;
		};

//		uint32_t rif_ = sizeof(SonikAudioStruct::RIFF_Chank);
//		uint32_t fmt_ = sizeof(SonikAudioStruct::FMT_Chank);
//		uint32_t dat_ = sizeof(SonikAudioStruct::DATA_Chank) - sizeof(char*);
		uint32_t rif_ = 12;
		uint32_t fmt_ = 24;
		uint32_t dat_ = 8;

		const SonikAudioFormatPlatForm::WindowsWaveFormat* winformat = reinterpret_cast<SonikAudioFormatPlatForm::WindowsWaveFormat*>(WriteAudioData.GetPointer());


		char* Data = new char[ rif_ + fmt_ + dat_ + winformat->Get_WaveDataSize() ];
		char* WritePoint = Data;

		//riff================================
		memcpy(WritePoint, winformat->GetRIFF_Format(), 4);
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->riff_.AllFileSize, 4);
		(*reinterpret_cast<unsigned long*>(WritePoint)) = winformat->GetRIFF_FileSize();
		WritePoint += 4;
		memcpy(WritePoint, winformat->GetRIFF_Type(), 4);
		WritePoint +=4;

		//fmt=================================(拡張無し)
		memcpy(WritePoint, winformat->GetFMT_Format(), 4);
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->fmt_.FormatSize, 4);
		(*reinterpret_cast<unsigned long*>(WritePoint)) = winformat->GetFMT_FormatSize();
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->fmt_.FormatCode, 2);
		(*reinterpret_cast<unsigned short*>(WritePoint)) = winformat->GetFMT_FormatCode();
		WritePoint += 2;
//		memcpy(WritePoint, &RefGet_->fmt_.Channel, 2);
		(*reinterpret_cast<unsigned short*>(WritePoint)) = winformat->GetFMT_Channels();
		WritePoint += 2;
//		memcpy(WritePoint, &RefGet_->fmt_.SamplingRate, 4);
		(*reinterpret_cast<unsigned long*>(WritePoint)) = winformat->GetFMT_SamplingRate();
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->fmt_.ProcSec, 4);
		(*reinterpret_cast<unsigned long*>(WritePoint)) = winformat->GetFMT_ByteProcSec();
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->fmt_.BlockBaundary, 2);
		(*reinterpret_cast<unsigned short*>(WritePoint)) = winformat->GetFMT_BlockBoundary();
		WritePoint += 2;
//		memcpy(WritePoint, &RefGet_->fmt_.BitsPerSample, 2);
		(*reinterpret_cast<unsigned short*>(WritePoint)) = winformat->GetFMT_BitsPerSample();
		WritePoint += 2;

		//data===============================
		memcpy(WritePoint, winformat->GetDATA_Format(), 4);
		WritePoint += 4;
//		memcpy(WritePoint, &RefGet_->data_.DataFileSize, 4);
		(*reinterpret_cast<unsigned long*>(WritePoint)) = winformat->Get_WaveDataSize();
		WritePoint += 4;
		memcpy(WritePoint, winformat->Get_WaveData(), winformat->Get_WaveDataSize());


		//書き込みよー
		local_fst.write(Data, rif_ + fmt_ + dat_ + winformat->Get_WaveDataSize());

		return true;
	};

	//char16_t型で指定したファイル名に、RefGet_のデータを書き出します。
	bool Write_WaveFile(const char16_t* WriteFilePass, const SonikAudio::SAudioFormat& WriteAudioData)
	{
		return false;
	}

};
