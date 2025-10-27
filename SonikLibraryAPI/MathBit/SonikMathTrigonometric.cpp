/*
 * SonikMathTrigonometric.cpp
 *
 *  Created on: 2015/06/02
 *      Author: SONIK
 */

#include "SonikMathTrigonometric.h"

#include "../SonikCAS/SonikAtomicLock.h"
#include <new>
#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES

#ifdef _DEBUG
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#endif

namespace SonikMath
{
	static const float __SONIK_MATH_M_PI_F = 3.14159265358979323846;
	static const double __SONIK_MATH_M_PI_D =  3.1415926535897932384626433832795029L;

	double __SONIK_MATH_SIN_LIST__[901];
	SonikLib::S_CAS::SonikAtomicLock global_initblock;

	//イニシャライザです。
	//おそらく初期化時に一回だけ呼ばれる計算。マルチスレッドで一回以上呼ばれてもAtomicによるブロックが働くので大丈夫なはず。
	int8_t Initialize(void)
	{
		global_initblock.lock();

		static bool SONIK_MATH_INIT_FLAG__ = false;

		if(SONIK_MATH_INIT_FLAG__)
		{
			global_initblock.unlock();
			return 0;
		};


		uint32_t i =0;
		double radian =0;
		double digree = 0;
		for(i = 0; i < 901; ++i)
		{
			if( i == 0 )
			{
				__SONIK_MATH_SIN_LIST__[i] = digree;

			}else
			{
				radian = digree * 3.14159265358979323846 / 180.0;
				__SONIK_MATH_SIN_LIST__[i] = sin(radian);
			};

			digree += 0.1f;
		};

		global_initblock.unlock();
		return 0;
	};

	int8_t __SONIK_MATH_INITIALIZECALLED_GLOBAL_VALIABLE_NUMBER__ = Initialize();


	//Sin値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	double Sin(double digree)
	{
		double math_signed = 1.0;
		double fixidmath_signed = 1.0;
		double rounds_ = 0.05;
		long index = 0;
#ifdef _DEBUG
		double ret = 0.0;
#endif

		if(digree < 0.0 )
		{
			math_signed = -1.0;
		};


		digree *= math_signed;
		digree -= 360.0 * (static_cast<int32_t>( digree / 360.0 ) );

		if(digree > 90.0 && digree < 180.0001)
		{
			digree = 180 - digree;
		}else if(digree > 180.0 && digree < 270.0001 )
		{
			digree -= 180.0;
			fixidmath_signed = -1.0;
		}else if(digree > 270.0 && digree < 360.0001)
		{
			digree = 360.0 - digree;
			fixidmath_signed = -1.0;
		};

		digree += rounds_;

		index = static_cast<int32_t>( digree * 10.0 );

#ifdef _DEBUG
		ret = ( __SONIK_MATH_SIN_LIST__[index] * fixidmath_signed );
		ret *= math_signed;
		return ret;
#endif

		//DEBUG版との違いは、計算過程が見れるかどうか。
		return ( ( __SONIK_MATH_SIN_LIST__[index] * fixidmath_signed ) * math_signed );
	};


	//Sin値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	//Float版
	float SinF(float digree)
	{
		float math_signed = 1.0;
		float fixidmath_signed = 1.0;
		float rounds_ = 0.05;
		long index = 0;
#ifdef _DEBUG
		float ret = 0.0;
#endif

		if(digree < 0.0 )
		{
			math_signed = -1.0;
		};

		digree *= math_signed;
		digree -= 360.0 * (static_cast<int32_t>( digree / 360.0 ) );

		if(digree > 90.0 && digree < 180.0001)
		{
			digree = 180 - digree;
		}else if(digree > 180.0 && digree < 270.0001 )
		{
			digree -= 180.0;
			fixidmath_signed = -1.0;
		}else if(digree > 270.0 && digree < 360.0001)
		{
			digree = 360.0 - digree;
			fixidmath_signed = -1.0;
		};

		digree += rounds_;

		index = static_cast<int32_t>( digree * 10.0 );


#ifdef _DEBUG
		ret = ( __SONIK_MATH_SIN_LIST__[index] * static_cast<float>(fixidmath_signed) );
		ret *= math_signed;
		return ret;
#endif

		//DEBUG版との違いは、計算過程が見れるかどうか。
		return ( ( __SONIK_MATH_SIN_LIST__[index] * static_cast<float>(fixidmath_signed) ) * math_signed );
	}


	//Cos値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	double Cos(double digree)
	{
		double math_signed = 1.0;
#ifdef _DEBUG
		double ret = 0;
#endif

		if(digree < 0 )
		{
			math_signed = -1.0;
		};


		digree *= math_signed;
		digree += 90.0;

#ifdef _DEBUG
		ret = Sin(digree);
		return ret;
#endif

		return Sin(digree);
	};

	//Cos値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	//Float版
	float CosF(float digree)
	{
		float math_signed = 1.0;
#ifdef _DEBUG
		float ret = 0;
#endif

		if(digree < 0 )
		{
			math_signed = -1.0;
		};


		digree *= math_signed;
		digree += 90.0;

#ifdef _DEBUG
		ret = SinF(digree);
		return ret;
#endif

		return SinF(digree);
	};

	//Tan値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	double Tan(double digree)
	{
		double _sin;
		double _cos;
#ifdef _DEBUG
		double ret = 0;
#endif

		_sin = Sin(digree);
		_cos = Cos(digree);

		if( _cos == 0.0 )
		{
			return 0.0;
		};

#ifdef _DEBUG
		ret = _sin / _cos;
		return ret;
#endif

		return _sin / _cos;
	};

	//Tan値を取得します。
	//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
	//Float版
	float TanF(float digree)
	{
		float _sin;
		float _cos;
#ifdef _DEBUG
		float ret = 0;
#endif

		_sin = SinF(digree);
		_cos = CosF(digree);

		if( _cos == 0.0f )
		{
			return 0.0f;
		};

#ifdef _DEBUG
		ret = _sin / _cos;
		return ret;
#endif

		return _sin / _cos;
	};


	//角度値をラジアン角値に変換します。
	//引数1: ラジアン角値に変換したい角度値を指定します。
	double digree_To_radian(double digree)
	{
		return digree * __SONIK_MATH_M_PI_D / 180.0;
	};

	//角度値をラジアン角値に変換します。
	//引数1: ラジアン角値に変換したい角度値を指定します。
	float digree_To_radianF(float digree)
	{
		return digree * __SONIK_MATH_M_PI_F / 180.0f;
	};

	//ラジアン角値を角度値に変換します。
	//引数1: 角度値に変換したいラジアン値を指定します。
	double radian_To_digree(double radian)
	{
		return radian * 180.0 / __SONIK_MATH_M_PI_D;
	};

	//ラジアン角値を角度値に変換します。
	//引数1: 角度値に変換したいラジアン値を指定します。
	float radian_To_digree(float radian)
	{
		return radian * 180.0f / __SONIK_MATH_M_PI_F;
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

