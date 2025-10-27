/*
 * SonikGraphicsHelperMath.cpp
 *
 *  Created on: 2015/08/28
 *      Author: SONIK
 */

#include "SonikKeyFrameMath.h"

#include <math.h>
#include "SonikMathDistance.h"
#include "SonikMathTrigonometric.h"

namespace SonikMath
{

	//StartPointから、Endポイントまでの線形補間を行います。
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 秒間処理フレームレート値を指定します。
	//引数4: 合計処理時間(ミリ秒)を指定します。
	//引数5: 取得する値の時間(ミリ秒)を指定します。
	//戻り値: 引数5 に指定した時間の時の線形補間値。
	//補足: 引数4 < 引数5 の条件が成り立つ場合、最終時間の値を取得します。
	double KeyFrameLinerInterPolation(double StartPoint, double EndPoint, uint32_t ProcessFramerate, uint64_t MilliSec_PTime, uint64_t MilliSec_GetTime)
	{
		double ret;
		double Ptime = 0;
		double GetTime = 0;

		if( MilliSec_PTime == 0)
		{
			return StartPoint;
		};

		if( MilliSec_GetTime == 0)
		{
			return StartPoint;
		};

		if( MilliSec_PTime < MilliSec_GetTime )
		{
			return EndPoint;
		};

		if( ProcessFramerate == 0 )
		{
			return StartPoint;
		};

		Ptime = ProcessFramerate * (static_cast<double>(MilliSec_PTime) / 1000.0);
		GetTime = ProcessFramerate * (static_cast<double>(MilliSec_GetTime) / 1000.0);

		//ProcessFrame = 現在の処理フレーム
		//ProcessTimeFrame  = 総処理フレーム
		// P = S + ProcessFrame * ( E - S ) / ProcessTimeFrame
		// ○ ------○--------●
		// S		P		  E

		ret = StartPoint + GetTime * ( EndPoint - StartPoint ) / Ptime;

		return ret;

	};


	//StartPointから、Endポイントまでの線形補間を行います。(Floatバージョン)
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 秒間処理フレームレート値を指定します。
	//引数4: 合計処理時間(ミリ秒)を指定します。
	//引数5: 取得する値の時間(ミリ秒)を指定します。
	//戻り値: 引数5 に指定した時間の時の線形補間値。
	//補足: 引数4 < 引数5 の条件が成り立つ場合、最終時間の値を取得します。
	float KeyFrameLinerInterPolation_F(float StartPoint, float EndPoint, uint32_t ProcessFramerate, uint64_t MilliSec_PTime, uint64_t MilliSec_GetTime)
	{
		float ret;
		float Ptime = 0;
		float GetTime = 0;

		if( MilliSec_PTime == 0)
		{
			return StartPoint;
		};

		if( MilliSec_GetTime == 0)
		{
			return StartPoint;
		};

		if( MilliSec_PTime < MilliSec_GetTime )
		{
			return EndPoint;
		};

		if( ProcessFramerate == 0 )
		{
			return StartPoint;
		};

		Ptime = ProcessFramerate * (static_cast<float>(MilliSec_PTime) / 1000.0f);
		GetTime = ProcessFramerate * (static_cast<float>(MilliSec_GetTime) / 1000.0f);

		//ProcessFrame = 現在の処理フレーム
		//ProcessTimeFrame  = 総処理フレーム
		// P = S + ProcessFrame * ( E - S ) / ProcessTimeFrame
		// ○ ------○--------●
		// S		P		  E

		ret = StartPoint + GetTime * ( EndPoint - StartPoint ) / Ptime;

		return ret;
	};

	//フレーム指定で、StartPointから、Endポイントまでの線形補間を行います。
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 合計処理フレーム数を指定します。
	//引数4: 取得するフレーム値を指定します。
	//戻り値: 引数4 に指定したフレームの時の線形補間値。
	//補足: 引数3 < 引数4 の条件が成り立つ場合、最終時間の値を取得します。
	double KeyFrameLinerInterPolationFrame(double StartPoint, double EndPoint, uint32_t SetProcFrame, uint64_t GetFrame)
	{
		double ret;

		if( SetProcFrame == 0)
		{
			return StartPoint;
		};

		if( GetFrame == 0)
		{
			return StartPoint;
		};

		if( SetProcFrame < GetFrame )
		{
			return EndPoint;
		};

		//ProcessFrame = 現在の処理フレーム
		//ProcessTimeFrame  = 総処理フレーム
		// P = S + ProcessFrame * ( E - S ) / ProcessTimeFrame
		// ○ ------○--------●
		// S		P		  E

		ret = StartPoint + GetFrame * ( EndPoint - StartPoint ) / SetProcFrame;

		return ret;
	};

	//フレーム指定で、StartPointから、Endポイントまでの線形補間を行います。(Floatバージョン)
	//引数1: 始点となる値を指定します。
	//引数2: 終点となる値を指定します。
	//引数3: 合計処理フレーム数を指定します。
	//引数4: 取得するフレーム値を指定します。
	//戻り値: 引数4 に指定したフレームの時の線形補間値。
	//補足: 引数3 < 引数4 の条件が成り立つ場合、最終時間の値を取得します。
	float KeyFrameLinerInterPolationFrame_F(float StartPoint, float EndPoint, uint32_t SetProcFrame, uint64_t GetFrame)
	{
		float ret;

		if( SetProcFrame == 0)
		{
			return StartPoint;
		};

		if( GetFrame == 0)
		{
			return StartPoint;
		};

		if( SetProcFrame < GetFrame )
		{
			return EndPoint;
		};

		//ProcessFrame = 現在の処理フレーム
		//ProcessTimeFrame  = 総処理フレーム
		// P = S + ProcessFrame * ( E - S ) / ProcessTimeFrame
		// ○ ------○--------●
		// S		P		  E

		ret = StartPoint + GetFrame * ( EndPoint - StartPoint ) / SetProcFrame;

		return ret;
	}

	//指定角度時の位置を取得します。
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 回転対象のX座標を指定します。
	//引数4: 回転対称のY座標を指定します。
	//引数5: 取得する座標の回転度数を指定します。
	//引数6: 引数5で指定した度数分回転させた後のX座標が入ります。
	//引数7: 引数5で指定した度数分回転させた後のY座標が入ります。
	//補足: sqrtを使用して現在の位置から中心点までの半径を計算し、回転を行います。
	void SinglePointRotationSinglePointRotation(double CriteriaX, double CriteriaY, double ObjectPosX, double ObjectPosY, double digree, double& GetX, double& GetY)
	{
		SonikMathDataBox::Sonik3DPoint f_point(ObjectPosX);
		SonikMathDataBox::Sonik3DPoint s_point(CriteriaX);
		//底辺
		double distant_Base = Distance(f_point, s_point);//SonikMath::TwoPointDistance(ObjectPosX, CriteriaX);
		//高さ
		f_point.SetXY(0.0, ObjectPosY);
		s_point.SetXY(0.0, CriteriaY);
		double distant_Top = Distance(f_point, s_point);//SonikMath::TwoPointDistance(ObjectPosY, CriteriaY);
		//半径
		double Radius = sqrt( ((distant_Base * distant_Base) + (distant_Top * distant_Top)) );

		GetX = Radius * SonikMath::Cos(digree);
		GetY = Radius * SonikMath::Sin(digree);

	};

	//指定角度時の位置を取得します。(float版)
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 回転対象のX座標を指定します。
	//引数4: 回転対称のY座標を指定します。
	//引数5: 取得する座標の回転度数を指定します。
	//引数6: 引数5で指定した度数分回転させた後のX座標が入ります。
	//引数7: 引数5で指定した度数分回転させた後のY座標が入ります。
	//補足: sqrtを使用して現在の位置から中心点までの半径を計算し、回転を行います。
	void SinglePointRotation_F(float CriteriaX, float CriteriaY, float ObjectPosX, float ObjectPosY, float digree, float& GetX, float& GetY)
	{
		SonikMathDataBox::Sonik3DPoint f_point(ObjectPosX);
		SonikMathDataBox::Sonik3DPoint s_point(CriteriaX);
		//底辺
		float distant_Base = static_cast<float>(Distance(f_point, s_point));//SonikMath::TwoPointDistance(ObjectPosX, CriteriaX);
		//高さ
		f_point.SetXY(0.0, ObjectPosY);
		s_point.SetXY(0.0, CriteriaY);
		float distant_Top = static_cast<float>(Distance(f_point, s_point));//SonikMath::TwoPointDistance(ObjectPosY, CriteriaY);
		//半径
		float Radius = sqrtf( ((distant_Base * distant_Base) + (distant_Top * distant_Top)) );

		GetX = Radius * SonikMath::CosF(digree);
		GetY = Radius * SonikMath::SinF(digree);
	};

	//半径を利用して指定角度時の位置を取得します。
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 中心点からの半径を指定します。
	//引数4: 取得する座標の回転度数を指定します。
	//引数5: 引数4で指定した度数分回転させた後のX座標が入ります。
	//引数6: 引数4で指定した度数分回転させた後のY座標が入ります。
	//補足: 取得できる座標は、中心点からの半径円座標であり、現在座標から中心点を回転した座標ではありません。
	void SinglePointRadiusRotation(double CriteriaX, double CriteriaY, double Radius, double digree, double& GetX, double& GetY)
	{
		GetX = CriteriaX + ( Radius * SonikMath::Cos(digree) );
		GetY = CriteriaY + ( Radius * SonikMath::Sin(digree) );

	};

	//半径を利用して指定角度時の位置を取得します。(float版)
	//引数1: 中心点のX座標を指定します。
	//引数2: 中心点のY座標を指定します。
	//引数3: 中心点からの半径を指定します。
	//引数4: 取得する座標の回転度数を指定します。
	//引数5: 引数4で指定した度数分回転させた後のX座標が入ります。
	//引数6: 引数4で指定した度数分回転させた後のY座標が入ります。
	//補足: 取得できる座標は、中心点からの半径円座標であり、現在座標から中心点を回転した座標ではありません。
	void SinglePointRadiusRotation_F(float CriteriaX, float CriteriaY, float Radius, float digree, float& GetX, float& GetY)
	{
		GetX = CriteriaX + ( Radius * SonikMath::CosF(digree) );
		GetY = CriteriaY + ( Radius * SonikMath::SinF(digree) );

	};


};
