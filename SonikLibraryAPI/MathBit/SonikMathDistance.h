/*
 * SonikMathDistance.h
 *
 *  Created on: 2015/08/17
 *      Author: SONIK
 */

#ifndef SONIKMATHDISTANCE_H_
#define SONIKMATHDISTANCE_H_

#include "../SmartPointer/SonikSmartPointer.hpp"

//前方宣言====================================
namespace SonikMathDataBox
{
	class Sonik3DPoint;
};

namespace DataBoxUsedValueContainer_SonikMath
{
	//ただの構造体。
	//クラスとすることでshared_ptrに突っ込めたりするので...。
	class Double3Value;
};

namespace SonikMath
{
//指定した3DPointのベクトルの長さを計算します。
double VectorLength(SonikMathDataBox::Sonik3DPoint& _point_) noexcept;
//二点間の差を表すベクトルを取得します。
void diffVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_, SonikMathDataBox::Sonik3DPoint& _outpoint_) noexcept;
//二点間のドット積を計算します。
double DotProductVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_) noexcept;

//引数1のポイントをベースに引数2の座標との距離を算出します。
double Distance(SonikMathDataBox::Sonik3DPoint& _f_point_, SonikMathDataBox::Sonik3DPoint& _s_point_) noexcept;
double Distance(double _fx_ = 0.0, double _fy_ = 0.0, double _fz_ = 0.0, double _sx_ = 0.0, double _sy_ = 0.0, double _sz_ = 0.0) noexcept;

//位置を表す3DPointと方向を表す3DPointからパンニングを計算します。
//0.0~1.0の範囲で返却します。3DAudioにおいて、ステレオチャンネルなどでL側への計算に使う場合は(1.0 - 返却値)を掛けるようにしてください。
//3DPointのみの指定の場合は内部で全て計算するため、「使い回せる値もすべて計算しなおします。」でも指定が楽です。
//diff, veclenを指定する場合は、それぞれ、diffVec, VectorLengthで事前計算した値を指定することを想定しています。
//LisVec は リスナの方向ベクトルをVectorLengthで計算した結果を指定します。
//PlyVecは リスナーの位置ベクトルと、音源(Player）の位置ベクトルをdiffVecで計算した結果の値をVectorLengthで計算した結果を指定します。
double Panning(SonikMathDataBox::Sonik3DPoint& _lispos_, SonikMathDataBox::Sonik3DPoint& _plypos_, SonikMathDataBox::Sonik3DPoint& _lisdir_, SonikMathDataBox::Sonik3DPoint& _plydir_, double _atten_max_ = 1.0, double _attenuate_ = 0.3) noexcept;
double Panning(double _dotproduct_, double _lisdirveclen_, double _diffveclen_, double _dotproductdir_, double _plydirveclen_, double _atten_max_ = 1.0, double _attenuate_ = 0.3) noexcept;

};


namespace SonikMathDataBox
{
	class Sonik3DPoint
	{
		//friend 指定
		friend double SonikMath::VectorLength(SonikMathDataBox::Sonik3DPoint& _point_) noexcept;
		friend void SonikMath::diffVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_, SonikMathDataBox::Sonik3DPoint& _outpoint_) noexcept;
		friend double SonikMath::DotProductVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_) noexcept;
		friend double SonikMath::Distance(SonikMathDataBox::Sonik3DPoint& _f_point_, SonikMathDataBox::Sonik3DPoint& _s_point_) noexcept;
		friend double SonikMath::Distance(double _fx_, double _fy_, double _fz_, double _sx_, double _sy_, double _sz_) noexcept;
		friend double SonikMath::Panning(SonikMathDataBox::Sonik3DPoint& _lispos_, SonikMathDataBox::Sonik3DPoint& _plypos_, SonikMathDataBox::Sonik3DPoint& _lisdir_, SonikMathDataBox::Sonik3DPoint& _plydir_, double _atten_max_, double _attenuate_) noexcept;
		friend double SonikMath::Panning(double _dotproduct_, double _lisdirveclen_, double _diffveclen_, double _dotproductdir_, double _plydirveclen_, double _atten_max_, double _attenuate_) noexcept;

	private:
		SonikLib::SharedSmtPtr<double> x;
		SonikLib::SharedSmtPtr<double> y;
		SonikLib::SharedSmtPtr<double> z;

	public:
			Sonik3DPoint(double _x_ = 0.0, double _y_ = 0.0, double _z_ =0.0);
			virtual ~Sonik3DPoint(void);

			void Set3Point(double _x_, double _y_, double _z_);
			void SetXY(double _x_, double _y_);
			void SetXZ(double _x_, double _z_);
			void SetYZ(double _y_, double _z_);
			void SetX(double _x_);
			void SetY(double _y_);
			void SetZ(double _z_);

			void Get3Point(double& _x_, double& _y_, double& _z_);
			void Get3Point(double* _x_, double* _y_, double* _z_);
			void GetXY(double& _x_, double& _y_);
			void GetXY(double* _x_, double* _y_);
			void GetXZ(double& _x_, double& _z_);
			void GetXZ(double* _x_, double* _z_);
			void GetYZ(double& _y_, double& _z_);
			void GetYZ(double* _y_, double* _z_);
			double GetX(void);
			double GetY(void);
			double GetZ(void);

			//UnLinkPointを呼ぶまで、LinkPointをコールしたオブジェクトと同期します。
			void LinkPointAll(Sonik3DPoint& _linked_);
			void LinkPointAll(Sonik3DPoint* _linked_);
			void LinkPointAll(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointXY(Sonik3DPoint& _linked_);
			void LinkPointXY(Sonik3DPoint* _linked_);
			void LinkPointXY(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointXZ(Sonik3DPoint& _linked_);
			void LinkPointXZ(Sonik3DPoint* _linked_);
			void LinkPointXZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointYZ(Sonik3DPoint& _linked_);
			void LinkPointYZ(Sonik3DPoint* _linked_);
			void LinkPointYZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointX(Sonik3DPoint& _linked_);
			void LinkPointX(Sonik3DPoint* _linked_);
			void LinkPointX(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointY(Sonik3DPoint& _linked_);
			void LinkPointY(Sonik3DPoint* _linked_);
			void LinkPointY(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);
			void LinkPointZ(Sonik3DPoint& _linked_);
			void LinkPointZ(Sonik3DPoint* _linked_);
			void LinkPointZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_);

			//LinkPoint状態を解除します。
			//内部で新しい座標をnewするので、そこに失敗したらfalseが帰ります。
			//※戻り値はnewの成功/失敗を表すもので、UnLinkedの成功/失敗を表すものではありません。
			//※基本的にShared_ptrをReleaseするだけなので成功/失敗の概念はほぼありません。
			bool UnLinkPointAll(void);
			bool UnLinkPointXY(void);
			bool UnLinkPointXZ(void);
			bool UnLinkPointYZ(void);
			bool UnLinkPointX(void);
			bool UnLinkPointY(void);
			bool UnLinkPointZ(void);

	};

	class SonikBazierBox
	{
		protected:
			double StartX, StartY; //始点
			double EndX, EndY;	  //終点
			double PivotX, PivotY; //制御点
			unsigned long Elapsed_Frame;  //始点から終点まで、何フレームで到達するか。

			double* m_saved_result_X;
			double* m_saved_result_Y;
		public:

			//引数最後のフラグは、先に計算を実施し、すべてのフレームの結果を保持しておくかどうかを設定します。(フレーム数 * doubleサイズ)分のメモリを使います。
			SonikBazierBox(double _x_ = 0.0, double _y_ = 0.0, double _endx_ = 0.0, double _endy_ = 0.0, double _pivx_ = 0.0, double  _pivy_ = 0.0, unsigned long _ef_= 0);
			~SonikBazierBox(void);

			//2次元のペジェ曲線に必要な情報を設定します。
			//引数最後のフラグは、先に計算を実施し、すべてのフレームの結果を保持しておくかどうかを設定します。(フレーム数 * doubleサイズ)分のメモリを使います。
			void SetValue(double _x_, double _y_, double _endx_, double _endy_, double _pivx_, double  _pivy_, unsigned long _ef_);

			//セットされた値からすべてのフレーム結果を算出し、保持します。
			bool SavedResultFrame(void);
			//保持しているフレーム結果をすべて破棄します。
			void SavedResultClear(void);

			//2次のベジェ曲線の位置点を算出します。
			//引数1: 結果であるX値を格納するために変数を指定します。
			//引数2: 結果であるY値を格納するために変数を指定します。
			//引数3: 始点から終点までの総合フレームの中で、何フレーム目の値を取得するのかを指定します。
			void Bezier_curveLine(double& GetX, double& GetY, unsigned long GetFrame);
	};

};

namespace DataBoxUsedValueContainer_SonikMath
{
//ただの構造体。
//クラスとすることでshared_ptrに突っ込めたりするので...。
class Double3Value
{
public:
	double x, y, z;
};

};

#endif /* SONIKMATHDISTANCE_H_ */
