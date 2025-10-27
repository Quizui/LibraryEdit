/*
 * SonikMathDistance.cpp
 *
 *  Created on: 2015/08/17
 *      Author: SONIK
 */

#include "SonikMathDistance.h"
#include "SonikMathStandard.h"

#include <stdint.h>
#include <new>

namespace SonikMathDataBox
{
	//=====================================
	//					Sonik3DPoint Implement
	//=====================================
	Sonik3DPoint::Sonik3DPoint(double _x_, double _y_, double _z_)
	{
		try
		{
			if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(new double, x))
			{
				throw std::bad_alloc();
			};
			if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(new double, y))
			{
				throw std::bad_alloc();
			};
			if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(new double, z))
			{
				throw std::bad_alloc();
			};

			(*x) = _x_;
			(*y) = _y_;
			(*z) = _z_;
		}catch(std::bad_alloc&)
		{
			x = SonikLib::SharedSmtPtr<double>();
			y = SonikLib::SharedSmtPtr<double>();
			z = SonikLib::SharedSmtPtr<double>();
			throw;
		};

	};

	Sonik3DPoint::~Sonik3DPoint(void)
	{
		//no process;
	};

	void Sonik3DPoint::Set3Point(double _x_, double _y_, double _z_)
	{
		(*x) = _x_;
		(*y) = _y_;
		(*z) = _z_;
	};

	void Sonik3DPoint::SetXY(double _x_, double _y_)
	{
		(*x) = _x_;
		(*y) = _y_;
	};
	void Sonik3DPoint::SetXZ(double _x_, double _z_)
	{
		(*x) = _x_;
		(*z) = _z_;
	};
	void Sonik3DPoint::SetYZ(double _y_, double _z_)
	{
		(*y) = _y_;
		(*z) = _z_;
	};

	void Sonik3DPoint::SetX(double _x_)
	{
		(*x) = _x_;
	};
	void Sonik3DPoint::SetY(double _y_)
	{
		(*y) = _y_;
	};
	void Sonik3DPoint::SetZ(double _z_)
	{
		(*z) = _z_;
	};

	void Sonik3DPoint::Get3Point(double& _x_, double& _y_, double& _z_)
	{
		_x_ = (*x);
		_y_ = (*y);
		_z_ = (*z);
	};

	void Sonik3DPoint::Get3Point(double* _x_, double* _y_, double* _z_)
	{
		if( _x_ == nullptr || _y_ == nullptr || _z_ == nullptr )
		{
			return;
		};

		(*_x_) = (*x);
		(*_y_) = (*y);
		(*_z_) = (*z);
	};

	void Sonik3DPoint::GetXY(double& _x_, double& _y_)
	{
		_x_ = (*x);
		_y_ = (*y);
	};

	void Sonik3DPoint::GetXY(double* _x_, double* _y_)
	{
		if( _x_ == nullptr || _y_ == nullptr)
		{
			return;
		};

		(*_x_) = (*x);
		(*_y_) = (*y);
	};

	void Sonik3DPoint::GetXZ(double& _x_, double& _z_)
	{
		_x_ = (*x);
		_z_ = (*z);
	};

	void Sonik3DPoint::GetXZ(double* _x_, double* _z_)
	{
		if( _x_ == nullptr || _z_ == nullptr )
		{
			return;
		};

		(*_x_) = (*x);
		(*_z_) = (*z);
	};

	void Sonik3DPoint::GetYZ(double& _y_, double& _z_)
	{
		_y_ = (*y);
		_z_ = (*z);
	};
	void Sonik3DPoint::GetYZ(double* _y_, double* _z_)
	{
		if( _y_ == nullptr || _z_ == nullptr )
		{
			return;
		};

		(*_y_) = (*y);
		(*_z_) = (*z);
	};

	double Sonik3DPoint::GetX(void)
	{
		return (*x);
	};

	double Sonik3DPoint::GetY(void)
	{
		return (*y);
	};

	double Sonik3DPoint::GetZ(void)
	{
		return (*z);
	};

	void Sonik3DPoint::LinkPointAll(Sonik3DPoint& _linked_)
	{
		_linked_.x = x;
		_linked_.y = y;
		_linked_.z = z;
	};

	void Sonik3DPoint::LinkPointAll(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->x = x;
		_linked_->y = y;
		_linked_->z = z;
	};

	void Sonik3DPoint::LinkPointAll(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->x = x;
		_linked_->y = y;
		_linked_->z = z;
	};

	void Sonik3DPoint::LinkPointXY(Sonik3DPoint& _linked_)
	{
		_linked_.x = x;
		_linked_.y = y;
	};
	void Sonik3DPoint::LinkPointXY(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->x = x;
		_linked_->y = y;
	};
	void Sonik3DPoint::LinkPointXY(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->x = x;
		_linked_->y = y;
	};
	void Sonik3DPoint::LinkPointXZ(Sonik3DPoint& _linked_)
	{
		_linked_.x = x;
		_linked_.z = z;
	};
	void Sonik3DPoint::LinkPointXZ(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->x = x;
		_linked_->z = z;
	};
	void Sonik3DPoint::LinkPointXZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->x = x;
		_linked_->z = z;
	};
	void Sonik3DPoint::LinkPointYZ(Sonik3DPoint& _linked_)
	{
		_linked_.y = y;
		_linked_.z = z;
	};
	void Sonik3DPoint::LinkPointYZ(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->y = y;
		_linked_->z = z;
	};
	void Sonik3DPoint::LinkPointYZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->y = y;
		_linked_->z = z;
	};
	void Sonik3DPoint::LinkPointX(Sonik3DPoint& _linked_)
	{
		_linked_.x = x;
	};
	void Sonik3DPoint::LinkPointX(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->x = x;
	};
	void Sonik3DPoint::LinkPointX(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->x = x;
	};
	void Sonik3DPoint::LinkPointY(Sonik3DPoint& _linked_)
	{
		_linked_.y = y;
	};
	void Sonik3DPoint::LinkPointY(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->y = y;
	};
	void Sonik3DPoint::LinkPointY(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->y = y;
	};
	void Sonik3DPoint::LinkPointZ(Sonik3DPoint& _linked_)
	{
		_linked_.z = z;
	};
	void Sonik3DPoint::LinkPointZ(Sonik3DPoint* _linked_)
	{
		if(_linked_ == nullptr)
		{
			return;
		};

		_linked_->z = z;
	};
	void Sonik3DPoint::LinkPointZ(SonikLib::SharedSmtPtr<Sonik3DPoint>& _linked_)
	{
		if(_linked_.IsNullptr())
		{
			return;
		};

		_linked_->z = z;
	};

	//LinkPoint状態を解除します。
	//内部で新しい座標をnewするので、そこに失敗したらfalseが帰ります。
	//※戻り値はnewの成功/失敗を表すもので、UnLinkedの成功/失敗を表すものではありません。
	//※基本的にShared_ptrをReleaseするだけなので成功/失敗の概念はほぼありません。
	bool Sonik3DPoint::UnLinkPointAll(void)
	{
		double* l_newvec[3] = {nullptr, nullptr, nullptr};
		if( x.GetRefCount() != 1 )
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[0] = new(std::nothrow) double;
			if(l_newvec[0] == nullptr)
			{
				return false;
			};

			(*l_newvec[0]) = 0.0;
		};

		if(y.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[1] = new(std::nothrow) double;
			if(l_newvec[1] == nullptr)
			{
				delete l_newvec[0];
				return false;
			};

			(*l_newvec[1]) = 0.0;
		};

		if(z.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[2] = new(std::nothrow) double;
			if(l_newvec[2] == nullptr)
			{
				delete l_newvec[0];
				delete l_newvec[1];
				return false;
			};

			(*l_newvec[2]) = 0.0;
		};

		//値バックアップ
		(*l_newvec[0]) = (*(this->x));
		(*l_newvec[1]) = (*(this->y));
		(*l_newvec[2]) = (*(this->z));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[0], x))
		{
			delete l_newvec[0];
			delete l_newvec[1];
			delete l_newvec[2];
			return false;
		};

		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[1], y) )
		{
			x = SonikLib::SharedSmtPtr<double>();
			delete l_newvec[1];
			delete l_newvec[2];

			return false;
		};

		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[2], z) )
		{
			x = SonikLib::SharedSmtPtr<double>();
			y = SonikLib::SharedSmtPtr<double>();
			delete l_newvec[2];

			return false;
		};

		return true;
	};

	bool Sonik3DPoint::UnLinkPointXY(void)
	{
		double* l_newvec[2] = {nullptr, nullptr};
		if( x.GetRefCount() != 1 )
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[0] = new(std::nothrow) double;
			if(l_newvec[0] == nullptr)
			{
				return false;
			};

			(*l_newvec[0]) = 0.0;
		};

		if(y.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[1] = new(std::nothrow) double;
			if(l_newvec[1] == nullptr)
			{
				delete l_newvec[0];
				return false;
			};

			(*l_newvec[1]) = 0.0;
		};

		//値バックアップ
		(*l_newvec[0]) = (*(this->x));
		(*l_newvec[1]) = (*(this->y));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[0], x))
		{
			delete l_newvec[0];
			delete l_newvec[1];
			return false;
		};

		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[1], y) )
		{
			x =SonikLib::SharedSmtPtr<double>();
			delete l_newvec[1];
			return false;
		};

		return true;
	};
	bool Sonik3DPoint::UnLinkPointXZ(void)
	{
		double* l_newvec[2] = {nullptr, nullptr};
		if( x.GetRefCount() != 1 )
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[0] = new(std::nothrow) double;
			if(l_newvec[0] == nullptr)
			{
				return false;
			};

			(*l_newvec[0]) = 0.0;
		};

		if(z.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[1] = new(std::nothrow) double;
			if(l_newvec[1] == nullptr)
			{
				delete l_newvec[0];
				delete l_newvec[1];
				return false;
			};

			(*l_newvec[1]) = 0.0;
		};

		//値バックアップ
		(*l_newvec[0]) = (*(this->x));
		(*l_newvec[1]) = (*(this->z));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[0], x))
		{
			delete l_newvec[0];
			delete l_newvec[1];
			return false;
		};

		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[1], z) )
		{
			x = SonikLib::SharedSmtPtr<double>();
			return false;
		};

		return true;
	};
	bool Sonik3DPoint::UnLinkPointYZ(void)
	{
		double* l_newvec[2] = {nullptr, nullptr};
		if(y.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[0] = new(std::nothrow) double;
			if(l_newvec[0] == nullptr)
			{
				delete l_newvec[0];
				return false;
			};

			(*l_newvec[0]) = 0.0;
		};

		if(z.GetRefCount() != 1)
		{
			//カウント1以外なら自身用のを作成。
			l_newvec[1] = new(std::nothrow) double;
			if(l_newvec[1] == nullptr)
			{
				delete l_newvec[0];
				delete l_newvec[1];
				return false;
			};

			(*l_newvec[1]) = 0.0;
		};

		//値バックアップ
		(*l_newvec[0]) = (*(this->y));
		(*l_newvec[1]) = (*(this->z));

		//現在の値を使用した自分の座標を新しく持つ。
		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[0], y) )
		{
			delete l_newvec[0];
			delete l_newvec[1];

			return false;
		};

		if( !SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec[1], z) )
		{
			y = SonikLib::SharedSmtPtr<double>();
			delete l_newvec[1];

			return false;
		};

		return true;
	};
	bool Sonik3DPoint::UnLinkPointX(void)
	{
		if( x.GetRefCount() == 1 )
		{
			//カウント1なら自身のものにしちゃう。
		};

		double* l_newvec = new(std::nothrow) double;
		if(l_newvec == nullptr)
		{
			return false;
		};

		//値バックアップ
		(*l_newvec) = (*(this->x));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec, x))
		{
			delete l_newvec;
			return false;
		};

		return true;
	};
	bool Sonik3DPoint::UnLinkPointY(void)
	{
		if( y.GetRefCount() == 1 )
		{
			//カウント1なら自身のものにしちゃう。
		};

		double* l_newvec = new(std::nothrow) double;
		if(l_newvec == nullptr)
		{
			return false;
		};

		//値バックアップ
		(*l_newvec) = (*(this->y));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec, y))
		{
			delete l_newvec;
			return false;
		};

		return true;
	};
	bool Sonik3DPoint::UnLinkPointZ(void)
	{
		if( z.GetRefCount() == 1 )
		{
			//カウント1なら自身のものにしちゃう。
		};

		double* l_newvec = new(std::nothrow) double;
		if(l_newvec == nullptr)
		{
			return false;
		};

		//値バックアップ
		(*l_newvec) = (*(this->z));

		//現在の値を使用した自分の座標を新しく持つ。
		if(!SonikLib::SharedSmtPtr<double>::SmartPointerCreate(l_newvec, z))
		{
			delete l_newvec;
			return false;
		};

		return true;
	};

	//=====================================
	//					SonikBazierBox Implement
	//=====================================
	SonikBazierBox::SonikBazierBox(double _x_, double _y_, double _endx_, double _endy_, double _pivx_, double  _pivy_, unsigned long _ef_)
	:StartX(_x_)
	,StartY(_y_)
	,EndX(_endx_)
	,EndY(_endy_)
	,PivotX(_pivx_)
	,PivotY(_pivy_)
	,Elapsed_Frame(_ef_)
	,m_saved_result_X(nullptr)
	,m_saved_result_Y(nullptr)
	{

	};
	SonikBazierBox::~SonikBazierBox(void)
	{
		if( m_saved_result_X != nullptr )
		{
			delete[] m_saved_result_X;
		};

		if( m_saved_result_Y != nullptr )
		{
			delete[] m_saved_result_Y;
		};

	};

	//セットされた値からすべてのフレーム結果を算出し、保持します。
	bool SonikBazierBox::SavedResultFrame(void)
	{
		//すでに配列を持っていれば一回開放。
		if( m_saved_result_X != nullptr )
		{
			delete[] m_saved_result_X;
			m_saved_result_X = nullptr;
		};
		if( m_saved_result_Y != nullptr )
		{
			delete[] m_saved_result_Y;
			m_saved_result_Y = nullptr;
		};

		//配列作成。
		m_saved_result_X = new(std::nothrow) double[Elapsed_Frame];
		if( m_saved_result_X == nullptr )
		{
			return false;
		};

		m_saved_result_Y = new(std::nothrow) double[Elapsed_Frame];
		if( m_saved_result_X == nullptr )
		{
			delete[] m_saved_result_X;
			m_saved_result_X = nullptr;
			return false;
		};

		double _getx;
		double _gety;
		for(uint32_t i=0; i < Elapsed_Frame; ++i)
		{
			Bezier_curveLine(_getx, _gety, i);
			m_saved_result_X[i] = _getx;
			m_saved_result_Y[i] = _gety;
		};

		return true;
	};
	//保持しているフレーム結果をすべて破棄します。
	void SonikBazierBox::SavedResultClear(void)
	{
		//すでに配列を持っていれば一回開放。
		if( m_saved_result_X != nullptr )
		{
			delete[] m_saved_result_X;
			m_saved_result_X = nullptr;
		};
		if( m_saved_result_Y != nullptr )
		{
			delete[] m_saved_result_Y;
			m_saved_result_Y = nullptr;
		};
	};

	//2次元のペジェ曲線に必要な情報を設定します。
	//引数最後のフラグは、先に計算を実施し、すべてのフレームの結果を保持しておくかどうかを設定します。(フレーム数 * doubleサイズ)分のメモリを使います。
	void SonikBazierBox::SetValue(double _x_, double _y_, double _endx_, double _endy_, double _pivx_, double  _pivy_, unsigned long _ef_)
	{
		StartX = _x_;
		StartY = _y_;
		EndX = _endx_;
		EndY = _endy_;
		PivotX = _pivx_;
		PivotY = _pivy_;
		Elapsed_Frame = _ef_;
	};

	//2次のベジェ曲線の位置点を算出します。
	//引数1: 結果であるX値を格納するために変数を指定します。
	//引数2: 結果であるY値を格納するために変数を指定します。
	//引数3: 始点から終点までの総合フレームの中で、何フレーム目の値を取得するのかを指定します。
	void SonikBazierBox::Bezier_curveLine(double& GetX, double& GetY, unsigned long GetFrame)
	{
		double time = 0.0;

		if(Elapsed_Frame < GetFrame)
		{
			time = 1.0;
		}else
		{
			time = GetFrame * (1 * Elapsed_Frame);
		};

		GetX = ((1.0 - time) * (1.0 - time) * StartX) + (2 * (1.0 - time) * time * PivotX) + (time * time * EndX);
		GetY = ((1.0 - time) * (1.0 - time) * StartY) + (2 * (1.0 - time) * time * PivotY) + (time * time * EndY);
	};

};


namespace SonikMath
{
	//指定した3DPointのベクトルの長さを計算します。
	double VectorLength(SonikMathDataBox::Sonik3DPoint& _point_) noexcept
	{
		return SonikMath::sqrt( (*(_point_.x)) * (*(_point_.x)) + (*(_point_.y)) * (*(_point_.y)) + (*(_point_.z)) * (*(_point_.z)));
	};

	//二点間の差を表すベクトルを取得します。
	void diffVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_, SonikMathDataBox::Sonik3DPoint& _outpoint_) noexcept
	{
	   (*( _outpoint_.x)) = (*(_s_pos_.x)) - (*(_f_pos_.x));
	   (*( _outpoint_.y)) = (*(_s_pos_.y)) - (*(_f_pos_.y));
	   (*( _outpoint_.z)) = (*(_s_pos_.z)) - (*(_f_pos_.z));
	    return;
	};

	//二点間のドット積を計算します。
	double DotProductVec(SonikMathDataBox::Sonik3DPoint& _f_pos_, SonikMathDataBox::Sonik3DPoint& _s_pos_) noexcept
	{
		return (*(_f_pos_.x)) * (*(_s_pos_.x)) + (*(_f_pos_.y)) * (*(_s_pos_.y)) + (*(_f_pos_.z)) * (*(_s_pos_.z));
	};


	//本クラスと引数で指定されたオブジェクト或いは座標との距離を算出します。
	double Distance(SonikMathDataBox::Sonik3DPoint& _f_point_, SonikMathDataBox::Sonik3DPoint& _s_point_) noexcept
	{
		double _distance[3];

		_distance[0] = (*(_f_point_.x)) - (*(_s_point_.x));
		_distance[1] = (*(_f_point_.y)) - (*(_s_point_.y));
		_distance[2] = (*(_f_point_.z)) - (*(_s_point_.z));

		return SonikMath::sqrt( _distance[0] * _distance[0] + _distance[1] * _distance[1] + _distance[2] * _distance[2]);

	};

	double Distance(double _fx_, double _fy_, double _fz_, double _sx_, double _sy_, double _sz_) noexcept
	{

		double _distance[3];

		_distance[0] = _fx_ - _sx_;
		_distance[1] = _fy_ - _sy_;
		_distance[2] = _fz_ - _sz_;

		return SonikMath::sqrt( _distance[0] * _distance[0] + _distance[1] * _distance[1] + _distance[2] * _distance[2]);
	};

	//位置を表す3DPointと方向を表す3DPointからパンニングを計算します。
	//0.0~1.0の範囲で返却します。3DAudioにおいて、ステレオチャンネルなどでL側への計算に使う場合は(1.0 - 返却値)を掛けるようにしてください。
	//3DPointのみの指定の場合は内部で全て計算するため、「使い回せる値もすべて計算しなおします。」でも指定が楽です。
	//diff, veclenを指定する場合は、それぞれ、diffVec, VectorLengthで事前計算した値を指定することを想定しています。
	//LisVec は リスナの方向ベクトルをVectorLengthで計算した結果を指定します。
	//PlyVecは リスナーの位置ベクトルと、音源(Player）の位置ベクトルをdiffVecで計算した結果の値をVectorLengthで計算した結果を指定します。
	double Panning(SonikMathDataBox::Sonik3DPoint& _lispos_, SonikMathDataBox::Sonik3DPoint& _plypos_, SonikMathDataBox::Sonik3DPoint& _lisdir_, SonikMathDataBox::Sonik3DPoint& _plydir_, double _atten_max_, double _attenuate_) noexcept
	{
	    // リスナーから音源へのベクトルを計算
		SonikMathDataBox::Sonik3DPoint l_point;
		diffVec(_plypos_, _lispos_, l_point);

	    // リスナーの向きと音源のベクトルの内積を計算
	    double dotProduct = DotProductVec(_lisdir_, l_point);

	    // リスナーの向きと音源のベクトルの長さを計算
	    double listenerDirLength = VectorLength(_lisdir_);
	    double sourceVecLength = VectorLength(l_point);

	    // 長さの逆数を計算
	    double invListenerDirLength = 1.0 / listenerDirLength;
	    double invSourceVecLength = 1.0 / sourceVecLength;

	    // パンニング角度を計算（-1.0から1.0の範囲）
	    double panning = dotProduct * invListenerDirLength * invSourceVecLength;

	    // パンニングの値を0.0から1.0の範囲にマッピング
	    panning = (panning + 1.0) * 0.5;

	    // リスナーの向きと音源の向きの内積を計算
	    double dotProductDir = DotProductVec(_lisdir_, _plydir_);

	    // リスナーの向きと音源の向きの長さを計算
	    double sourceDirLength = VectorLength(_plydir_);

	    // 長さの逆数を計算
	    double invSourceDirLength = 1.0 / sourceDirLength;

	    // 音源の向きとリスナーの向きの間の角度を計算（-1.0から1.0の範囲）
	    double directionality = dotProductDir * invListenerDirLength * invSourceDirLength;

	    // 音の強さを調整
	    double adjustedVolume = _attenuate_ + (_atten_max_ - _attenuate_) * ((directionality + 1.0) * 0.5);

	    // パンニングと音の強さを考慮した最終的な音量を計算
	    return panning * adjustedVolume;
	};
	double Panning(double _dotproduct_, double _lisdirveclen_, double _diffveclen_, double _dotproductdir_, double _plydirveclen_, double _atten_max_, double _attenuate_) noexcept
	{
	    // 長さの逆数を計算
	    double invListenerDirLength = 1.0 / _lisdirveclen_;
	    double invSourceVecLength = 1.0 / _diffveclen_;

	    // パンニング角度を計算（-1.0から1.0の範囲）
	    double panning = _dotproduct_ * invListenerDirLength * invSourceVecLength;

	    // パンニングの値を0.0から1.0の範囲にマッピング
	    panning = (panning + 1.0) * 0.5;

	    // リスナーの向きと音源の向きの内積を計算
	    double dotProductDir = _dotproductdir_;

	    // リスナーの向きと音源の向きの長さを計算
	    double sourceDirLength = _plydirveclen_;

	    // 長さの逆数を計算
	    double invSourceDirLength = 1.0 / sourceDirLength;

	    // 音源の向きとリスナーの向きの間の角度を計算（-1.0から1.0の範囲）
	    double directionality = dotProductDir * invListenerDirLength * invSourceDirLength;

	    // 音の強さを調整
	    double adjustedVolume = _attenuate_ + (_atten_max_ - _attenuate_) * ((directionality + 1.0) * 0.5);

	    // パンニングと音の強さを考慮した最終的な音量を計算
	    return panning * adjustedVolume;

	};

};

