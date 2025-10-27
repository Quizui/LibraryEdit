/*
 * LoadFunction.cpp
 *
 *  Created on: 2017/10/10
 *      Author: SONIC
 */

#include <fstream>
#include <stdio.h>
#include <map>

#include "./LoadFunction.h"

#include "../../SonikString/SonikString.h"

#include "../Format/SonikWindowsWaveFormat.h"
#include "..//Functions/WaveBitConvert.h"

namespace SonikAudioLoadFunction
{
	//char型でファイル名を指定し、WAVEファイルを読み込みます。
	bool Load_WaveFile(const char* LoadFilePass, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg)
	{

		std::ifstream ifs_;

		ifs_.open(LoadFilePass, std::ios::binary);
		if( !ifs_.is_open() )
		{
			return false;
		};

		char* buffers;
		char* buffcontrol;
		unsigned int beg_pos;
		unsigned int eof_pos;

		ifs_.seekg(0, std::fstream::end);
		eof_pos = ifs_.tellg();

		ifs_.clear();

		ifs_.seekg(0, std::fstream::beg);
		beg_pos = ifs_.tellg();

		buffers = new(std::nothrow) char[ eof_pos - beg_pos ];
		if(buffers == 0)
		{
			return false;
		};

		//ファイルコピー
		ifs_.read(buffers, (eof_pos - beg_pos));
		//あとはリードファイルはいらないのでクローズ
		ifs_.close();


		SonikAudioFormatPlatForm::WindowsWaveFormat* pWave = nullptr;
		pWave = new(std::nothrow) SonikAudioFormatPlatForm::WindowsWaveFormat;
		if(pWave == 0)
		{
			delete[] buffers;
			return false;
		};

		//読み進めつつデータを入れていく。
		std::string str_;
		buffcontrol = buffers;

		//RIFF情報取得======================================
//		memcpy(RefGet_->riff_.Format, buffcontrol, 4);
//		RefGet_->riff_.Format[4] = '\0';
		pWave->SetRIFF_Format(buffcontrol);
		buffcontrol += 4;


		str_ = pWave->GetRIFF_Format();
		if(str_ != "RIFF")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};


//		RefGet_->riff_.AllFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetRIFF_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);


//		memcpy(RefGet_->riff_.Type, buffcontrol, 4);
//		RefGet_->riff_.Type[4] = '\0';
		pWave->SetRIFF_Type(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetRIFF_Type();
		if(str_ != "WAVE")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		//FMT情報取得======================================
//		memcpy(RefGet_->fmt_.Format, buffcontrol, 4);
//		RefGet_->fmt_.Format[4] = '\0';
		pWave->SetFMT_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetFMT_Format();
		if(str_ != "fmt ")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

//		RefGet_->fmt_.FormatSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_FormatSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

//		unsigned long controlsize = RefGet_->fmt_.FormatSize;
		unsigned long controlsize = pWave->GetFMT_FormatSize();

//		RefGet_->fmt_.FormatCode = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_FormatCode( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.Channel = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_Channels( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.SamplingRate = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_SamplingRate( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.ProcSec = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_ByteProcSec( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.BlockBaundary = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BlockBoundary( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.BitsPerSample = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BitsPerSample( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

		if(controlsize != 0)
		{
			buffcontrol += controlsize;
		};

		char* endp = buffers + (eof_pos - beg_pos);
		unsigned long tval = 0;
		//DATAチャンク情報がくるまで読み進める。
		for(/*省略*/; buffcontrol != endp  ; ++buffcontrol )
		{
			if( (*buffcontrol) == 'd' )
			{
				tval = (*reinterpret_cast<unsigned long*>(buffcontrol));
				if( tval == 0x61746164 ) //'d''a''t''a'
				{
					break;
				};

			};

			tval = 0;
		};

		if( tval == 0 )
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		//Data情報取得======================================
//		memcpy(RefGet_->data_.Format, buffcontrol, 4);
//		RefGet_->data_.Format[4] = '\0';
		pWave->SetDATA_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetDATA_Format();
		if( str_ != "data")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

//		RefGet_->data_.DataFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetDATA_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

		char* l_wavedata = new(std::nothrow) char[pWave->Get_WaveDataSize()];
		if(l_wavedata == 0)
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		memcpy(l_wavedata, buffcontrol, pWave->Get_WaveDataSize());

		pWave->SetDATA_WaveData(l_wavedata);
		bool Convret_ = false;

		//サンプリングレートのビットだけ取得する。
		unsigned long l_sample = (static_cast<unsigned long>(ConvertType) & 0x0000FF);

		l_sample *= 1000;
		if(l_sample == 44000)
		{
			l_sample += 100;
		};
		Convret_ = SonikAudioWAVEBitConvert::SonikWAVEConvertSampling(reinterpret_cast<SonikAudioFormat::SonikAudioFormat_InnerSet*>(pWave), l_sample);


		if( !Convret_ )
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		if(!SonikAudio::SAudioFormat::SmartPointerCreate(pWave, RefGet_))
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		delete[] buffers;
		return true;

	};

	//メモリからWAVEファイルとして読み込みます。
	bool Load_WaveFileInMemory(const char* LoadFileData, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg)
	{
		if(LoadFileData == 0)
		{
			return false;
		};

		char* buffers = const_cast<char*>(LoadFileData);
		char* buffcontrol;

		SonikAudioFormatPlatForm::WindowsWaveFormat* pWave = nullptr;
		pWave = new(std::nothrow) SonikAudioFormatPlatForm::WindowsWaveFormat;
		if(pWave == 0)
		{
			return false;
		};


		//読み進めつつデータを入れていく。
		std::string str_;
		buffcontrol = buffers;

		//RIFF情報取得======================================
//		memcpy(RefGet_->riff_.Format, buffcontrol, 4);
//		RefGet_->riff_.Format[4] = '\0';
		pWave->SetRIFF_Format(buffcontrol);
		buffcontrol += 4;


		str_ = pWave->GetRIFF_Format();
		if(str_ != "RIFF")
		{
			delete pWave;
//			delete[] buffers;
			return false;
		};


//		RefGet_->riff_.AllFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetRIFF_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);


//		memcpy(RefGet_->riff_.Type, buffcontrol, 4);
//		RefGet_->riff_.Type[4] = '\0';
		pWave->SetRIFF_Type(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetRIFF_Type();
		if(str_ != "WAVE")
		{
			delete pWave;
//			delete[] buffers;
			return false;
		};

		//FMT情報取得======================================
//		memcpy(RefGet_->fmt_.Format, buffcontrol, 4);
//		RefGet_->fmt_.Format[4] = '\0';
		pWave->SetFMT_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetFMT_Format();
		if(str_ != "fmt ")
		{
			delete pWave;
			return false;
		};

//		RefGet_->fmt_.FormatSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_FormatSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

//		unsigned long controlsize = RefGet_->fmt_.FormatSize;
		unsigned long controlsize = pWave->GetFMT_FormatSize();

//		RefGet_->fmt_.FormatCode = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_FormatCode( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.Channel = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_Channels( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.SamplingRate = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_SamplingRate( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.ProcSec = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_ByteProcSec( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.BlockBaundary = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BlockBoundary( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.BitsPerSample = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BitsPerSample( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

		if(controlsize != 0)
		{
			buffcontrol += controlsize;
		};

		unsigned long tval = 0;
		//DATAチャンク情報がくるまで読み進める。
		for(/*省略*/; /*省略*/  ; ++buffcontrol )
		{
			if( (*buffcontrol) == 'd' )
			{
				tval = (*reinterpret_cast<unsigned long*>(buffcontrol));
				if( tval == 0x61746164 ) //'d''a''t''a'
				{
					break;
				};

			};

			tval = 0;
		};

		if( tval == 0 )
		{
//			delete[] buffers;
			return false;
		};

		//Data情報取得======================================
//		memcpy(RefGet_->data_.Format, buffcontrol, 4);
//		RefGet_->data_.Format[4] = '\0';
		pWave->SetDATA_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetDATA_Format();
		if( str_ != "data")
		{
			delete pWave;
//			delete[] buffers;
			return false;
		};

//		RefGet_->data_.DataFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetDATA_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

		char* l_wavedata = new(std::nothrow) char[pWave->Get_WaveDataSize()];
		if(l_wavedata == 0)
		{
			delete pWave;
//			delete[] buffers;
			return false;
		};

		memcpy(l_wavedata, buffcontrol, pWave->Get_WaveDataSize());

		pWave->SetDATA_WaveData(l_wavedata);
		bool Convret_ = false;

		if( ConvertAudioFlg )
		{
			//変換フラグがオンであれば変換をかける。
			switch( ConvertType )
			{
			case SonikAudio::SCVType::B16_CH1_SR44100:
				//16Bit Mono 44100Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH1_SR48000:
				//16Bit Mono 48000Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH1_SR96000:
				//16bit Mono 96000Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH2_SR44100:
				//16bit Stereo 44100Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH2_SR48000:
				//16bit Stereo 48000Hz Convert
				Convret_ = SonikAudioWAVEBitConvert::SonikWAVEConvertSampling(reinterpret_cast<SonikAudioFormat::SonikAudioFormat_InnerSet*>(pWave), 48000);

				break;

			case SonikAudio::SCVType::B16_CH2_SR96000:
				//16bit Stereo 96000Hz Convert

				break;

			default:
				//no convert;
				//元々Convret_にfalseを設定しているのでここを通ればロード処理自体失敗となります。

				break;
			};

		};

		if( !Convret_ )
		{
			delete pWave;
			return false;
		};

		if(!SonikAudio::SAudioFormat::SmartPointerCreate(pWave, RefGet_))
		{
			delete pWave;
			return false;
		};

		return true;

	};

	//wchar_t型でファイル名を指定し、WAVEファイルを読み込みます。
	bool Load_WaveFile(const char16_t* LoadFilePass, SonikAudio::SAudioFormat& RefGet_, SonikAudio::SCVType ConvertType, bool ConvertAudioFlg)
	{
		std::ifstream ifs_;
		SonikLib::SonikString sonikstr_;

		sonikstr_ = LoadFilePass;

		ifs_.open(sonikstr_.str_c(), std::ios::binary);
		if( !ifs_.is_open() )
		{
			return false;
		};

		char* buffers;
		char* buffcontrol;
		unsigned int beg_pos;
		unsigned int eof_pos;

		ifs_.seekg(0, std::fstream::end);
		eof_pos = ifs_.tellg();

		ifs_.clear();

		ifs_.seekg(0, std::fstream::beg);
		beg_pos = ifs_.tellg();

		buffers = new(std::nothrow) char[ eof_pos - beg_pos ];
		if(buffers == 0)
		{
			return false;
		};

		//ファイルコピー
		ifs_.read(buffers, (eof_pos - beg_pos));
		//あとはリードファイルはいらないのでクローズ
		ifs_.close();


		SonikAudioFormatPlatForm::WindowsWaveFormat* pWave = nullptr;
		pWave = new(std::nothrow) SonikAudioFormatPlatForm::WindowsWaveFormat;
		if(pWave == 0)
		{
			delete[] buffers;
			return false;
		};

		//読み進めつつデータを入れていく。
		std::string str_;
		buffcontrol = buffers;

		//RIFF情報取得======================================
//		memcpy(RefGet_->riff_.Format, buffcontrol, 4);
//		RefGet_->riff_.Format[4] = '\0';
		pWave->SetRIFF_Format(buffcontrol);
		buffcontrol += 4;


		str_ = pWave->GetRIFF_Format();
		if(str_ != "RIFF")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};


//		RefGet_->riff_.AllFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetRIFF_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);


//		memcpy(RefGet_->riff_.Type, buffcontrol, 4);
//		RefGet_->riff_.Type[4] = '\0';
		pWave->SetRIFF_Type(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetRIFF_Type();
		if(str_ != "WAVE")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		//FMT情報取得======================================
//		memcpy(RefGet_->fmt_.Format, buffcontrol, 4);
//		RefGet_->fmt_.Format[4] = '\0';
		pWave->SetFMT_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetFMT_Format();
		if(str_ != "fmt ")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

//		RefGet_->fmt_.FormatSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_FormatSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

//		unsigned long controlsize = RefGet_->fmt_.FormatSize;
		unsigned long controlsize = pWave->GetFMT_FormatSize();

//		RefGet_->fmt_.FormatCode = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_FormatCode( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.Channel = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_Channels( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.SamplingRate = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_SamplingRate( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.ProcSec = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetFMT_ByteProcSec( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);
		controlsize -= sizeof(unsigned long);

//		RefGet_->fmt_.BlockBaundary = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BlockBoundary( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

//		RefGet_->fmt_.BitsPerSample = (*reinterpret_cast<unsigned short*>(buffcontrol));
		pWave->SetFMT_BitsPerSample( (*reinterpret_cast<unsigned short*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned short);
		controlsize -= sizeof(unsigned short);

		if(controlsize != 0)
		{
			buffcontrol += controlsize;
		};

		char* endp = buffers + (eof_pos - beg_pos);
		unsigned long tval = 0;
		//DATAチャンク情報がくるまで読み進める。
		for(/*省略*/; buffcontrol != endp  ; ++buffcontrol )
		{
			if( (*buffcontrol) == 'd' )
			{
				tval = (*reinterpret_cast<unsigned long*>(buffcontrol));
				if( tval == 0x61746164 ) //'d''a''t''a'
				{
					break;
				};

			};

			tval = 0;
		};

		if( tval == 0 )
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		//Data情報取得======================================
//		memcpy(RefGet_->data_.Format, buffcontrol, 4);
//		RefGet_->data_.Format[4] = '\0';
		pWave->SetDATA_Format(buffcontrol);
		buffcontrol += 4;

		str_ = pWave->GetDATA_Format();
		if( str_ != "data")
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

//		RefGet_->data_.DataFileSize = (*reinterpret_cast<unsigned long*>(buffcontrol));
		pWave->SetDATA_FileSize( (*reinterpret_cast<unsigned long*>(buffcontrol)) );
		buffcontrol += sizeof(unsigned long);

		char* l_wavedata = new(std::nothrow) char[pWave->Get_WaveDataSize()];
		if(l_wavedata == 0)
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		memcpy(l_wavedata, buffcontrol, pWave->Get_WaveDataSize());

		pWave->SetDATA_WaveData(l_wavedata);
		bool Convret_ = false;

		if( ConvertAudioFlg )
		{
			//変換フラグがオンであれば変換をかける。
			switch( ConvertType )
			{
			case SonikAudio::SCVType::B16_CH1_SR44100:
				//16Bit Mono 44100Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH1_SR48000:
				//16Bit Mono 48000Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH1_SR96000:
				//16bit Mono 96000Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH2_SR44100:
				//16bit Stereo 44100Hz Convert

				break;

			case SonikAudio::SCVType::B16_CH2_SR48000:
				//16bit Stereo 48000Hz Convert
				Convret_ = SonikAudioWAVEBitConvert::SonikWAVEConvertSampling(reinterpret_cast<SonikAudioFormat::SonikAudioFormat_InnerSet*>(pWave), 48000);

				break;

			case SonikAudio::SCVType::B16_CH2_SR96000:
				//16bit Stereo 96000Hz Convert

				break;

			default:
				//no convert;
				//元々Convret_にfalseを設定しているのでここを通ればロード処理自体失敗となります。

				break;
			};

		};

		if( !Convret_ )
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		if(!SonikAudio::SAudioFormat::SmartPointerCreate(pWave, RefGet_))
		{
			delete pWave;
			delete[] buffers;
			return false;
		};

		return true;
	};


};




