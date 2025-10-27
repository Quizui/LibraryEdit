/*
 * WaveBitConvert.cpp
 *
 *  Created on: 2018/05/06
 *      Author: SONIC
 */

#include <climits>
#include <cstdint>
#include <string> //memset


#include "../Functions/WaveBitConvert.h"

#include "../../MathBit/SonikNormalize.h"
#include "../Format/SonikAudioFormat_InnerUse.h"
#include "fftw3.h"

namespace SonikAudioWAVEBitConvert
{
	inline void Convert8Bitto16Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		short* Dst = reinterpret_cast<short*>(dstTop);
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = (*srcTop);
			++Dst;
			++srcTop;
		};

	};

	inline void Convert8Bitto32Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		long* Dst = reinterpret_cast<long*>(dstTop);
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = (*srcTop);
			++Dst;
			++srcTop;
		};

	};

	inline void Convert16Bitto8Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		short* Src = reinterpret_cast<short*>(srcTop);
		char* Dst = dstTop;
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = static_cast<char>(SonikMath::Normalize_Max_to_MinF((*Src), LONG_MAX, LONG_MIN, CHAR_MAX, CHAR_MIN) ); //8bitの範囲に正規化
			++Dst;
			++Src;
		};

	};

	inline void Convert16Bitto32Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		uint16_t* Src = reinterpret_cast<uint16_t*>(srcTop);
		int32_t* Dst = reinterpret_cast<int32_t*>(dstTop);
		BlockAlign <<= 1;
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = static_cast<int32_t>( (*Src) << 16 );
			++Dst;
			++Src;
			(*Dst) = static_cast<int32_t>( (*Src) << 16 );;
			++Dst;
			++Src;
		};

	};

	inline void Convert32Bitto8Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		long* Src = reinterpret_cast<long*>(srcTop);
		char* Dst = dstTop;
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = static_cast<char>(SonikMath::Normalize_Max_to_MinF((*Src),LONG_MAX, LONG_MIN, CHAR_MAX, CHAR_MIN));
			++Dst;
			++Src;
		};

	};

	inline void Convert32Bitto16Bit(unsigned long SrcWaveSize, unsigned long BlockAlign, char* dstTop, char* srcTop)
	{
		//新しい領域に１サンプル単位で代入
		long* Src = reinterpret_cast<long*>(srcTop);
		short* Dst = reinterpret_cast<short*>(dstTop);
		for(/*省略*/; SrcWaveSize != 0; SrcWaveSize -= BlockAlign)
		{
			(*Dst) = static_cast<short>(SonikMath::Normalize_Max_to_MinF((*Src), LONG_MAX, LONG_MIN, SHRT_MAX, SHRT_MIN));
			++Dst;
			++Src;
		};

	};



//	bool SonikWAVEConvert(SonikAudioFormat_WAVE::WaveFormat* RefGet_, SonikBitMode::AudioBitMode BitMode)
//	{
//		return false;
//	};

	//アップサンプリング
	inline bool SonikWaveConvert_Up_Sampling_2ch_16bit(SonikAudioFormat::SonikAudioFormat_InnerSet* ConvTarget, int64_t ConvertSamplingRate)
	{
		fftw_complex* L_in = nullptr;
		fftw_complex* R_in = nullptr;
		fftw_complex* L_out = nullptr;
		fftw_complex* R_out = nullptr;
		fftw_complex* L_ConvertOut = nullptr;
		fftw_complex* R_ConvertOut = nullptr;


		fftw_plan FFT_Lfunc;
		fftw_plan FFT_Rfunc;
		fftw_plan IFFT_Lfunc;
		fftw_plan IFFT_Rfunc;

		int64_t l_samplingrate = ConvTarget->Get_FormatSamplingRate();

		//WAVEデータを格納する領域を生成
		L_in  = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		R_in  = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		L_out = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );
		R_out = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );
		L_ConvertOut = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );
		R_ConvertOut = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );

		//関数登録
		FFT_Lfunc = fftw_plan_dft_1d(l_samplingrate, L_in, L_out, FFTW_FORWARD, FFTW_ESTIMATE);
		FFT_Rfunc = fftw_plan_dft_1d(l_samplingrate, R_in, R_out, FFTW_FORWARD, FFTW_ESTIMATE);
		IFFT_Lfunc = fftw_plan_dft_1d(ConvertSamplingRate, L_out, L_ConvertOut, FFTW_BACKWARD, FFTW_ESTIMATE);
		IFFT_Rfunc = fftw_plan_dft_1d(ConvertSamplingRate, R_out, R_ConvertOut, FFTW_BACKWARD, FFTW_ESTIMATE);


		//配列回すのに必要な変数群
		uint64_t nwdatasize				= static_cast<uint64_t>( (static_cast<double>(ConvTarget->Get_WaveDataSize()) / static_cast<double>(ConvTarget->Get_FormatBlockBaundary())) / l_samplingrate);
		char* NewWaveData				= new char[ ((nwdatasize + 1) * ConvertSamplingRate) * ConvTarget->Get_FormatBlockBaundary() ];
		int16_t* NWaveDataInsert	= reinterpret_cast<int16_t*>(NewWaveData);
		const int16_t* p_copy			= reinterpret_cast<const int16_t*>(ConvTarget->Get_WaveData());
		uint32_t ZeroValueRange		= ConvertSamplingRate - l_samplingrate;
		uint32_t HalfPoint					= l_samplingrate / 2;

		//配列に突っ込みながら0でアップサンプリングしていきます。
		for(uint64_t i=0; i < nwdatasize -1; ++i)
		{
			//ぶっこんでー
			for(uint64_t l=0; l < l_samplingrate; ++l)
			{
				L_in[l][0] = (*p_copy);
				L_in[l][1] = 0.0;
				++p_copy;
				R_in[l][0] = (*p_copy);
				R_in[l][1] = 0.0;
				++p_copy;

			};

			//FFTかけてー
	        fftw_execute(FFT_Lfunc);
	        fftw_execute(FFT_Rfunc);

	        //FFT結果の周波数におけるSamplinRate / 2 が有効周波数でそれより右はあっても意味がない(ノイズの原因になるかも)なので削除
	        //よってHalfPointから先は0埋めする。
	        memset(&L_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));
	        memset(&R_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));

	        //アップサンプリング処理
	        //各最後に0を挿入(memsetで挿入済みではあるが...)してアップサンプリングを試みる。
	        for(uint64_t l=0; l < ZeroValueRange; ++l)
	        {
	        	//実部に代入。
	        	L_out[HalfPoint + l][0] = 0.0;
	        	R_out[HalfPoint + l][0] = 0.0;
	        };

	        //逆変換をしてみる。
	        fftw_execute(IFFT_Lfunc);
	        fftw_execute(IFFT_Rfunc);

	        //新しい領域に追加
	        //追加するときに正規化してあげないと砂嵐になります！FFT理解してない私はこれで１年かかった。
	        for(uint64_t l=0; l < ConvertSamplingRate; ++l)
	        {
	        	(*NWaveDataInsert) = L_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate);
	        	++NWaveDataInsert;
	        	(*NWaveDataInsert) = R_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate);
	        	++NWaveDataInsert;
	        };

		};

		//最後の一部の領域がループ回数計算上うまく釣り合わず、超過してしまう場合があるため、ループ内処理と
		//同じ処理をはみ出し部分の領域を対象に実施する。
		//0最初に端数以外の領域を０埋め
		memset(L_in, 0, sizeof(fftw_complex) * l_samplingrate);
		memset(R_in, 0, sizeof(fftw_complex) * l_samplingrate);

		//残り端数を挿入
		uint64_t broken_num = ( ConvTarget->Get_WaveDataSize() - (nwdatasize * ConvTarget->Get_FormatBlockBaundary() * l_samplingrate) ) / ConvTarget->Get_FormatBlockBaundary();

		for(uint64_t m=0; m < broken_num; ++m)
		{
			L_in[m][0] = (*p_copy);
			L_in[m][1] = 0.0;
			++p_copy;
			R_in[m][0] = (*p_copy);
			R_in[m][1] = 0.0;
			++p_copy;
		};

		//FFT
		fftw_execute(FFT_Lfunc);
		fftw_execute(FFT_Rfunc);

        //FFT結果の周波数におけるSamplinRate / 2 が有効周波数でそれより右はあっても意味がない(ノイズの原因になるかも)なので削除
        //よってHalfPointから先は0埋めする。
        memset(&L_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));
        memset(&R_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));

        //中央に0を挿入します。
        //各最後に0を挿入(memsetで挿入済みではあるが...)してアップサンプリングを試みる。
        for(uint64_t l=0; l < ZeroValueRange; ++l)
        {
        	//実部に代入。
        	L_out[HalfPoint + l][0] = 0.0;
        	R_out[HalfPoint + l][0] = 0.0;
        };


		//IFFT
		fftw_execute(IFFT_Lfunc);
		fftw_execute(IFFT_Rfunc);

		//新しい領域に追加
        for(uint64_t l=0; l < ConvertSamplingRate; ++l)
        {
        	(*NWaveDataInsert) = L_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate); //正規化して代入
        	++NWaveDataInsert;
        	(*NWaveDataInsert) = R_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate); //正規化して代入
        	++NWaveDataInsert;
        };

        //wavedata置き換え
        const char* l_pcm_data = ConvTarget->Get_WaveData();
        delete[] l_pcm_data;
        ConvTarget->Set_WaveDataSize( (nwdatasize + 1) * 4 * ConvertSamplingRate );
        ConvTarget->Set_FormatSamplingRate( ConvertSamplingRate );
        ConvTarget->Set_WaveData(NewWaveData);


        // 後始末（使用した配列等を廃棄）
        fftw_destroy_plan(FFT_Lfunc);
        fftw_destroy_plan(FFT_Rfunc);
        fftw_destroy_plan(IFFT_Lfunc);
        fftw_destroy_plan(IFFT_Rfunc);
        fftw_free(L_in);
        fftw_free(R_in);
        fftw_free(L_out);
        fftw_free(R_out);
        fftw_free(L_ConvertOut);
        fftw_free(R_ConvertOut);

		return true;

	};


	//ダウンサンプリング
	inline bool SonikWaveConvert_Down_Sampling_2ch_16bit(SonikAudioFormat::SonikAudioFormat_InnerSet* ConvTarget, int64_t ConvertSamplingRate)
	{
		fftw_complex* L_in = nullptr;
		fftw_complex* R_in = nullptr;
		fftw_complex* L_out = nullptr;
		fftw_complex* R_out = nullptr;
		fftw_complex* L_ConvertOut = nullptr;
		fftw_complex* R_ConvertOut = nullptr;


		fftw_plan FFT_Lfunc;
		fftw_plan FFT_Rfunc;
		fftw_plan IFFT_Lfunc;
		fftw_plan IFFT_Rfunc;

		int64_t l_samplingrate = ConvTarget->Get_FormatSamplingRate();

		//WAVEデータを格納する領域を生成
		L_in  = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		R_in  = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		L_out = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		R_out = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * l_samplingrate) );
		L_ConvertOut = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );
		R_ConvertOut = reinterpret_cast<fftw_complex*>( fftw_malloc(sizeof(fftw_complex) * ConvertSamplingRate) );

		//関数登録
		FFT_Lfunc = fftw_plan_dft_1d(l_samplingrate, L_in, L_out, FFTW_FORWARD, FFTW_ESTIMATE);
		FFT_Rfunc = fftw_plan_dft_1d(l_samplingrate, R_in, R_out, FFTW_FORWARD, FFTW_ESTIMATE);
		IFFT_Lfunc = fftw_plan_dft_1d(ConvertSamplingRate, L_out, L_ConvertOut, FFTW_BACKWARD, FFTW_ESTIMATE);
		IFFT_Rfunc = fftw_plan_dft_1d(ConvertSamplingRate, R_out, R_ConvertOut, FFTW_BACKWARD, FFTW_ESTIMATE);


		//配列回すのに必要な変数群
		uint64_t nwdatasize				= static_cast<uint64_t>( (static_cast<double>(ConvTarget->Get_WaveDataSize()) / static_cast<double>(ConvTarget->Get_FormatBlockBaundary())) / l_samplingrate);
		char* NewWaveData				= new char[ ((nwdatasize + 1) * ConvertSamplingRate) * ConvTarget->Get_FormatBlockBaundary() ];
		int16_t* NWaveDataInsert	= reinterpret_cast<int16_t*>(NewWaveData);
		const int16_t* p_copy			= reinterpret_cast<const int16_t*>(ConvTarget->Get_WaveData());
		uint32_t ZeroValueRange		= l_samplingrate - ConvertSamplingRate;
		uint32_t HalfPoint					= l_samplingrate / 2;
        uint32_t smple_thin_out_point = HalfPoint - ZeroValueRange; //ダウンサンプリング時のみ必要。間引きポイントを算出

		//配列に突っ込みながら0でアップサンプリングしていきます。
		for(uint64_t i=0; i < nwdatasize -1; ++i)
		{
			//ぶっこんでー
			for(uint64_t l=0; l < l_samplingrate; ++l)
			{
				L_in[l][0] = (*p_copy);
				L_in[l][1] = 0.0;
				++p_copy;
				R_in[l][0] = (*p_copy);
				R_in[l][1] = 0.0;
				++p_copy;
			};

			//FFTかけてー
	        fftw_execute(FFT_Lfunc);
	        fftw_execute(FFT_Rfunc);

	        //FFT結果の周波数におけるSamplinRate / 2 が有効周波数でそれより右はあっても意味がない(ノイズの原因になるかも)なので削除
	        //よってHalfPointから先は0埋めする。
	        memset(&L_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));
	        memset(&R_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));

	        //ダウンサンプリング処理
	        //間引く。
	        for(uint64_t l=0; l < ZeroValueRange; ++l)
	        {
	        	L_out[smple_thin_out_point + l][0] = 0.0;
	        	R_out[smple_thin_out_point + l][0] = 0.0;
	        };


	        //逆変換をしてみる。
	        fftw_execute(IFFT_Lfunc);
	        fftw_execute(IFFT_Rfunc);

	        //新しい領域に追加
	        //追加するときに正規化してあげないと砂嵐になります！FFT理解してない私はこれで１年かかった。
	        for(uint64_t l=0; l < ConvertSamplingRate; ++l)
	        {
	        	(*NWaveDataInsert) = L_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate);
	        	++NWaveDataInsert;
	        	(*NWaveDataInsert) = R_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate);
	        	++NWaveDataInsert;
	        };

		};

		//最後の一部の領域がループ回数計算上うまく釣り合わず、超過してしまう場合があるため、ループ内処理と
		//同じ処理をはみ出し部分の領域を対象に実施する。
		//0最初に端数以外の領域を０埋め
		memset(L_in, 0, sizeof(fftw_complex) * l_samplingrate);
		memset(R_in, 0, sizeof(fftw_complex) * l_samplingrate);

		//残り端数を挿入
		uint64_t broken_num = ( ConvTarget->Get_WaveDataSize() - (nwdatasize * ConvTarget->Get_FormatBlockBaundary() * l_samplingrate) ) / ConvTarget->Get_FormatBlockBaundary();

		for(uint64_t m=0; m < broken_num; ++m)
		{
			L_in[m][0] = (*p_copy);
			L_in[m][1] = 0.0;
			++p_copy;
			R_in[m][0] = (*p_copy);
			R_in[m][1] = 0.0;
			++p_copy;
		};

		//FFT
		fftw_execute(FFT_Lfunc);
		fftw_execute(FFT_Rfunc);

        //FFT結果の周波数におけるSamplinRate / 2 が有効周波数でそれより右はあっても意味がない(ノイズの原因になるかも)なので削除
        //よってHalfPointから先は0埋めする。
        memset(&L_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));
        memset(&R_out[(HalfPoint)], 0, (sizeof(fftw_complex) * HalfPoint));

        //ダウンサンプリング処理
        //間引く。
        for(uint64_t l=0; l < ZeroValueRange; ++l)
        {
        	L_out[smple_thin_out_point + l][0] = 0.0;
        	R_out[smple_thin_out_point + l][0] = 0.0;
        };

		//IFFT
		fftw_execute(IFFT_Lfunc);
		fftw_execute(IFFT_Rfunc);

		//新しい領域に追加
        for(uint64_t l=0; l < ConvertSamplingRate; ++l)
        {
        	(*NWaveDataInsert) = L_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate); //正規化して代入
        	++NWaveDataInsert;
        	(*NWaveDataInsert) = R_ConvertOut[l][0] / static_cast<double>(ConvertSamplingRate); //正規化して代入
        	++NWaveDataInsert;
        };

        //wavedata置き換え
        const char* l_pcm_data = ConvTarget->Get_WaveData();
        delete[] l_pcm_data;
        ConvTarget->Set_WaveDataSize( (nwdatasize + 1) * 4 * ConvertSamplingRate );
        ConvTarget->Set_FormatSamplingRate( ConvertSamplingRate );
        ConvTarget->Set_WaveData(NewWaveData);


        // 後始末（使用した配列等を廃棄）
        fftw_destroy_plan(FFT_Lfunc);
        fftw_destroy_plan(FFT_Rfunc);
        fftw_destroy_plan(IFFT_Lfunc);
        fftw_destroy_plan(IFFT_Rfunc);
        fftw_free(L_in);
        fftw_free(R_in);
        fftw_free(L_out);
        fftw_free(R_out);
        fftw_free(L_ConvertOut);
        fftw_free(R_ConvertOut);

		return true;

	};


	bool SonikWAVEConvertSampling(SonikAudioFormat::SonikAudioFormat_InnerSet* ConvTarget, int64_t ConvertSamplingRate)
	{
		if(ConvTarget == nullptr)
		{
			return false;
		};

		bool _ret = false;
		uint32_t _convtype = ConvTarget->Get_FormatBitsPerSample();
		//ビット演算して一つの変数にビット数とチャンネル数を含める。
		 //ビット演算のため左に8bitずらしてORしてビットを立て、簡易的に結合(Windowsはリトルエンディアン)
		_convtype = (_convtype  << 8) | ConvTarget->Get_FormatChannel();

		switch(_convtype)
		{
		case 0x1001:	//16bit ch1

			break;
		case 0x1002: //16bit ch2
			//アップサンプリングか？
			if( ConvertSamplingRate >= ConvTarget->Get_FormatSamplingRate() )
			{
				_ret = SonikWaveConvert_Up_Sampling_2ch_16bit(ConvTarget, ConvertSamplingRate);

			}else
			{
				//条件が一致しなければダウンサンプリング。
				_ret = SonikWaveConvert_Down_Sampling_2ch_16bit(ConvTarget, ConvertSamplingRate);
			};

			break;
		default:
			return false;
		};

		return _ret;
	};

};



