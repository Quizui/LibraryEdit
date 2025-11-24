#include "SonikMathNumeric.h"

#include "SonikMathStandard.h"
#include <new>
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

	double __SONIK_MATH_SIN_LIST__[36001] = {0};
	double __SONIK_MATH_COS_LIST__[36001] = {0};

	//イニシャライザです。
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

	int8_t __SONIK_MATH_INITIALIZECALLED_GLOBAL_VALIABLE_NUMBER__ = Initialize();

	//SIN, COSで使う定数
	const double __SONIK_MATH_GL_STEP = __SONIK_MATH_M_PI_D / 18000.0;
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
    DEF_FORCE_INLINE bool DFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST& out_Sampling)
    {
        //公式：X[k] = Σ_{n=0}^{N-1} x[n] * e^(-i*2πkn/N)
        int N = in_Sampling->GetAllocCount();
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