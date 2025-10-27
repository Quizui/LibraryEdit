/*
 * SonikMultiMediaInterface.cpp
 *
 *  Created on: 2020/07/25
 *      Author: SONIC
 */

#include <new>

#include "SonikMultiMediaInterface.h"
#include "SonikMM_AudioInfoImple.h"

#if defined(_WIN32) || defined(_WIN64)
	//windows用ヘッダ

//#include <initguid.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <FunctionDiscoveryKeys_devpkey.h>

const CLSID CLSID_MMDeviceEnumerator    = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator       = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient              = __uuidof(IAudioClient);
const IID IID_IAudioClock               = __uuidof(IAudioClock);
const IID IID_IAudioRenderClient        = __uuidof(IAudioRenderClient);

#elif defined(__linux__)
	//linux用ヘッダ

#endif


namespace SonikLib
{
#if defined(_WIN32) || defined(_WIN64)
	//windows implemente


	SonikMultiMedia::SonikMultiMedia(void)
	:ComState(0)
	{

	};

	SonikMultiMedia::~SonikMultiMedia(void)
	{

	};

	SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> SonikMultiMedia::Get_Hard_AudioInfo_OSSetting(void)
	{
		m_lock.lock();

		SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> retval;

		SonikLib::AudioInfoImple* imple_ = new(std::nothrow) SonikLib::AudioInfoImple;
		if(imple_ == nullptr)
		{
			return retval;
		};

		if(!SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo>::SmartPointerCreate(imple_, retval))
		{
			delete imple_;
			return retval;
		};

		//スレッドは、COMコンポーネントとのやり取りで発生するすべてのスレッド同期作業を
		//COMが背後で面倒見てほしいと宣言
		ComState = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED);

		//スレッドは、COMコンポーネントとのやり取りで発生するすべてのスレッド同期作業を
		//自分で行うと宣言
//		ComState = CoInitializeEx( nullptr, COINIT_MULTITHREADED);

		if( FAILED(ComState) )
		{
			//COM初期化失敗。(アパートメント属性設定に失敗)
			m_lock.unlock();
			return retval;
		};

		//c マルチメディアデバイスの列挙子
		IMMDeviceEnumerator* pDeviceEnumerator = nullptr;	//c マルチメディアデバイス列挙インターフェース
		HRESULT ret;

		ret = CoCreateInstance( CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator );
		if( !SUCCEEDED(ret) )
		{
			//c マルチメディアデバイスの列挙に失敗。
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//c デフォルトのデバイスを選択
		IMMDevice* pDevice;			//c デバイスインターフェース
		ret = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		if( !SUCCEEDED(ret) )
		{
			//c デフォルトデバイスの選択に失敗。
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//c オーディオクライアント
		IAudioClient* pAudioClient;		//c オーディオクライアントインターフェース
		ret = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pAudioClient);
		if( !SUCCEEDED(ret) )
		{
			//c オーディオクライアント取得失敗。
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		WAVEFORMATEXTENSIBLE* pmixextensible = new(std::nothrow) WAVEFORMATEXTENSIBLE(); //c ゼロ初期化のために()呼び(C++限定)
		if( pmixextensible == nullptr )
		{
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		WAVEFORMATEX* _mixformat;
		ret = pAudioClient->GetMixFormat(&_mixformat);
		if( !SUCCEEDED(ret) )
		{
			//c フォーマット取得に失敗。
			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//OSから取得した情報で設定ができるかをまずチェック
		pmixextensible->Format.wBitsPerSample 	= _mixformat->wBitsPerSample;
		pmixextensible->Format.nSamplesPerSec	= _mixformat->nSamplesPerSec;
		pmixextensible->Format.nChannels		= _mixformat->nChannels;
		pmixextensible->Format.nBlockAlign 		= _mixformat->nChannels * pmixextensible->Format.wBitsPerSample / 8;
		pmixextensible->Format.nAvgBytesPerSec	= _mixformat->nSamplesPerSec * pmixextensible->Format.nBlockAlign;
		pmixextensible->Format.wFormatTag		= _mixformat->wFormatTag;
		pmixextensible->Format.cbSize			= _mixformat->cbSize;

		pmixextensible->Samples.wValidBitsPerSample = pmixextensible->Format.wBitsPerSample;
		pmixextensible->Samples.wSamplesPerBlock	= pmixextensible->Samples.wValidBitsPerSample;
		pmixextensible->Samples.wReserved			= pmixextensible->Samples.wValidBitsPerSample;
		pmixextensible->SubFormat					= KSDATAFORMAT_SUBTYPE_PCM;
		pmixextensible->dwChannelMask                = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
		//c 設定から終わったらとりあえずOSから取得した情報を開放
		CoTaskMemFree(_mixformat);

		//c １回目のフォーマットチェック
		//非排他モードは第３引数を設定して、エラーなら再設定する必要が出て来る。
		WAVEFORMATEXTENSIBLE* retwf = nullptr; //失敗した場合の近似設定保存先。
		ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)pmixextensible, (WAVEFORMATEX**)&retwf);
		//c 近似値はあまり使う気がないので即free
		if( retwf != nullptr )
		{
			CoTaskMemFree(retwf);
			retwf = nullptr;
		};
		if( SUCCEEDED(ret) )
		{
			//c 成功なら終了
			imple_->m_Bit			= pmixextensible->Format.wBitsPerSample;
			imple_->m_SamplingRate	= pmixextensible->Format.nSamplesPerSec;
			imple_->m_Channels		= pmixextensible->Format.nChannels;

			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();

			return retval;
		};

		//c OSから取得した物が使えなかったので。
		//c ループして最適なフォーマットを探す。
		ret = E_INVALIDARG;
		bool loopval = true;
		while( loopval )
		{
			//0 = bit, 1 = samplingrate, 2 = channels
			int switchvalue[3] = {0, 0, 0};
			//0 = bit, 1 = samplingrate, 2 = channels, 3 = speaker position
			int Setvalue[4] = {0, 0, 0, 0};

			//c ビット選択
			switch( switchvalue[0] )
			{
			case 0: //16bit
				Setvalue[0] = 16;
				break;

			case 1: //32bit
				Setvalue[0] = 32;
				break;

			default:
				//c ここに来る時は全部駄目だったことになるのでループフラグを降ろして終了
				loopval = false;
				break;

			};

			//c サンプリングレート選択
			switch( switchvalue[1] )
			{
			case 0: //44.1kHz
				Setvalue[1] = 44100;
				++switchvalue[1];
				break;

			case 1: //48.0kHz
				Setvalue[1] = 48000;
				++switchvalue[1];
				break;

			case 2: //96.0kHz
				Setvalue[1] = 96000;
				++switchvalue[1];
				break;

			case 3: //192.0kHz
				Setvalue[1] = 192000;

				//c 該当がなかったのでチャンネル数の値をインクリメントし、サンプリングレートのスイッチ値をリセット
				++switchvalue[2];
				switchvalue[1] = 0;
				break;

			default:
				//c 通らないこと想定です。
				//warning回避
				break;

			};

			//c チャンネル選択
			switch( switchvalue[2] )
			{
			case 0: //c モノラル
				Setvalue[2] = 1;
				Setvalue[3] = SPEAKER_FRONT_CENTER;
				break;

			case 1: //c ステレオ
				Setvalue[2] = 2;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
				break;

			case 2: //c 4ch
				Setvalue[2] = 4;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER;
				break;

			case 3: //c 5.1ch
				Setvalue[2] = 6;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER;
				Setvalue[3] = Setvalue[3] | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
				break;

			case 4: //c 7.1ch
				Setvalue[2] = 8;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER;
				Setvalue[3] = Setvalue[3] | SPEAKER_LOW_FREQUENCY | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;
				Setvalue[3] = Setvalue[3] | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;

				//c 全部駄目だった時用にビット数を上げる
				++switchvalue[0];
				switchvalue[2] = 0;

				break;

			default:
				//c 通らないこと想定です。
				//warning回避
				break;
			};

			if( !loopval )
			{
				//c ループを抜ける
				break;
			};

			//Set
			pmixextensible->Format.wBitsPerSample 	= Setvalue[0];
			pmixextensible->Format.nSamplesPerSec	= Setvalue[1];
			pmixextensible->Format.nChannels		= Setvalue[2];
			pmixextensible->Format.nBlockAlign 		= pmixextensible->Format.nChannels * pmixextensible->Format.wBitsPerSample / 8;
			pmixextensible->Format.nAvgBytesPerSec	= pmixextensible->Format.nSamplesPerSec * pmixextensible->Format.nBlockAlign;

			pmixextensible->Samples.wValidBitsPerSample = pmixextensible->Format.wBitsPerSample;
			pmixextensible->Samples.wSamplesPerBlock	= pmixextensible->Samples.wValidBitsPerSample;
			pmixextensible->Samples.wReserved			= pmixextensible->Samples.wValidBitsPerSample;
			pmixextensible->dwChannelMask               = Setvalue[3];

			ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)pmixextensible, (WAVEFORMATEX**)&retwf);
			if( retwf != nullptr )
			{
				CoTaskMemFree(retwf);
				retwf = nullptr;
			};

			if( SUCCEEDED(ret) )
			{
				//c 成功ならその場でbreak
				break;
			};

		};//end while

		if( !SUCCEEDED(ret) )
		{
			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};


		imple_->m_Bit			= pmixextensible->Format.wBitsPerSample;
		imple_->m_SamplingRate	= pmixextensible->Format.nSamplesPerSec;
		imple_->m_Channels		= pmixextensible->Format.nChannels;

		delete pmixextensible;
		pAudioClient->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();
		CoUninitialize();
		m_lock.unlock();

		return retval;
	};




	SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> SonikMultiMedia::Get_Hard_AudioInfo_OpenALSetting(void)
	{
		m_lock.lock();

		SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo> retval;

		SonikLib::AudioInfoImple* imple_ = new(std::nothrow) SonikLib::AudioInfoImple;
		if(imple_ == nullptr)
		{
			return retval;
		};

		if(!SonikLib::SharedSmtPtr<SonikLib::SonikMMAudioInfo>::SmartPointerCreate(imple_, retval))
		{
			delete imple_;
			return retval;
		};

		//c スレッドは、COMコンポーネントとのやり取りで発生するすべてのスレッド同期作業を
		//c COMが背後で面倒見てほしいと宣言
		ComState = CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED);

		//c スレッドは、COMコンポーネントとのやり取りで発生するすべてのスレッド同期作業を
		//c 自分で行うと宣言
//		ComState = CoInitializeEx( nullptr, COINIT_MULTITHREADED);

		if( FAILED(ComState) )
		{
			//COM初期化失敗。(アパートメント属性設定に失敗)
			m_lock.unlock();
			return retval;
		};

		//c マルチメディアデバイスの列挙子
		IMMDeviceEnumerator* pDeviceEnumerator = nullptr;	//c マルチメディアデバイス列挙インターフェース
		HRESULT ret;

		ret = CoCreateInstance( CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator );
		if( !SUCCEEDED(ret) )
		{
			//c マルチメディアデバイスの列挙に失敗。
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//c デフォルトのデバイスを選択
		IMMDevice* pDevice;			//c デバイスインターフェース
		ret = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		if( !SUCCEEDED(ret) )
		{
			//c デフォルトデバイスの選択に失敗。
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//c オーディオクライアント
		IAudioClient* pAudioClient;		//c オーディオクライアントインターフェース
		ret = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&pAudioClient);
		if( !SUCCEEDED(ret) )
		{
			//c オーディオクライアント取得失敗。
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		WAVEFORMATEXTENSIBLE* pmixextensible = new(std::nothrow) WAVEFORMATEXTENSIBLE(); //c ゼロ初期化のために()呼び(C++限定)
		if( pmixextensible == nullptr )
		{
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		WAVEFORMATEX* _mixformat;
		ret = pAudioClient->GetMixFormat(&_mixformat);
		if( !SUCCEEDED(ret) )
		{
			//c フォーマット取得に失敗。
			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};

		//OSから取得した情報で設定ができるかをまずチェック
		//手動でOpenALで使える形に修正。
		pmixextensible->Format.wBitsPerSample 	= 16;
		pmixextensible->Format.nSamplesPerSec	= _mixformat->nSamplesPerSec;
		pmixextensible->Format.nChannels		= 2;
		pmixextensible->Format.nBlockAlign 		= _mixformat->nChannels * pmixextensible->Format.wBitsPerSample / 8;
		pmixextensible->Format.nAvgBytesPerSec	= _mixformat->nSamplesPerSec * pmixextensible->Format.nBlockAlign;
		pmixextensible->Format.wFormatTag		= _mixformat->wFormatTag;
		pmixextensible->Format.cbSize			= _mixformat->cbSize;

		pmixextensible->Samples.wValidBitsPerSample = pmixextensible->Format.wBitsPerSample;
		pmixextensible->Samples.wSamplesPerBlock	= pmixextensible->Samples.wValidBitsPerSample;
		pmixextensible->Samples.wReserved			= pmixextensible->Samples.wValidBitsPerSample;
		pmixextensible->SubFormat					= KSDATAFORMAT_SUBTYPE_PCM;
		pmixextensible->dwChannelMask                = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
		//c 設定から終わったらとりあえずOSから取得した情報を開放
		CoTaskMemFree(_mixformat);

		//c １回目のフォーマットチェック
		//非排他モードは第３引数を設定して、エラーなら再設定する必要が出て来る。
		WAVEFORMATEXTENSIBLE* retwf = nullptr; //失敗した場合の近似設定保存先。
		ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)pmixextensible, (WAVEFORMATEX**)&retwf);
		//c 近似値はあまり使う気がないので即free
		if( retwf != nullptr )
		{
			CoTaskMemFree(retwf);
			retwf = nullptr;
		};
		if( SUCCEEDED(ret) )
		{
			//c 成功なら終了
			imple_->m_Bit			= pmixextensible->Format.wBitsPerSample;
			imple_->m_SamplingRate	= pmixextensible->Format.nSamplesPerSec;
			imple_->m_Channels		= pmixextensible->Format.nChannels;

			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();

			return retval;
		};

		//c OSから取得した物が使えなかったので。
		//c ループして最適なフォーマットを探す。
		ret = E_INVALIDARG;
		bool loopval = true;
		while( loopval )
		{
			//0 = bit, 1 = samplingrate, 2 = channels
			int switchvalue[3] = {0, 0, 0};
			//0 = bit, 1 = samplingrate, 2 = channels, 3 = speaker position
			int Setvalue[4] = {0, 0, 0, 0};

			//c ビット選択
			switch( switchvalue[0] )
			{
			case 0: //16bit
				Setvalue[0] = 16;
				break;

			case 1: //32bit
				Setvalue[0] = 32;
				break;

			default:
				//c ここに来る時は全部駄目だったことになるのでループフラグを降ろして終了
				loopval = false;
				break;

			};

			//c サンプリングレート選択
			switch( switchvalue[1] )
			{
			case 0: //44.1kHz
				Setvalue[1] = 44100;
				++switchvalue[1];
				break;

			case 1: //48.0kHz
				Setvalue[1] = 48000;
				++switchvalue[1];
				break;

			case 2: //96.0kHz
				Setvalue[1] = 96000;
				++switchvalue[1];
				break;

			case 3: //192.0kHz
				Setvalue[1] = 192000;

				//c 該当がなかったのでチャンネル数の値をインクリメントし、サンプリングレートのスイッチ値をリセット
				++switchvalue[2];
				switchvalue[1] = 0;
				break;

			default:
				//c 通らないこと想定です。
				//warning回避
				break;

			};

			//c チャンネル選択
			switch( switchvalue[2] )
			{
			case 0: //c モノラル
				Setvalue[2] = 1;
				Setvalue[3] = SPEAKER_FRONT_CENTER;
				break;

			case 1: //c ステレオ
				Setvalue[2] = 2;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
				break;

			case 2: //c 4ch
				Setvalue[2] = 4;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER;
				break;

			case 3: //c 5.1ch
				Setvalue[2] = 6;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER;
				Setvalue[3] = Setvalue[3] | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
				break;

			case 4: //c 7.1ch
				Setvalue[2] = 8;
				Setvalue[3] = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_LEFT | SPEAKER_FRONT_CENTER;
				Setvalue[3] = Setvalue[3] | SPEAKER_LOW_FREQUENCY | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;
				Setvalue[3] = Setvalue[3] | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;

				//c 全部駄目だった時用にビット数を上げる
				++switchvalue[0];
				switchvalue[2] = 0;

				break;

			default:
				//c 通らないこと想定です。
				//warning回避
				break;
			};

			if( !loopval )
			{
				//c ループを抜ける
				break;
			};

			//Set
			pmixextensible->Format.wBitsPerSample 	= Setvalue[0];
			pmixextensible->Format.nSamplesPerSec	= Setvalue[1];
			pmixextensible->Format.nChannels		= Setvalue[2];
			pmixextensible->Format.nBlockAlign 		= pmixextensible->Format.nChannels * pmixextensible->Format.wBitsPerSample / 8;
			pmixextensible->Format.nAvgBytesPerSec	= pmixextensible->Format.nSamplesPerSec * pmixextensible->Format.nBlockAlign;

			pmixextensible->Samples.wValidBitsPerSample = pmixextensible->Format.wBitsPerSample;
			pmixextensible->Samples.wSamplesPerBlock	= pmixextensible->Samples.wValidBitsPerSample;
			pmixextensible->Samples.wReserved			= pmixextensible->Samples.wValidBitsPerSample;
			pmixextensible->dwChannelMask               = Setvalue[3];

			ret = pAudioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)pmixextensible, (WAVEFORMATEX**)&retwf);
			if( retwf != nullptr )
			{
				CoTaskMemFree(retwf);
				retwf = nullptr;
			};

			if( SUCCEEDED(ret) )
			{
				//c 成功ならその場でbreak
				break;
			};

		};//end while

		if( !SUCCEEDED(ret) )
		{
			delete pmixextensible;
			pAudioClient->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			CoUninitialize();
			m_lock.unlock();
			return retval;
		};


		imple_->m_Bit			= pmixextensible->Format.wBitsPerSample;
		imple_->m_SamplingRate	= pmixextensible->Format.nSamplesPerSec;
		imple_->m_Channels		= pmixextensible->Format.nChannels;

		delete pmixextensible;
		pAudioClient->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();
		CoUninitialize();
		m_lock.unlock();

		return retval;
	};

#elif defined(__linux__)
	//linux用ヘッダ

#endif

};
