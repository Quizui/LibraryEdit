/*
 * SonikNormalize.cpp
 *
 *  Created on: 2019/01/20
 *      Author: SONIC
 */

#include "SonikNormalize.h"

namespace SonikMath
{

	//0.0 ~ 1.0へ正規化します。
	double Normalize(double MaxValue, double NowValue)
	{
		if(NowValue >= MaxValue)
		{
			return 1.0;

		}else if(NowValue <= 0)
		{
			return 0.0;
		};

		return NowValue / MaxValue;
	};


	//0.0 ~ 1.0へ正規化します。
	//最大値を0.0
	//最小値を1.0
	//として扱います。
	double NormalizeInverse(double MaxValue, double NowValue)
	{
		if(NowValue >= MaxValue)
		{
			return 0.0;

		}else if(NowValue <= 0)
		{
			return 1.0;
		};

		return 1.0 - (NowValue / MaxValue);
	};


	//引数１の値を 引数4 ~ 引数5 の範囲に正規化します。
	// Y = ( (X - Xmin) / (Xmax - Xmin) ) * (Max - Min) + Min
	inline float Normalize_Max_to_MinF(long NTV, long NTV_Max, long NTV_Min, long NormalizeMax, long NormalizeMin)
	{
		if(NTV_Max <= NTV_Min)
		{
			return 0.0f;
		};

		if(NormalizeMax < NormalizeMin)
		{
			return 0.0f;
		}

		float value[5];
		value[0] = static_cast<float>(NTV);
		value[1] = static_cast<float>(NTV_Max);
		value[2] = static_cast<float>(NTV_Min);
		value[3] = static_cast<float>(NormalizeMax);
		value[4] = static_cast<float>(NormalizeMin);

		return ( (value[0] - value[2]) / (value[1] - value[2]) ) * (value[3] - value[4]) + value[4];
	};

	//double版
	inline double Normalize_Max_to_Min(long long NTV, long long NTV_Max, long long NTV_Min, long long NormalizeMax, long long NormalizeMin)
	{
		if(NTV_Max <= NTV_Min)
		{
			return 0.0;
		};

		if(NormalizeMax < NormalizeMin)
		{
			return 0.0;
		}

		float value[5];
		value[0] = static_cast<double>(NTV);
		value[1] = static_cast<double>(NTV_Max);
		value[2] = static_cast<double>(NTV_Min);
		value[3] = static_cast<double>(NormalizeMax);
		value[4] = static_cast<double>(NormalizeMin);

		return ( (value[0] - value[2]) / (value[1] - value[2]) ) * (value[3] - value[4]) + value[4];
	};


};


