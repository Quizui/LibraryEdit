/*
 * SonikMathStandard.cpp
 *
 *  Created on: 2019/06/20
 *      Author: SONIC
 */

#include "SonikMathStandard.h"

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
	    int64_t intValue = static_cast<int64_t>(_val_);
	    if (_val_ < 0 && _val_ != intValue) {
	        return intValue - 1;
	    }
	    return static_cast<double>(intValue);
	};

	//余剰の計算をします。
	double mod(double _num_, double _denom_)
	{
    	if(_denom_ == 0.0)
		{
			return 0.0;
		};

    	double q = SonikMath::floor(_num_ / _denom_);
    	double result = _num_ - _denom_ * q;

    	if (result < 0.0)
		{
        	result += SonikMath::abs(_denom_);
    	};

	    return result;
	};
	float mod(float _num_, float _denom_)
	{
    	if(_denom_ == 0.0f)
		{
			return 0.0f;
		};

    	double q = SonikMath::floor(_num_ / _denom_);
    	double result = _num_ - _denom_ * q;

    	if (result < 0.0f)
		{
        	result += SonikMath::abs(_denom_);
    	};

	    return result;
	};
	int64_t mod(int64_t _num_, int64_t _denom_)
	{
    	if(_denom_ == 0)
		{
			return 0;
		};
		
		int64_t result = _num_ % _denom_;
		if (result < 0)
		{
			result += SonikMath::abs(_denom_);
		};
		
		return result;
	};
	uint64_t mod(uint64_t _num_, uint64_t _denom_)
	{
    	if (_denom_ == 0)
		{
			return 0;
		};
		
    	return _num_ % _denom_; // 常に正なのでそのままでOK
	};


	//絶対値を計算します。
	int64_t abs(int64_t _abs_) SLIB_CVR_NOEXCEPT
	{
		return (_abs_ < 0) ? (-_abs_) : _abs_;
	};
	double abs(double _abs_) SLIB_CVR_NOEXCEPT
	{
		 return (_abs_ < 0.0) ? (-_abs_) : _abs_;
	};
	float abs(float _abs_) SLIB_CVR_NOEXCEPT
	{
		 return (_abs_ < 0.0f) ? (-_abs_) : _abs_;
	};

	//負の数の対応した平方根を計算します。
	//第2引数：精度を指定することが可能です。
	double sqrt(double _sqrtval_, double _threshold_) SLIB_CVR_NOEXCEPT
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
	double sqrt_Quick(double _srtval_) SLIB_CVR_NOEXCEPT
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
	float sqrtF_Quick(float _srtval_) SLIB_CVR_NOEXCEPT
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
	double rsqrt_Quick(double _srtval_) SLIB_CVR_NOEXCEPT
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
	float rsqrtF_Quick(float _srtval_) SLIB_CVR_NOEXCEPT
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
};


