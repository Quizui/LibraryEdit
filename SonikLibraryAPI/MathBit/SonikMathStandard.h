/*
 * SonikMathStandard.h
 *
 *  Created on: 2019/06/20
 *      Author: SONIC
 */

#ifndef MATHBIT_SONIKMATHSTANDARD_H_
#define MATHBIT_SONIKMATHSTANDARD_H_

#include <stdint.h>

//未分類や使用範囲が広い計算類を実装します。
namespace SonikMath
{
	struct SonikComplex
	{
		uint32_t r; //実部、実数
		uint32_t i; //虚部、虚数
	};

	//2を底とする対数を計算します。
	int log2(int x);
	//指定した値の2の累乗(るいじょう)数を計算します。
	int pow2(int x);

	//小数点以下の切り捨てを行います。
	double floor(double _val_);

	//余剰の計算をします。
	double mod(double _num_, double _denom_);
	float mod(float _num_, float _denom);
	int64_t mod(int64_t _num_, int64_t _denom);
	uint64_t mod(uint64_t _num_, uint64_t _denom);

	//絶対値を計算します。
	int64_t abs(int64_t _abs_) noexcept;
	double abs(double _abs_) noexcept;

	//負の数の対応した平方根を計算します。
	//第2引数：精度を指定することが可能です。
	double sqrt(double _sqrtval_, double _threshold_ = 1e-6) noexcept;

	//精度を犠牲にして高速に平方根を計算します。
	double sqrt_Quick(double _srtval_) noexcept;
	float sqrtF_Quick(float _srtval_) noexcept;
	//精度を犠牲にして高速に逆平方根を計算します。
	double rsqrt_Quick(double _srtval_) noexcept;
	float rsqrtF_Quick(float _srtval_) noexcept;

	//FFT(高速フーリエ変換)を行います。
	//void FFT_1d(SonikComplex* p_complex, int TargetSampling);
	//IFFT(逆高速フーリエ変換を行います。
	//void IFFT_1d(SonikComplex* p_complex, int TargetSampling);


};



#endif /* MATHBIT_SONIKMATHSTANDARD_H_ */
