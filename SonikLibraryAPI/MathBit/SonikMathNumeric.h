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

    // 指定されたサンプリング領域全体を一括FFT/IFFT処理するための関数ポインタを取得します。
    // 第1引数：入力サンプリング配列のサイズを指定します。
    // 第2引数：入力サイズに基づいて選択された一括処理用FFT関数（FullRangeFFT）への関数ポインタを受け取る変数を指定します。
    // 第3引数：入力サイズに基づいて選択された一括処理用IFFT関数（FullRangeIFFT）への関数ポインタを受け取る変数を指定します。
    // 第4引数：入力サイズに対して自動選択された最適処理レベル（NLevel）を受け取る変数を指定します。
    // ※取得した FFT/IFFT 関数を呼び出す際は、この NLevel を FFT/IFFT 関数の第3引数として必ず指定してください。
    //   指定しなかった場合の動作は未定義です。
    // ※この関数で取得した関数ポインタは「取得時に指定した入力サイズ」に対して最適化されています。
    //   取得時と異なるサイズの配列を使用した場合、動作は保証されません（同一サイズであれば問題ありません）。
    // ※FullRangeFFT / FullRangeIFFT の内部で、入力配列に対して最適化のための拡張処理（ゼロパディング）が行われます。
    //   出力用コンテナ（第2引数）はサイズ0の作成直後の状態を指定して問題ありません。
    // ※FullRangeFFT / FullRangeIFFT のout拡張処理前(どの条件においても通過します)に、出力コンテナに対して
    //   Clearが発行されます。
    // 本関数及び取得できるFFT. IFFTは内部で窓関数は適用しません。別途関数で入力前に適用してください。
    SLIB_CVR_USING(FFTFullFunctionP, bool(*)(const SLIB_SAMPLINGLIST, SLIB_SAMPLINGLIST, uint32_t));
    DEF_PRE_NO_DISCARD bool Get_FullRange_FFTFunction(uint64_t inSamplingsize, FFTFullFunctionP& out_Full_FFT_funcptr, FFTFullFunctionP& out_Full_IFFT_funcptr, uint32_t& out_NLevel) DEF_POST_NO_DISCARD;

    // 指定されたサンプリング配列に対して、必要に応じて固定長処理に適した
    // サイズ（baseN の倍数）へ拡張（ゼロパディング）を行い、
    // その結果に対応する固定長 FFT / IFFT 関数ポインタを取得します。
    // 第1引数：固定長処理を行いたいサンプリング配列を指定します。
    //           ※スマートポインタのため、拡張が発生した場合は inSampling が更新されます。
    // 第2引数：第4引数で指定した固定長処理レベルに対応する FFT 関数へのポインタを受け取る変数を指定します。
    // 第3引数：第4引数で指定した固定長処理レベルに対応する IFFT 関数へのポインタを受け取る変数を指定します。
    // 第4引数：任意の固定長処理レベル（NLevel）を指定します。
    //           NLevel = 0 → 512 サンプル
    //           NLevel = 1 → 1024 サンプル
    //           NLevel = 2 → 2048 サンプル
    //           NLevel = 3 → 4096 サンプル
    //           上記以外の値を指定した場合、本関数は false を返します。
    // ※入力サイズがすでに baseN の倍数である場合、拡張処理は行われません。
    // ※取得した FFT / IFFT 関数は、第3引数（idx）を使用することで、
    //   同じ配列に対して連続的・継続的に固定長処理を行うことが可能です。
    // 本関数は内部で窓関数は適用しません。別途関数で入力前に適用してください。
    SLIB_CVR_USING(FFTFixedFuntionP, bool(*)(const SLIB_SAMPLINGLIST, SLIB_SAMPLINGLIST, uint64_t&));
    DEF_PRE_NO_DISCARD bool Get_FixedRange_FFTFuncction(SLIB_SAMPLINGLIST inSampling, FFTFixedFuntionP& out_Fixed_FFT_funcptr, FFTFixedFuntionP& out_Fixed_IFFT_funcptr, uint32_t NLevel) DEF_POST_NO_DISCARD;

    //窓関数
    //窓関数ロジック：ハニング
    void ApplyWindow_Hann(SLIB_SAMPLINGLIST inSampling);
    //窓関数ロジック：ハミング
    void ApplyWindow_Hamming(SLIB_SAMPLINGLIST inSampling);
    //窓関数ロジック：ブラックマン
    void ApplyWindow_Blackman(SLIB_SAMPLINGLIST inSampling);

    // サンプリングレート変換を行います。
    // 第1引数：入力データ (FFTおよび窓関数が適用されます)
    // 第2引数：出力データ格納先
    // 第3引数：変換元レートを指定します。(例: 44100.0)
    // 第4引数：変換先レートを指定します。(例: 48000.0)
    // アップサンプリング例: currentRate = 44100.0, targetRate = 48000.0
    // ダウンサンプリング例: currentRate = 96000.0, targetRate = 44100.0
    bool ReSamplingTransform(SLIB_SAMPLINGLIST in_Sampling, SLIB_SAMPLINGLIST out_Resampled, double currentRate, double targetRate);

#ifdef _DEBUG

	//Sin値及びCos値のリストをtxtに出力します。
	//_DEBUGが定義されている時のみ有効です。
	void SinCosValueOutPut(void);

#endif


};


#endif //__SONIKLIB_NUMERIC_HEDDER__