/*
 * SonikMatTrigonometric.h
 *
 *  Created on: 2015/06/02
 *      Author: SONIK
 */

#ifndef SONIKMATHTRIGONOMETRIC_H_
#define SONIKMATHTRIGONOMETRIC_H_

namespace SonikMath
{
		//Sin値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		double Sin(double digree);

		//Sin値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		//Float版
		float SinF(float digree);

		//Cos値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		double Cos(double digree);

		//Cos値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		//Float版
		float CosF(float digree);

		//Tan値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		double Tan(double digree);

		//Tan値を取得します。
		//引数1: 角度単位で取得したい角度を指定します。(ラジアン角ではありません)
		//Float版
		float TanF(float digree);

		//角度値をラジアン角値に変換します。
		//引数1: ラジアン角値に変換したい角度値を指定します。
		double digree_To_radian(double digree);

		//角度値をラジアン角値に変換します。
		//引数1: ラジアン角値に変換したい角度値を指定します。
		float digree_To_radianF(float digree);

		//ラジアン角値を角度値に変換します。
		//引数1: 角度値に変換したいラジアン値を指定します。
		double radian_To_digree(double radian);

		//ラジアン角値を角度値に変換します。
		//引数1: 角度値に変換したいラジアン値を指定します。
		float radian_To_digree(float radian);


#ifdef _DEBUG

		//Sin値及びCos値のリストをtxtに出力します。
		//_DEBUGが定義されている時のみ有効です。
		void SinCosValueOutPut(void);

#endif


};



#endif /* SONIKMATHTRIGONOMETRIC_H_ */
