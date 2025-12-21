#include "SonikMathNumeric.h"

#include "Container/RangedForContainer.hpp"
#include "SonikMathStandard.h"
//#include <new>
#include <cstdint>

#ifdef _DEBUG
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#endif



namespace SonikMath
{
	//有効桁数は7桁程度
	static const float __SONIK_MATH_M_PI_F = 3.1415927f;
	//有効桁数は15桁程度
	static const double __SONIK_MATH_M_PI_D =  3.141592653589793;

	double __SONIK_MATH_SIN_LIST__[36001] = {0}; //Sin事前テーブル
	double __SONIK_MATH_COS_LIST__[36001] = {0}; //Cos事前テーブル
	uint32_t __SONIK_MATH_FFT_BITREVERSE_TABLE__[4096] = {0}; //FFTで使うビット反転事前テーブル
	SonikComplex __SONIK_MATH_FFT_TWIDDLE_TABLE__[4096] = {0}; //FFTで使う反転因子事前テーブル

	static const double __SONIK_MATH_FFT_INVERSE512__ = 1.0 / 512.0;
	static const double __SONIK_MATH_FFT_INVERSE1024__ = 1.0 / 1024.0;
	static const double __SONIK_MATH_FFT_INVERSE2048__ = 1.0 / 2048.0;
	static const double __SONIK_MATH_FFT_INVERSE4096__ = 1.0 / 4096.0;

	//Sin/Cos配列作成イニシャライザです。
	//プログラム実行前の初期化時に一回だけ呼ばれる計算。
	int8_t Initialize(void)
	{
    	const double step = __SONIK_MATH_M_PI_D / 18000.0; // 0.01°刻みのラジアン
    	double radian = 0.0;
		double l_x = 0.0;
		double l_xs = 0.0;

		//Sinテーブル生成
    	for (uint32_t i = 0; i <= 36000; ++i)
		{
			//テイラー展開
			l_x = SonikMath::mod(radian, 2.0 * __SONIK_MATH_M_PI_D);
			// -π〜π に収めると精度が安定するらしい。
			if (l_x > __SONIK_MATH_M_PI_D)
			{
				l_x -= 2.0 * __SONIK_MATH_M_PI_D;
			};
			l_xs = l_x * l_x;

        	__SONIK_MATH_SIN_LIST__[i] = l_x * (1.0 - l_xs/6.0 + l_xs * l_xs/120.0 - l_xs * l_xs * l_xs / 5040.0); // 7次までの展開
        	radian += step;
    	};

		l_x = 0.0;
		l_xs = 0.0;
		//Cosテーブル生成
    	for (uint32_t i = 0; i <= 36000; ++i)
		{
			//テイラー展開
			l_x = SonikMath::mod(radian, 2.0 * __SONIK_MATH_M_PI_D);
			// -π〜π に収めると精度が安定するらしい。
			if (l_x > __SONIK_MATH_M_PI_D)
			{
				l_x -= 2.0 * __SONIK_MATH_M_PI_D;
			};
			l_xs = l_x * l_x;

        	__SONIK_MATH_COS_LIST__[i] = 1.0 - l_xs / 2.0 + (l_xs * l_xs) / 24.0 - (l_xs * l_xs * l_xs) / 720.0; // 6次までの展開(SIN7次と同精度)
        	radian += step;
    	};

    	return 0;
	};

	//FFTで使うビット反転、反転因子テーブルを作成します。(4096個分の反転テーブル)
	//プログラム実行前の初期化時に一回だけ呼ばれる計算。
	// N は 4096固定（2^12）
	// 1024/2048 FFT でも流用可能
	int8_t CreateFFTUsedTable(void)
	{
    	const uint32_t N = 4096;
    	const uint32_t bits = 12; // log2(4096)

		//ビット反転テーブル事前作成
    	for (uint32_t i = 0; i < N; i++)
    	{
        	uint32_t x = i;
        	uint32_t r = 0;

        	// 12bit 反転
        	for (uint32_t b = 0; b < bits; b++)
        	{
            	r = (r << 1) | (x & 1);
            	x >>= 1;
        	};

        	__SONIK_MATH_FFT_BITREVERSE_TABLE__[i] = r;
    	};

		//反転因子テーブル事前作成
		double invN = 1.0 / static_cast<double>(N);
		for (uint32_t k = 0; k < N; k++)
    	{
        	// θ = -2πk/N
        	double theta = -2.0 * __SONIK_MATH_M_PI_D * k * invN;
        	__SONIK_MATH_FFT_TWIDDLE_TABLE__[k].re = Cos(theta);
        	__SONIK_MATH_FFT_TWIDDLE_TABLE__[k].im = -Sin(theta);
    	};

		return 0;
	};

	//グローバルコール。こうすることで.a(.lib)は意識せず自動的にテーブルが生成され使うことが可能になる。
	//グローバル領域のため、ここを通った時初回のみ１回だけコールされる。
	//よってマルチスレッド環境下においても１回のみが保証される。
	int8_t __SONIK_MATH_INITIALIZECALLED_GLOBAL_DUMMY_NUMBER__	= Initialize();
	int8_t __SONIK_MATH_BITREVERSETABLE_GLOBAL_DUMMY_NUMBER__	= CreateFFTUsedTable();

	//SIN, COSで使う定数
	//const double __SONIK_MATH_GL_STEP = __SONIK_MATH_M_PI_D / 18000.0;
	const double __SONIK_MATH_GL_INVSTEP = 18000.0 / __SONIK_MATH_M_PI_D; // 1/step

	//Sin値を取得します。
	//引数1: ラジアン角を指定します。
	//戻り値：
	DEF_FORCE_INLINE double Sin(double radian)
	{
    	// インデックス計算 (modをインデックス側で処理)
    	int32_t index = static_cast<int32_t>(radian * __SONIK_MATH_GL_INVSTEP + 0.5);
		if(index < 0)
		{
			index += 36001;
		};

    	// 0〜36000 に収める
    	index %= 36001;

    	// テーブル参照
    	return __SONIK_MATH_SIN_LIST__[index];
	};

	//Sin値を取得します。
	//引数1: ラジアン角を指定します。
	//float版
	DEF_FORCE_INLINE float SinF(float radian)
	{
    	// インデックス計算 (modをインデックス側で処理)
    	int32_t index = static_cast<int32_t>(radian * __SONIK_MATH_GL_INVSTEP + 0.5f);
		if(index < 0)
		{
			index += 36001;
		};

    	// 0〜36000 に収める
    	index %= 36001;

    	// テーブル参照
    	return __SONIK_MATH_SIN_LIST__[index];
	};


	//Cos値を取得します。
	//引数1: ラジアン角を指定します。
	DEF_FORCE_INLINE double Cos(double radian)
	{
    	// インデックス計算 (modをインデックス側で処理)
    	int32_t index = static_cast<int32_t>(radian * __SONIK_MATH_GL_INVSTEP + 0.5);
    	if (index < 0)
		{
        	index += 36001; // 負の入力補正
    	};
		
		// 0〜36000 に収める
		index %= 36001;

    	// テーブル参照
    	return __SONIK_MATH_COS_LIST__[index];
	};

	//Cos値を取得します。
	//引数1: ラジアン角を指定します。
	//flaot版
	DEF_FORCE_INLINE float CosF(float radian)
	{
    	// インデックス計算 (modをインデックス側で処理)
    	int32_t index = static_cast<int32_t>(radian * __SONIK_MATH_GL_INVSTEP + 0.5f);
    	if (index < 0)
		{
        	index += 36001; // 負の入力補正
    	};
		
		// 0〜36000 に収める
		index %= 36001;

    	// テーブル参照
    	return __SONIK_MATH_COS_LIST__[index];
	};

	//Tan値を取得します。
	//引数1: ラジアン角を指定します。
	DEF_FORCE_INLINE double Tan(double radian)
	{
    	double cosVal = Cos(radian);

    	// cos がほぼゼロのときは発散するので安全対策
    	if (SonikMath::abs(cosVal) < 1e-12)
		{
        	// 無限大に近い値を返す。(inf返し)
        	union
			{
        		uint64_t u;
        		double d;
    		} v;

    		v.u = 0x7FF0000000000000ULL; // IEEE754 正の無限大のビットパターン
    		return v.d;
    	};
		
		return Sin(radian) / cosVal;
	};

	//Tan値を取得します。
	//引数1: ラジアン角を指定します。
	//float版
	DEF_FORCE_INLINE float TanF(float radian)
	{
    	float cosVal = CosF(radian);

    	// cos がほぼゼロのときは発散するので安全対策
    	if (SonikMath::abs(cosVal) < 1e-12)
		{
        	// 無限大に近い値を返す。(inf返し)
        	union
			{
        		uint32_t u;
        		float f;
    		} v;

    		v.u = 0x7F800000U; // IEEE754 float 正の無限大
    		return v.f;
    	};
		
		return SinF(radian) / cosVal;
	};


	//角度値をラジアン角値に変換します。
	//引数1: ラジアン角値に変換したい角度値を指定します。
	DEF_FORCE_INLINE double digree_To_radian(double digree)
	{
		return digree * __SONIK_MATH_M_PI_D / 180.0;
	};

	//角度値をラジアン角値に変換します。
	//引数1: ラジアン角値に変換したい角度値を指定します。
	DEF_FORCE_INLINE float digree_To_radianF(float digree)
	{
		return digree * __SONIK_MATH_M_PI_F / 180.0f;
	};

	//ラジアン角値を角度値に変換します。
	//引数1: 角度値に変換したいラジアン値を指定します。
	DEF_FORCE_INLINE double radian_To_digree(double radian)
	{
		return radian * 180.0 / __SONIK_MATH_M_PI_D;
	};

	//ラジアン角値を角度値に変換します。
	//引数1: 角度値に変換したいラジアン値を指定します。
	DEF_FORCE_INLINE float radian_To_digree(float radian)
	{
		return radian * 180.0f / __SONIK_MATH_M_PI_F;
	};

    //Disntance Fourier Transform(離散フーリエ変換)
    bool DFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST& out_Sampling)
    {
        //公式：X[k] = Σ_{n=0}^{N-1} x[n] * e^(-i*2πkn/N)
        int N = in_Sampling->GetAllocCount();
		if(N == 0)
		{
			//0除算対策。0なら変換しない。
			return false;
		};
		
        SonikComplex sumcomp; // Σ の「足し合わせる器」 → 出力 X[k] を作るための累積変数
        SonikComplex tmpcomplex; //ループ内計算用
        double theta; // θ
        SLIB_SAMPLINGLIST outobj;

        if(!SonikLib::Container::SonikVariableArrayContainer<SonikComplex>::CreateContainer(outobj, N))
        {
            return false;
        };

        for(uint32_t k=0; k < N; ++k)
        {
            sumcomp.re = 0.0;
            sumcomp.im = 0.0;

            for(uint32_t n=0; n < N; ++n)
            {
                // Σ の範囲 (n=0→N-1)
                // θ = (2π/N) * (k*n)
                theta =  2.0 * __SONIK_MATH_M_PI_D * k * n / N;
                
                // e^(-iθ) = cosθ - i sinθ
                tmpcomplex.re = Cos(theta);     // 実部 = cosθ
                tmpcomplex.im = -(Sin(theta));  // 虚部 = (-sinθ)i
                
                // Σ の中身：x[n] * e^(-iθ)
                sumcomp += (*in_Sampling)[n] * tmpcomplex;
            };
            // Σ の結果を代入
            (*outobj)[k] = sumcomp;
        };

        out_Sampling = outobj;
        return true;
    };

	//Inverse Disntance Fourier Transform(逆離散フーリエ変換)
	//DFTのSIN算出の符号が逆になるだけ。
    bool IDFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST& out_Sampling)
	{
        //公式：x[n] = (1/N) Σ_{k=0}^{N-1} X[k] * e^(+i*2πkn/N)
        int N = in_Sampling->GetAllocCount();
		if(N == 0)
		{
			//0除算対策。0なら変換しない。
			return false;
		};

		double InvN = 1 / static_cast<double>(N);
        SonikComplex sumcomp; 	 // Σ の「足し合わせる器」 → 出力 X[k] を作るための累積変数
        SonikComplex tmpcomplex; //ループ内計算用
        double theta; 			 // θ
        SLIB_SAMPLINGLIST outobj;

        if(!SonikLib::Container::SonikVariableArrayContainer<SonikComplex>::CreateContainer(outobj, N))
        {
            return false;
        };

        for(uint32_t n=0; n < N; ++n)
        {
            sumcomp.re = 0.0;
            sumcomp.im = 0.0;

            for(uint32_t k=0; k < N; ++k)
            {
                // Σ の範囲 (k=0 → N-1)
                // θ = 2πkn/N ※+i* は imに代入するという意味なので計算には入れない。
                theta =  2.0 * __SONIK_MATH_M_PI_D * k * n / N;
                
                // e^(-iθ) = cosθ - i sinθ
                tmpcomplex.re = Cos(theta);  // 実部 = cosθ
                tmpcomplex.im = Sin(theta);  // 虚部 = (+sinθ)i
                
                // Σ の中身：x[k] * e^(-iθ)
                sumcomp += (*in_Sampling)[k] * tmpcomplex;
            };
            // Σ の結果を代入
			// (1/N) に対してΣ以降計算の算出値を掛ける
            (*outobj)[n].re = InvN * sumcomp.re;
			(*outobj)[n].im = InvN * sumcomp.im;

        };

        out_Sampling = outobj;
        return true;

	}

	//フーリエ変換
	//Fast Fourier Transform(高速フーリエ変換)
	//公式：
	//  X[k]       = E[k] + W_N^k * O[k]
	//  X[k+N/2]   = E[k] - W_N^k * O[k]
	//
	//  W_N^k = e^(-i * 2πk / N)
	//
	//  4096個の回転因子テーブルを縮小して使うことで
	//  1024/2048/4096 FFT に対応する。
	//第３引数はコール時は開始index, 関数終了時は処理後indexが格納されます。
	//第３引数を使用して継続して実行することが可能です。
	//配列すべてが完了しかたを知りたい場合は本関数終了後に第３引数に指定した変数とコンテナのサイズを比較してください。
	//512Byte版
	DEF_FORCE_INLINE bool FFT512Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
	        return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 512;

    	// すでに終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
		{
	        return true;
		};

    	// out は常に 512 サンプル必要
    	if (out_Sampling->GetAllocCount() < N)
		{
	        return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+511] を out[0..511] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
		{
        	return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
		{
        	(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（512 = 9bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 9u; // 4096→512 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];
        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//1024Byte版
	DEF_FORCE_INLINE bool FFT1024Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
	        return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 1024;

    	// すでに終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
		{
	        return true;
		};

    	// out は常に 512 サンプル必要
    	if (out_Sampling->GetAllocCount() < N)
		{
	        return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+1023] を out[0..1023] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
		{
        	return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
		{
        	(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（1024 = 10bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 10u; // 4096→1024 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];
        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//2048Byte版
	DEF_FORCE_INLINE bool FFT2048Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
	        return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 2048;

    	// すでに終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
		{
	        return true;
		};

    	// out は常に 512 サンプル必要
    	if (out_Sampling->GetAllocCount() < N)
		{
	        return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+2047] を out[0..2047] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
		{
        	return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
		{
        	(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（2048 = 11bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 11u; // 4096→2048 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];
        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//4096Byte版
	DEF_FORCE_INLINE bool FFT4096Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
	        return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 4098;

    	// すでに終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
		{
	        return true;
		};

    	// out は常に 512 サンプル必要
    	if (out_Sampling->GetAllocCount() < N)
		{
	        return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+4097] を out[0..4097] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
		{
        	return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
		{
        	(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（4098 = 12bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 12u; // 4098は縮小なし

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];
        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};

	//逆フーリエ変換
	//Inverse Fast Fourier Transform(逆高速フーリエ変換)
	//公式：
	//  x[n] = (1/N) * Σ_{k=0}^{N-1} X[k] * e^(+i * 2πkn / N)
	//
	// Cooley–Tukey 型の分割表現（偶数・奇数に分解）:
	//  x[n]       = (1/N) * { E[n] + W_N^{-n} * O[n] }
	//  x[n+N/2]   = (1/N) * { E[n] - W_N^{-n} * O[n] }
	//
	//  ここで W_N^k = e^(-i * 2πk / N)
	//  IFFT では FFT の回転因子 W_N^k の共役 W_N^{-k} を用い，
	//  最後に全サンプルを 1/N で正規化している。
	//
	//  4096個の回転因子テーブルを縮小して使うことで
	//  512/1024/2048/4096 IFFT に対応する。
	//第３引数はコール時は開始index, 関数終了時は処理後indexが格納されます。
	//第３引数を使用して継続して実行することが可能です。
	//配列すべてが完了しかたを知りたい場合は本関数終了後に第３引数に指定した変数とコンテナのサイズを比較してください。
	//512Byte版
	DEF_FORCE_INLINE bool IFFT512Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 512;

    	// 終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
        {
			return true;
		};

    	// out は常に 512 サンプル必要
    	if (out_Sampling->GetSizeMax() < N)
        {
			return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+511] を out[0..511] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
        {
			return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
        {
			(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（512 = 9bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 9u; // 4096→512 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算（IFFT は回転因子を共役にする）
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L_base = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];

        	// IFFT: 共役
        	SonikComplex W_L;
        	W_L.re =  W_L_base.re;
        	W_L.im = -W_L_base.im;

        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. IFFT の正規化（1/N）
    	// ------------------------------------------------------------
    	for (uint32_t i = 0; i < N; ++i)
    	{
        	(*outbuf)[i].re *= __SONIK_MATH_FFT_INVERSE512__;
        	(*outbuf)[i].im *= __SONIK_MATH_FFT_INVERSE512__;
    	};

    	// ------------------------------------------------------------
    	// 5. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//1024Byte版
	DEF_FORCE_INLINE bool IFFT1024Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 1024;

    	// 終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
        {
			return true;
		};

    	// out は常に 1024 サンプル必要
    	if (out_Sampling->GetSizeMax() < N)
        {
			return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+1023] を out[0..1023] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
        {
			return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
        {
			(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（1024 = 10bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 10u; // 4096→1024 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算（IFFT は回転因子を共役にする）
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L_base = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];

        	// IFFT: 共役
        	SonikComplex W_L;
        	W_L.re =  W_L_base.re;
        	W_L.im = -W_L_base.im;

        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. IFFT の正規化（1/N）
    	// ------------------------------------------------------------
    	for (uint32_t i = 0; i < N; ++i)
    	{
        	(*outbuf)[i].re *= __SONIK_MATH_FFT_INVERSE1024__;
        	(*outbuf)[i].im *= __SONIK_MATH_FFT_INVERSE1024__;
    	};

    	// ------------------------------------------------------------
    	// 5. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//2048Byte版
	DEF_FORCE_INLINE bool IFFT2048Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 2048;

    	// 終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
        {
			return true;
		};

    	// out は常に 2048 サンプル必要
    	if (out_Sampling->GetSizeMax() < N)
        {
			return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+2047] を out[0..2047] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
        {
			return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
        {
			(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（2048 = 11bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 11u; // 4096→2048 縮小

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算（IFFT は回転因子を共役にする）
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L_base = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];

        	// IFFT: 共役
        	SonikComplex W_L;
        	W_L.re =  W_L_base.re;
        	W_L.im = -W_L_base.im;

        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. IFFT の正規化（1/N）
    	// ------------------------------------------------------------
    	for (uint32_t i = 0; i < N; ++i)
    	{
        	(*outbuf)[i].re *= __SONIK_MATH_FFT_INVERSE2048__;
        	(*outbuf)[i].im *= __SONIK_MATH_FFT_INVERSE2048__;
    	};

    	// ------------------------------------------------------------
    	// 5. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};
	//4096Byte版
	DEF_FORCE_INLINE bool IFFT4096Ver(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint64_t& ref_variableindex)
	{
    	// 入力チェック
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	const uint32_t N = 4096;

    	// 終端を超えている → 正常終了
    	if (ref_variableindex >= insize)
        {
			return true;
		};

    	// out は常に 4096 サンプル必要
    	if (out_Sampling->GetSizeMax() < N)
        {
			return false;
		};

    	// ------------------------------------------------------------
    	// 1. in[ref_variableindex .. ref_variableindex+4095] を out[0..4095] にコピー
    	// ------------------------------------------------------------
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* inbuf  = in_Sampling.GetPointer();
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

    	// 範囲外アクセス防止（Normalize 済みなら不要だが安全のため）
    	if (ref_variableindex + N > insize)
        {
			return false;
		};

    	for (uint32_t i = 0; i < N; ++i)
        {
			(*outbuf)[i] = (*inbuf)[ref_variableindex + i];
		};

    	// ------------------------------------------------------------
    	// 2. ビット反転（4096 = 11bit）
    	// ------------------------------------------------------------
    	const uint32_t shift = 12u - 12u; // 4096 縮小無し

    	for (uint32_t i = 0; i < N; ++i)
    	{
        	uint32_t j = __SONIK_MATH_FFT_BITREVERSE_TABLE__[i] >> shift;
        	if (i < j)
        	{
            	SonikComplex t = (*outbuf)[i];
            	(*outbuf)[i] = (*outbuf)[j];
            	(*outbuf)[j] = t;
        	};
    	};

    	// ------------------------------------------------------------
    	// 3. バタフライ演算（IFFT は回転因子を共役にする）
    	// ------------------------------------------------------------
    	for (uint32_t L = 2u; L <= N; L <<= 1)
    	{
        	const uint32_t step = 4096u / L;
        	const SonikComplex W_L_base = __SONIK_MATH_FFT_TWIDDLE_TABLE__[step];

        	// IFFT: 共役
        	SonikComplex W_L;
        	W_L.re =  W_L_base.re;
        	W_L.im = -W_L_base.im;

        	const uint32_t halfL = (L >> 1);

        	for (uint32_t p = 0; p < N; p += L)
        	{
            	SonikComplex W(1.0, 0.0);

            	for (uint32_t k = 0; k < halfL; ++k)
            	{
                	const uint32_t idx_even = p + k;
                	const uint32_t idx_odd  = idx_even + halfL;

                	const SonikComplex u = (*outbuf)[idx_even];
                	const SonikComplex v = (*outbuf)[idx_odd] * W;

                	(*outbuf)[idx_even] = u + v;
                	(*outbuf)[idx_odd]  = u - v;

                	W *= W_L;
            	};
        	};
    	};

    	// ------------------------------------------------------------
    	// 4. IFFT の正規化（1/N）
    	// ------------------------------------------------------------
    	for (uint32_t i = 0; i < N; ++i)
    	{
        	(*outbuf)[i].re *= __SONIK_MATH_FFT_INVERSE4096__;
        	(*outbuf)[i].im *= __SONIK_MATH_FFT_INVERSE4096__;
    	};

    	// ------------------------------------------------------------
    	// 5. 次のブロックへ進める
    	// ------------------------------------------------------------
    	ref_variableindex += N;
    	return true;
	};

	//一括FFT
	DEF_FORCE_INLINE bool FullRangeFFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint32_t NLevel)
	{
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	if (insize == 0)
		{
        	return true;
		};
		
		if(NLevel > 3)
		{
			//NLevelが最大値以上なら駄目。
			return false;
		};

		uint64_t N = 0;
		switch(NLevel)
		{
		case 0:
			N = 512;
			break;

		case 1:
			N = 1024;
			break;

		case 2:
			N = 2048;
			break;

		case 3:
			N = 4096;
			break;

		default:
			return false;
		};

    	// --- in の拡張（ゼロパディング） ---
    	uint64_t blocks = (insize + N - 1) / N;
    	uint64_t targetSize = blocks * N;

    	if (targetSize != insize)
    	{
        	if (!in_Sampling->ReSize(targetSize))
			{
            	return false;
			};

        	for (uint64_t i = insize; i < targetSize; i++)
        	{
            	(*in_Sampling)[i].re = 0.0;
            	(*in_Sampling)[i].im = 0.0;
        	};
    	};

		// --- out の拡張 ---
		if (out_Sampling->GetSizeMax() < targetSize)
		{
    		if (!out_Sampling->ReSize(targetSize))
    		{
        		return false;
    		};
		};

		// --- ブロック処理（必ず実行されるべき） ---
		uint64_t idx = 0;
		SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();
		
		switch(NLevel)
		{
		case 0:
			//512コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!FFT512Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 1:
			//1024コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!FFT1024Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 2:
			//2048コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!FFT2048Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 3:
			//4096コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!FFT4096Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		default:
			return false;
		};

    	return true;
	};

	//一括IFFT
	DEF_FORCE_INLINE bool FullRangeIFFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Sampling, uint32_t NLevel)
	{
    	if (in_Sampling.IsNullptr() || out_Sampling.IsNullptr())
		{
        	return false;
		};

    	const uint64_t insize = in_Sampling->GetAllocCount();
    	if (insize == 0)
		{
        	return true;
		};
		
		if(NLevel > 3)
		{
			//NLevelが最大値以上なら駄目。
			return false;
		};

		uint64_t N = 0;
		switch(NLevel)
		{
		case 0:
			N = 512;
			break;

		case 1:
			N = 1024;
			break;

		case 2:
			N = 2048;
			break;

		case 3:
			N = 4096;
			break;

		default:
			return false;
		};

    	// --- in の拡張（ゼロパディング） ---
    	uint64_t blocks = (insize + N - 1) / N;
    	uint64_t targetSize = blocks * N;

    	if (targetSize != insize)
    	{
        	if (!in_Sampling->ReSize(targetSize))
			{
            	return false;
			};

        	for (uint64_t i = insize; i < targetSize; i++)
        	{
            	(*in_Sampling)[i].re = 0.0;
            	(*in_Sampling)[i].im = 0.0;
        	};
    	};

		// --- out の拡張 ---
		//outに対してclear;
		out_Sampling->Clear();
		if (out_Sampling->GetSizeMax() < targetSize)
		{
    		if (!out_Sampling->ReSize(targetSize))
    		{
        		return false;
    		};
		};

		// --- ブロック処理（必ず実行されるべき） ---
		uint64_t idx = 0;
		SonikLib::Container::SonikVariableArrayContainer<SonikComplex>* outbuf = out_Sampling.GetPointer();

		switch(NLevel)
		{
		case 0:
			//512コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!IFFT512Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 1:
			//1024コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!IFFT1024Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 2:
			//2048コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!IFFT2048Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		case 3:
			//4096コール(Inline化を期待して関数ポインタによる可読性から速度重視へ変更実装)
			while (idx < targetSize)
			{
    			uint64_t prev_idx = idx;

    			if (!IFFT4096Ver(in_Sampling, out_Sampling, idx))
    			{
        			return false;
    			};

    			for (uint32_t i = 0; i < N; ++i)
    			{
        			(*outbuf)[prev_idx + i] = (*outbuf)[i];
    			};
			};
			break;

		default:
			return false;
		};

    	return true;
	};

	//取得関数FullFFT
	DEF_FORCE_INLINE bool Get_FullRange_FFTFunction(uint64_t inSamplingsize, FFTFullFunctionP& out_Full_FFT_funcptr, FFTFullFunctionP& out_Full_IFFT_funcptr, uint32_t& out_NLevel)
	{
		//サイズ1より下なら実施しない。
		if(inSamplingsize < 1)
		{
			return false;
		};

		//単位レベル(NLevel)の自動選択(スコア方式)
    	double bestScore = 1e100;
    	uint32_t bestLevel = 0;

    	for (uint32_t level = 0; level <= 3; ++level)
    	{
        	uint32_t N = 512 << level;
        	if (N == 0) continue;

        	// 拡張後サイズ（N の倍数に切り上げ）
        	uint64_t expanded = ((inSamplingsize + N - 1) / N) * N;
        	uint64_t pad = expanded - inSamplingsize ;

        	// ループ回数
        	double loops = (double)expanded / (double)N;

        	// 拡張量の相対コスト
        	double padCost = (double)pad / (double)N;

        	// ペナルティ：拡張量が N/2 を超える場合
        	double penalty = (pad > (N / 2)) ? 1.0 : 0.0;

        	double score = loops + padCost + penalty;

        	if (score < bestScore)
        	{
            	bestScore = score;
            	bestLevel = level;
        	};
    	};

    	out_NLevel = bestLevel;
		out_Full_FFT_funcptr = &SonikMath::FullRangeFFT;
		out_Full_IFFT_funcptr = &SonikMath::FullRangeIFFT;
    	return true;
	};

	//取得関数FixedFFT
	DEF_FORCE_INLINE bool Get_FixedRange_FFTFuncction(SLIB_SAMPLINGLIST inSampling, FFTFixedFuntionP& out_Fixed_FFT_funcptr, FFTFixedFuntionP& out_Fixed_IFFT_funcptr, uint32_t NLevel)
	{
    	uint32_t baseN = 0;
		FFTFixedFuntionP l_fftfunc = nullptr;
		FFTFixedFuntionP l_ifftfunc = nullptr;

    	switch (NLevel)
    	{
        case 0:
			baseN = 512;
			l_fftfunc = &SonikMath::FFT512Ver;
			l_ifftfunc = &SonikMath::IFFT512Ver;
			break;
        case 1:
			baseN = 1024;
			l_fftfunc = &SonikMath::FFT1024Ver;
			l_ifftfunc = &SonikMath::IFFT1024Ver;
			break;

        case 2:
			baseN = 2048;
			l_fftfunc = &SonikMath::FFT2048Ver;
			l_ifftfunc = &SonikMath::IFFT2048Ver;
			break;

        	case 3:
			baseN = 4096;
			l_fftfunc = &SonikMath::FFT4096Ver;
			l_ifftfunc = &SonikMath::IFFT4096Ver;
			break;

        default:
            return false; // Nレベル不正
    	};

    	uint32_t inputN = inSampling->GetAllocCount();

    	// 何ブロック必要か？
    	uint32_t blocks = inputN / baseN;
    	if (inputN % baseN != 0)
		{
        	++blocks;
		};
		
		uint32_t targetSize = blocks * baseN;
		
		// 必要なら拡張（ゼロパディング）
		if (targetSize != inputN)
		{
			if (!inSampling->ReSize(targetSize))
			{
				return false;
			};

			for (uint32_t i = inputN; i < targetSize; i++)
			{
            	(*inSampling)[i].re = 0.0;
            	(*inSampling)[i].im = 0.0;
        	};
    	};

		out_Fixed_FFT_funcptr = l_fftfunc;
		out_Fixed_IFFT_funcptr = l_ifftfunc;
    	return true;
	};

    //窓関数
    //窓関数ロジック：ハニング
    DEF_FORCE_INLINE void ApplyWindow_Hann(SLIB_SAMPLINGLIST inSampling)
	{
		uint64_t dataSize = inSampling->GetAllocCount();
		double angleStep = 2.0 * __SONIK_MATH_M_PI_D / (static_cast<double>(dataSize) - 1.0);
        for (uint64_t i = 0; i < dataSize; ++i)
        {
            double win = 0.5 - 0.5 * SonikMath::Cos(angleStep * i);
            (*inSampling)[i].re *= win;
            (*inSampling)[i].im *= win;
        };
	};
    //窓関数ロジック：ハミング
    DEF_FORCE_INLINE void ApplyWindow_Hamming(SLIB_SAMPLINGLIST inSampling)
	{
		uint64_t dataSize = inSampling->GetAllocCount();
		double angleStep = 2.0 * __SONIK_MATH_M_PI_D / (static_cast<double>(dataSize) - 1.0);
        for (uint64_t i = 0; i < dataSize; ++i)
        {
            double win = 0.54 - 0.46 * SonikMath::Cos(angleStep * i);
            (*inSampling)[i].re *= win;
            (*inSampling)[i].im *= win;
        };
	};
    //窓関数ロジック：ブラックマン
    DEF_FORCE_INLINE void ApplyWindow_Blackman(SLIB_SAMPLINGLIST inSampling)
	{
		uint64_t dataSize = inSampling->GetAllocCount();
		double angleStep = 2.0 * __SONIK_MATH_M_PI_D / (static_cast<double>(dataSize) - 1.0);
        for (uint64_t i = 0; i < dataSize; ++i)
        {
            double a = angleStep * i;
            // 0.42 - 0.5*cos(a) + 0.08*cos(2a)
            double win = 0.42 - 0.5 * SonikMath::Cos(a) + 0.08 * SonikMath::Cos(2.0 * a);
            (*inSampling)[i].re *= win;
            (*inSampling)[i].im *= win;
        }
	};


	// サンプリングレート変換 (44100 -> 48000 など) ノイズ抑制版
	bool ReSamplingTransform(SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Resampled, double currentRate, double targetRate)
	{
    	if (in_Sampling.IsNullptr() || out_Resampled.IsNullptr())
		{
			return false;
		};
		
		//入力をコピー
		SLIB_SAMPLINGLIST l_tmpin_sampling;
		if(!SonikLib::Container::SonikVariableArrayContainer<SonikComplex>::CreateContainer(l_tmpin_sampling, in_Sampling->GetAllocCount()))
		{
			return false;
		};

		if(!l_tmpin_sampling->Copy(in_Sampling))
		{
			return false;
		};

    	// 1. 入力データの窓関数適用 (周波数解析時のリークを抑制)
    	// ※一括処理なので、全体に対してハニング窓を適用
    	ApplyWindow_Hann(l_tmpin_sampling);

    	// 2. FFT準備
    	FFTFullFunctionP fftFunc, ifftFunc;
    	uint32_t nLevel;
    	uint64_t originalInSize = l_tmpin_sampling->GetAllocCount();
		
    	if (!Get_FullRange_FFTFunction(originalInSize, fftFunc, ifftFunc, nLevel))
		{
			return false;
		};

    	// 一括FFT実行
    	//if (!fftFunc(in_Sampling, out_Resampled, nLevel))
		//ライブラリ内なので直呼びに変更(パフォーマンスアップ)
		if (!FullRangeFFT(l_tmpin_sampling, out_Resampled, nLevel)) 
		{
			return false;
		};

    	// FFT処理されたサイズ（パディング後）
    	uint64_t fftSize = l_tmpin_sampling->GetAllocCount();
    
    	// 3. 変換後のサイズ計算
    	uint64_t newSize = static_cast<uint64_t>(static_cast<double>(fftSize) * targetRate / currentRate);

    	// 作業用コンテナの生成 (freqContainer)
    	SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<SonikComplex>> freqContainer;
    	SonikLib::Container::SonikVariableArrayContainer<SonikComplex>::CreateContainer(freqContainer);
    	if (!freqContainer->ReSize(newSize))
		{
			return false;
		};
		
    	// 4. 周波数スペクトルの移行 (エイリアシング防止フィルタを兼ねる)
    	// 移行すべき成分は、変換前後の「低い方のナイキスト周波数」まで
    	uint64_t copyLimit = (fftSize < newSize) ? (fftSize / 2) : (newSize / 2);

    	// --- 高域ノイズをカットしながらコピー ---
		// 正の周波数領域
    	for (uint64_t i = 0; i < copyLimit; ++i)
		{
        	(*freqContainer)[i] = (*out_Resampled)[i];
    	};

    	// 中間（高域成分）を0埋め (ダウンサンプリング時はここが「カット」に相当)
    	for (uint64_t i = copyLimit; i < (newSize - copyLimit); ++i)
		{
        	(*freqContainer)[i].re = 0.0;
        	(*freqContainer)[i].im = 0.0;
    	};
    	// 負の周波数領域
    	for (uint64_t i = 1; i <= copyLimit; ++i)
		{
        	(*freqContainer)[newSize - i] = (*out_Resampled)[fftSize - i];
    	};

    	// 5. 逆FFTの実行
    	FFTFullFunctionP ifftFuncNew, fftFuncNew;
    	uint32_t nLevelNew;
    	if (!Get_FullRange_FFTFunction(newSize, fftFuncNew, ifftFuncNew, nLevelNew))
		{
			return false;
		};

    	// 一括IFFT (結果は out_Resampled に格納)
    	//if (!ifftFuncNew(freqContainer, out_Resampled, nLevelNew))
		//ライブラリ内部なので直呼びに変更(パフォーマンスアップ)
		if (!FullRangeIFFT(freqContainer, out_Resampled, nLevelNew))
		{
			return false;
		};

    	// 6. 後処理：ゲイン補正と窓関数の影響調整
    	// レート変換比率(gainAdjust)と、窓関数による振幅減少の平均的な補正(約2.0倍)を掛ける
    	double gainAdjust = (targetRate / currentRate) * 2.0; 
    	uint64_t finalSize = out_Resampled->GetAllocCount();
    	for (uint64_t i = 0; i < finalSize; ++i)
		{
        	(*out_Resampled)[i].re *= gainAdjust;
        	(*out_Resampled)[i].im *= gainAdjust;
    	};

    	return true;
	};

#ifdef _DEBUG

		//Sin値及びCos値のリストをtxtに出力します。
		//_DEBUGが定義されている時のみ有効です。
	void SinCosValueOutPut(void)
	{
		std::ofstream ofs("Debug_Sin_Cos_Value.txt");
		std::ofstream mofs("Debug_Sin_Cos_MinusValue.txt");
		std::string _str;
		float digree = 0.0f;
		float ret = 0.0f;
		float ret_c = 0.0f;
		int8_t buffer[300];

		sprintf(reinterpret_cast<char*>(buffer), "Sin値　Cos値 表 ============================\n\n");
		_str += reinterpret_cast<char*>(buffer);
		memset(buffer, 0, 300);

		while(digree < 360.0)
		{
			ret = SinF(digree);
			ret_c = sin(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[SinF] = [sin] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);

			ret = CosF(digree);
			ret_c = cos(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[CosF] = [cos] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);

			ret = TanF(digree);
			ret_c = tan(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[TanF] = [tan] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);


			digree += 0.1f;
		};

		ofs << _str.c_str() << std::endl;

		_str = "";
		digree = 0.0f;

		sprintf(reinterpret_cast<char*>(buffer), "負のSin値　負のCos値 表 ============================\n\n");
		_str += reinterpret_cast<char*>(buffer);
		memset(buffer, 0, 300);

		while( digree > (-360.0) )
		{
			ret = SinF(digree);
			ret_c = sin(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[SinF] = [sin] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);

			ret = CosF(digree);
			ret_c = cos(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[CosF] = [cos] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);

			ret = TanF(digree);
			ret_c = tan(digree * __SONIK_MATH_M_PI_D / 180.0);

			sprintf(reinterpret_cast<char*>(buffer), "Value:[TanF] = [tan] %-7.5f = %-7.5f   \n", ret, ret_c);
			_str += reinterpret_cast<char*>(buffer);
			memset(buffer, 0, 300);

				digree -= 0.1f;
		};
		mofs << _str.c_str() << std::endl;
	};

#endif


};