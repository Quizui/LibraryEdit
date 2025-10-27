/*
 * SonikMathStandard.cpp
 *
 *  Created on: 2019/06/20
 *      Author: SONIC
 */

#include "SonikMathStandard.h"
#include "SonikMathTrigonometric.h"
#include "memory.h"
#include <stdint.h>
#include <stdexcept>

//#include <fftw3.h>

namespace SonikMath
{
	//2を底とする対数を計算します。
	int log2(int x)
	{
		int y;

		y = 0;
		while (x > 1)
		{
			x >>= 1;
			y++;
		}

		return y;
	};

	//指定した値の2の累乗(るいじょう)数を計算します。
	int pow2(int x)
	{
		int y;

		if (x == 0)
		{
			y = 1;
		}
		else
		{
			y = 2 << (x - 1);
		}

		return y;
	};

	//小数点以下の切り捨てを行います。
	double floor(double _val_)
	{
	    int intValue = static_cast<int>(_val_);
	    if (_val_ < 0 && _val_ != intValue) {
	        return intValue - 1;
	    }
	    return intValue;
	};

	//余剰の計算をします。
	double mod(double _num_, double _denom_)
	{
	    if (_denom_ == 0.0)
	    {
	        // ０除算対策
	        return 0.0;
	    };

	    double result = _num_ - _denom_ * SonikMath::floor(_num_ / _denom_);

	    // 結果が負で、分母が正の場合、結果に分母を加えます
	    if (result < 0.0 && _denom_ > 0.0)
	    {
	        result += _denom_;
	    }
	    // 結果が正で、分母が負の場合、結果から分母を引きます
	    else if (result > 0.0 && _denom_< 0.0)
	    {
	        result -= _denom_;
	    }

	    return result;
	};
	float mod(float _num_, float _denom_)
	{
	    if (_denom_ == 0.0)
	    {
	        // ０除算対策
	        return 0.0;
	    };

	    float result = _num_ - _denom_ * SonikMath::floor(_num_ / _denom_);

	    // 結果が負で、分母が正の場合、結果に分母を加えます
	    if (result < 0.0 && _denom_ > 0.0)
	    {
	        result += _denom_;
	    }
	    // 結果が正で、分母が負の場合、結果から分母を引きます
	    else if (result > 0.0 && _denom_< 0.0)
	    {
	        result -= _denom_;
	    }

	    return static_cast<float>(result);
	};
	int64_t mod(int64_t _num_, int64_t _denom_)
	{
	    if (_denom_ == 0)
	    {
	        // ０除算対策
	        return 0;
	    };

	    double result = _num_ - _denom_ * (_num_ / _denom_);

	    // 結果が負で、分母が正の場合、結果に分母を加えます
	    if (result < 0.0 && _denom_ > 0.0)
	    {
	        result += _denom_;
	    }
	    // 結果が正で、分母が負の場合、結果から分母を引きます
	    else if (result > 0.0 && _denom_< 0.0)
	    {
	        result -= _denom_;
	    }

	    return static_cast<int64_t>(result);
	};
	uint64_t mod(uint64_t _num_, uint64_t _denom_)
	{
	    if (_denom_ == 0)
	    {
	        // ０除算対策
	        return 0;
	    };

	    double result = _num_ - _denom_ * (_num_ / _denom_);

	    // 結果が負で、分母が正の場合、結果に分母を加えます
	    if (result < 0.0 && _denom_ > 0.0)
	    {
	        result += _denom_;
	    }
	    // 結果が正で、分母が負の場合、結果から分母を引きます
	    else if (result > 0.0 && _denom_< 0.0)
	    {
	        result -= _denom_;
	    }

	    return static_cast<uint64_t>(result);
	};


	//絶対値を計算します。
	int64_t abs(int64_t _abs_) noexcept
	{
		return (_abs_ < 0) ? (-_abs_) : _abs_;
	};
	double abs(double _abs_) noexcept
	{
		 return (_abs_ < 0) ? (-_abs_) : _abs_;
	};

	//負の数の対応した平方根を計算します。
	//第2引数：精度を指定することが可能です。
	double sqrt(double _sqrtval_, double _threshold_) noexcept
	{
	    if (_sqrtval_ < 0)
	    {
	        //throw std::invalid_argument("sqrt received negative number");
	    	return 0.0;
	    }

	    long long i;
	    double x, y, e;
	    const double threehalfs = 1.5;

	    double _E = _threshold_;

	    y  = _sqrtval_;
	    i  = * ( long long * ) &y;
	    i  = 0x5fe6ec85e7de30da - ( i >> 1 ); // 魔法の定数を調整
	    x  = * ( double * ) &i;
	    x  = x * ( threehalfs - ( 0.5 * x * x * y ) ); // 1回の反復
	    e = x * x - _sqrtval_;

	    while (SonikMath::abs(e) >= _E)
	    {
	        x -= e / (x * 2);
	        e = x * x - _sqrtval_;
	    };

	    return x;
	};

	//精度を犠牲にして高速に平方根を計算します。
	double sqrt_Quick(double _srtval_) noexcept
	{
		long i;
		double x2, y;
		const double threehalfs = 1.5F;

		x2 = _srtval_ * 0.5F;
		y  = _srtval_;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( double * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return (y != 0) ? (1/y) : 0.0;
	};
	float sqrtF_Quick(float _srtval_) noexcept
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = _srtval_ * 0.5F;
		y  = _srtval_;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return (y != 0) ? (1/y) : 0.0;
	};
	//精度を犠牲にして高速に逆平方根を計算します。
	double rsqrt_Quick(double _srtval_) noexcept
	{
		long i;
		double x2, y;
		const double threehalfs = 1.5F;

		x2 = _srtval_ * 0.5F;
		y  = _srtval_;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( double * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	};
	float rsqrtF_Quick(float _srtval_) noexcept
	{
		long i;
		double x2, y;
		const double threehalfs = 1.5F;

		x2 = _srtval_ * 0.5F;
		y  = _srtval_;
		i  = * ( long * ) &y;
		i  = 0x5f3759df - ( i >> 1 );
		y  = * ( double * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );

		return y;
	};

	//FFT(高速フーリエ変換)を行います。
	//作成途中。
	//void FFT_1d(SonikComplex* p_complex, int TargetSampling)
	//{
		//オイラーの公式
		//e^iθ = cosθ -i sinθ
		// i = -1の平方根(√-1)

		//

	//};

	//IFFT(逆高速フーリエ変換を行います。
	//void IFFT_1d(SonikComplex* p_complex, int TargetSampling);
};


