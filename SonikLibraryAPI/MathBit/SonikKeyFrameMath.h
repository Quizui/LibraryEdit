/*
 * SonikGraphicsHelperMath.h
 *
 *  Created on: 2015/08/28
 *      Author: SONIK
 */

#ifndef SONIKGRAPHICSHELPERMATH_H_
#define SONIKGRAPHICSHELPERMATH_H_

#include <cstdint>

namespace SonikMath
{
	//時間指定で、StartPointから、Endポイントまでの線形補間を行います。
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 秒間処理フレームレート値を指定します。
	//引数4: 合計処理時間(ミリ秒)を指定します。
	//引数5: 取得する値の時間(ミリ秒)を指定します。
	//戻り値: 引数5 に指定した時間の時の線形補間値。
	//補足: 引数4 < 引数5 の条件が成り立つ場合、最終時間の値を取得します。
	double KeyFrameLinerInterPolation(double StartPoint, double EndPoint, uint32_t ProcessFramerate, uint64_t MilliSec_PTime, uint64_t MilliSec_GetTime);

	//時間指定で、StartPointから、Endポイントまでの線形補間を行います。(Floatバージョン)
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 秒間処理フレームレート値を指定します。
	//引数4: 合計処理時間(ミリ秒)を指定します。
	//引数5: 取得する値の時間(ミリ秒)を指定します。
	//戻り値: 引数5 に指定した時間の時の線形補間値。
	//補足: 引数4 < 引数5 の条件が成り立つ場合、最終時間の値を取得します。
	float KeyFrameLinerInterPolation_F(float StartPoint, float EndPoint, uint32_t ProcessFramerate, uint64_t MilliSec_PTime, uint64_t MilliSec_GetTime);

	//フレーム指定で、StartPointから、Endポイントまでの線形補間を行います。
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 合計処理フレーム数を指定します。
	//引数4: 取得するフレーム値を指定します。
	//戻り値: 引数4 に指定したフレームの時の線形補間値。
	//補足: 引数3 < 引数4 の条件が成り立つ場合、最終時間の値を取得します。
	double KeyFrameLinerInterPolationFrame(double StartPoint, double EndPoint, uint32_t SetProcFrame, uint64_t GetFrame);

	//フレーム指定で、StartPointから、Endポイントまでの線形補間を行います。(Floatバージョン)
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 合計処理フレーム数を指定します。
	//引数4: 取得するフレーム値を指定します。
	//戻り値: 引数4 に指定したフレームの時の線形補間値。
	//補足: 引数3 < 引数4 の条件が成り立つ場合、最終時間の値を取得します。
	float KeyFrameLinerInterPolationFrame_F(float StartPoint, float EndPoint, uint32_t SetProcFrame, uint64_t GetFrame);

	//指定角度時の位置を取得します。
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 回転対象のX座標を指定します。
	//引数4: 回転対称のY座標を指定します。
	//引数5: 取得する座標の回転度数を指定します。
	//引数6: 引数5で指定した度数分回転させた後のX座標が入ります。
	//引数7: 引数5で指定した度数分回転させた後のY座標が入ります。
	//補足: sqrtを使用して現在の位置から中心点までの半径を計算し、回転を行います。
	void SinglePointRotation(double CriteriaX, double CriteriaY, double ObjectPosX, double ObjectPosY, double digree, double& GetX, double& GetY);

	//指定角度時の位置を取得します。(float版)
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 回転対象のX座標を指定します。
	//引数4: 回転対称のY座標を指定します。
	//引数5: 取得する座標の回転度数を指定します。
	//引数6: 引数5で指定した度数分回転させた後のX座標が入ります。
	//引数7: 引数5で指定した度数分回転させた後のY座標が入ります。
	//補足: sqrtを使用して現在の位置から中心点までの半径を計算し、回転を行います。
	void SinglePointRotation_F(float CriteriaX, float CriteriaY, float ObjectPosX, float ObjectPosY, float digree, float& GetX, float& GetY);

	//半径を利用して指定角度時の位置を取得します。
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 中心点からの半径を指定します。
	//引数4: 取得する座標の回転度数を指定します。
	//引数5: 引数4で指定した度数分回転させた後のX座標が入ります。
	//引数6: 引数4で指定した度数分回転させた後のY座標が入ります。
	//補足: 取得できる座標は、中心点からの半径円座標であり、現在座標から中心点を回転した座標ではありません。
	void SinglePointRadiusRotation(double CriteriaX, double CriteriaY, double Radius, double digree, double& GetX, double& GetY);

	//半径を利用して指定角度時の位置を取得します。(float版)
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 中心点からの半径を指定します。
	//引数4: 取得する座標の回転度数を指定します。
	//引数5: 引数4で指定した度数分回転させた後のX座標が入ります。
	//引数6: 引数4で指定した度数分回転させた後のY座標が入ります。
	//補足: 取得できる座標は、中心点からの半径円座標であり、現在座標から中心点を回転した座標ではありません。
	void SinglePointRadiusRotation_F(float CriteriaX, float CriteriaY, float Radius, float digree, float& GetX, float& GetY);



};


#endif /* SONIKGRAPHICSHELPERMATH_H_ */
