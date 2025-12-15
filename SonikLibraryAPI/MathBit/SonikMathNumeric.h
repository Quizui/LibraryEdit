#ifndef __SONIKLIB_NUMERIC_HEDDER__
#define __SONIKLIB_NUMERIC_HEDDER__

#include <CPPGrammarDefines.h>
#include <CompilersPreProcesser.h>
#include <SmartPointer/SonikSmartPointer.hpp>
#include <Container/RangedForContainer.hpp>

namespace SonikMath
{
	class SonikComplex
	{
    public:
		double re; //実部、実数
		double im; //虚部、虚数

    public:
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex(double _re_ = 0.0, double _im_ = 0.0)
        :re(_re_)
        ,im(_im_)
        {/*none*/};

        SonikComplex(const SonikComplex& _copy_)
        {
            re = _copy_.re;
            im = _copy_.im;
        };
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex& operator =(const SonikComplex& _rhs_)
        {
            if(this == &_rhs_)
            {
                return (*this);
            };

            re = _rhs_.re;
            im = _rhs_.im;

            return (*this);
        };

        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex& operator +=(const SonikComplex& _rhs_)
        {
            re += _rhs_.re;
            im += _rhs_.im;

            return (*this);
        };

        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex& operator -=(const SonikComplex& _rhs_)
        {
            re -= _rhs_.re;
            im -= _rhs_.im;

            return (*this);
        };

        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex& operator *=(const SonikComplex& _rhs_)
        {
            //公式：(a+bi)(c+di) = (ac-bd) + (ad+bc)i
            double l_real  = re * _rhs_.re - im * _rhs_.im; //(ac-bd)
                                                            //   +
            double l_image = re * _rhs_.im + im * _rhs_.re; //(ad+bc)i
            
            re = l_real; 
            im = l_image;

            return (*this);
        };
        DEF_FORCE_INLINE SLIB_CVR_CONSTEXPR SonikComplex& operator /=(const SonikComplex& _rhs_)
        {
            // 公式：(a+bi) / (c+di) = (ac+bd)/(c^2+d^2) + (bc-ad)/(c^2+d^2)i
            double denom = _rhs_.re * _rhs_.re + _rhs_.im * _rhs_.im;  // 分母 (c^2 + d^2)
            if (denom == 0.0)
            {
                // ゼロ除算防止
                re = 0.0;
                im = 0.0;

                return (*this);
            };

            double l_real  = (re * _rhs_.re + im * _rhs_.im) / denom; // 実部 (ac+bd)/(c^2+d^2)
            double l_image = (im * _rhs_.re - re * _rhs_.im) / denom; // 虚部 (bc-ad)/(c^2+d^2)

            re = l_real;
            im = l_image;

            return (*this);
        };

        DEF_FORCE_INLINE SonikComplex operator +(const SonikComplex& _rhs_) const
        {
            SonikComplex retobj(re + _rhs_.re, im + _rhs_.im);
            return retobj;
        };

        DEF_FORCE_INLINE SonikComplex operator -(const SonikComplex& _rhs_) const
        {
            SonikComplex retobj(re - _rhs_.re, im - _rhs_.im);
            return retobj;
        };
        DEF_FORCE_INLINE SonikComplex operator *(const SonikComplex& _rhs_) const
        {
            //公式：(a+bi)(c+di) = (ac-bd) + (ad+bc)i
            SonikComplex retobj(re * _rhs_.re - im * _rhs_.im    //(ac-bd)
                                                                     //   +
                                , re * _rhs_.im + im * _rhs_.re  //(ad+bc)i
                                );
            return retobj;
        };
        DEF_FORCE_INLINE SonikComplex operator /(const SonikComplex& _rhs_) const
        {
            // 公式：(a+bi) / (c+di) = (ac+bd)/(c^2+d^2) + (bc-ad)/(c^2+d^2)i
            double denom = _rhs_.re * _rhs_.re + _rhs_.im * _rhs_.im;  // 分母 (c^2 + d^2)
            if (denom == 0.0)
            {
                //0除算防止。発生なら0のペアを返却
                return SonikComplex(0.0, 0.0);
            }

            return SonikComplex( (re * _rhs_.re + im * _rhs_.im) / denom, // 実部 (ac+bd)/(c^2+d^2)
                                 (im * _rhs_.re - re * _rhs_.im) / denom  // 虚部 (bc-ad)/(c^2+d^2)
                               );
        };
	};

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

    //Disntance Fourier Transform(離散フーリエ変換)
    SLIB_CVR_USING(SLIB_SAMPLINGLIST, SonikLib::SharedSmtPtr<SonikLib::Container::SonikVariableArrayContainer<SonikComplex>>);
    bool DFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST& out_Sampling);
    bool IDFT(const SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST& out_Sampling);


#ifdef _DEBUG

	//Sin値及びCos値のリストをtxtに出力します。
	//_DEBUGが定義されている時のみ有効です。
	void SinCosValueOutPut(void);

#endif


};


#endif //__SONIKLIB_NUMERIC_HEDDER__