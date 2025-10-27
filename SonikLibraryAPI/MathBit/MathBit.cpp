
#include "MathBit.h"

#if defined(__GNUC__) || defined(__clang__)
    #define __SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__

    #if defined(__BMI__)
        #include <bmiintrin.h> //BMI bitfunction supported
        #undef ___SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__
        #define __SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__
    #endif

#elif defined(_MSC_VER) // MSVC でビットカウントを行うための関数
    #define __SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__
    #include <intrin.h>
    #include <immintrin.h>

    #if defined(__BMI__)
        #include <bmiintrin.h> //BMI bitfunction supported
        #undef __SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__
        #define __SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__
    #endif

#endif

namespace SonikMathBit
{
    //立っているビットの数をカウントします。
    uint16_t OnBitCount(uint8_t BitValue)
    {
        if (BitValue == 0)
        {

            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_popcount(static_cast<uint32_t>(BitValue)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return static_cast<uint16_t>(__popcnt(static_cast<uint32_t>(BitValue)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__popcnt_u32(static_cast<uint32_t>(BitValue)));
#else
        //黒魔術ビット操作
        BitValue = (BitValue & 0x55) + ((BitValue >> 1) & 0x55);
        BitValue = (BitValue & 0x33) + ((BitValue >> 2) & 0x33);

        return (BitValue & 0x0F) + ((BitValue >> 4) & 0x0F);

#endif
    };
    uint16_t OnBitCount(uint16_t BitValue)
    {
        if (BitValue == 0)
        {

            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_popcount(static_cast<uint32_t>(BitValue)));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return static_cast<uint16_t>(__popcnt(static_cast<uint32_t>(BitValue)));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__popcnt_u32(static_cast<uint32_t>(BitValue)));
#else

        BitValue = (BitValue & 0x5555) + ((BitValue >> 1) & 0x5555);
        BitValue = (BitValue & 0x3333) + ((BitValue >> 2) & 0x3333);
        BitValue = (BitValue & 0x0F0F) + ((BitValue >> 4) & 0x0F0F);

        return (BitValue & 0x00FF) + ((BitValue >> 8) & 0x00FF);
#endif
    };
    uint16_t OnBitCount(uint32_t BitValue)
    {
        if (BitValue == 0)
        {

            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_popcount(BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return static_cast<uint16_t>(__popcnt(BitValue));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__popcnt_u32(BitValue));
#else

        BitValue = (BitValue & 0x55555555) + ((BitValue >> 1) & 0x55555555);
        BitValue = (BitValue & 0x33333333) + ((BitValue >> 2) & 0x33333333);
        BitValue = (BitValue & 0x0F0F0F0F) + ((BitValue >> 4) & 0x0F0F0F0F);
        BitValue = (BitValue & 0x00FF00FF) + ((BitValue >> 8) & 0x00FF00FF);

        return (BitValue & 0x0000FFFF) + ((BitValue >> 16) & 0x0000FFFF);
#endif
    };
    uint16_t OnBitCount(uint64_t BitValue)
    {
        if (BitValue == 0)
        {

            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_popcountll (BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return static_cast<uint16_t>(__popcnt64(BitValue));

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__popcnt_u64(BitValue));
#else

        BitValue = (BitValue & 0x5555555555555555) + ((BitValue >> 1) & 0x5555555555555555);
        BitValue = (BitValue & 0x3333333333333333) + ((BitValue >> 2) & 0x3333333333333333);
        BitValue = (BitValue & 0x0F0F0F0F0F0F0F0F) + ((BitValue >> 4) & 0x0F0F0F0F0F0F0F0F);
        BitValue = (BitValue & 0x00FF00FF00FF00FF) + ((BitValue >> 8) & 0x00FF00FF00FF00FF);
        BitValue = (BitValue & 0x0000FFFF0000FFFF) + ((BitValue >> 16) & 0x0000FFFF0000FFFF);

        return static_cast<uint16_t>( (BitValue & 0x00000000FFFFFFFF) + ((BitValue >> 32) & 0x00000000FFFFFFFF) );
#endif
    };


    //立っているビット中一番最上位のビットの位置を取得します。(1~カウント)
    int16_t GetMSB(uint8_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(8 - __builtin_clz(BitValue) - 24);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(8 - __lzcnt_u32(BitValue) + 24);

#else

        int16_t bit = 8;
        
/*        if (!BitValue)
        {
            return 0;
        };
*/
        if (!(BitValue & 0xf0)) { BitValue <<= 4; bit -= 4;};
        if (!(BitValue & 0xc0)) { BitValue <<= 2; bit -= 2; };
        if (!(BitValue & 0x80)) { BitValue <<= 1; --bit; };
        
        return bit;
#endif
    };

    int16_t GetMSB(uint16_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(16 - __builtin_clz(BitValue) - 16);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(16 - __lzcnt_u32(BitValue) + 16);

#else

        int16_t bit = 16;

/*        if (!BitValue)
        {
            return 0;
        };
*/

        if (!(BitValue & 0xff00)) { BitValue <<= 8; bit -= 8; }
        if (!(BitValue & 0xf000)) { BitValue <<= 4; bit -= 4; }
        if (!(BitValue & 0xc000)) { BitValue <<= 2; bit -= 2; }
        if (!(BitValue & 0x8000)) { BitValue <<= 1; --bit; }

        return bit;

#endif
    };

    int16_t GetMSB(uint32_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(32 - __builtin_clz(BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(32 - __lzcnt_u32(BitValue));

#else

        int16_t bit = 32; 
        
/*        if (!BitValue)
        {
            return 0;
        };
*/      
        if (!(BitValue & 0xffff0000)) { BitValue <<= 16; bit -= 16; };
        if (!(BitValue & 0xff000000)) { BitValue <<= 8;  bit -= 8; };
        if (!(BitValue & 0xf0000000)) { BitValue <<= 4;  bit -= 4; };
        if (!(BitValue & 0xc0000000)) { BitValue <<= 2;  bit -= 2; };
        if (!(BitValue & 0x80000000)) { BitValue <<= 1;  --bit; };
        
        return bit;
#endif
    };

    int16_t GetMSB(uint64_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(64 - __builtin_clzll(BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse64(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(64 - __lzcnt_u64(BitValue));

#else

        int16_t bit = 64;
        
/*        if (!BitValue)
        {
            return 0;
        };
*/      
        if (!(BitValue & 0xffffffff00000000)) { BitValue <<= 32; bit -= 32; };
        if (!(BitValue & 0xffff000000000000)) { BitValue <<= 16; bit -= 16; };
        if (!(BitValue & 0xff00000000000000)) { BitValue <<= 8; bit -= 8; };
        if (!(BitValue & 0xf000000000000000)) { BitValue <<= 4; bit -= 4; };
        if (!(BitValue & 0xc000000000000000)) { BitValue <<= 2; bit -= 2; };
        if (!(BitValue & 0x8000000000000000)) { BitValue <<= 1; --bit; }
        
        return bit;
#endif
    };


    //立っていないビット中一番最上位のビットの位置を取得します。(1~カウント)
    int16_t GetZEROMSB(uint8_t BitValue)
    {
        if (BitValue == 0xFF)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(8 - __builtin_clz(~BitValue) - 24);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), static_cast<uint32_t>( (~BitValue) << 24 )) ? index - 23 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(8 - __lzcnt_u32(~BitValue) + 24);

#else
        int16_t bit = 8;

        BitValue = ~BitValue;

        if (!(BitValue & 0xf0)) { BitValue <<= 4; bit -= 4; };
        if (!(BitValue & 0xc0)) { BitValue <<= 2; bit -= 2; };
        if (!(BitValue & 0x80)) { BitValue <<= 1; --bit; };
        
        return bit;
#endif
    };
    int16_t GetZEROMSB(uint16_t BitValue)
    {


        if (BitValue == 0xFFFF)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(16 - __builtin_clz(~BitValue) - 16);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), static_cast<uint32_t>((~BitValue) << 16)) ? index - 15 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(16 - __lzcnt_u32(~BitValue) + 16);

#else
        int16_t bit = 16;

        BitValue = ~BitValue;

        if (!(BitValue & 0xff00)) { BitValue <<= 8; bit -= 8; }
        if (!(BitValue & 0xf000)) { BitValue <<= 4; bit -= 4; }
        if (!(BitValue & 0xc000)) { BitValue <<= 2; bit -= 2; }
        if (!(BitValue & 0x8000)) { BitValue <<= 1; --bit; }

        return bit;
#endif
    };
    int16_t GetZEROMSB(uint32_t BitValue)
    {
        if (BitValue == 0xFFFFFFFF)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(32 - __builtin_clz(~BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse(reinterpret_cast<unsigned long*>(&index), ~BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(32 - __lzcnt_u32(~BitValue));

#else
        int16_t bit = 32;

        BitValue = ~BitValue;

        if (!(BitValue & 0xffff0000)) { BitValue <<= 16; bit -= 16; };
        if (!(BitValue & 0xff000000)) { BitValue <<= 8;  bit -= 8; };
        if (!(BitValue & 0xf0000000)) { BitValue <<= 4;  bit -= 4; };
        if (!(BitValue & 0xc0000000)) { BitValue <<= 2;  bit -= 2; };
        if (!(BitValue & 0x80000000)) { BitValue <<= 1;  --bit; };

        return bit;
#endif
    };
    int16_t GetZEROMSB(uint64_t BitValue)
    {
        if (BitValue == 0xFFFFFFFFFFFFFFFF)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(64 - __builtin_clzll(~BitValue));

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanReverse64(reinterpret_cast<unsigned long*>(&index), ~BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(64 - __lzcnt_u64(~BitValue));

#else
        int16_t bit = 64;

        BitValue = ~BitValue;

        if (!(BitValue & 0xffffffff00000000)) { BitValue <<= 32; bit -= 32; };
        if (!(BitValue & 0xffff000000000000)) { BitValue <<= 16; bit -= 16; };
        if (!(BitValue & 0xff00000000000000)) { BitValue <<= 8; bit -= 8; };
        if (!(BitValue & 0xf000000000000000)) { BitValue <<= 4; bit -= 4; };
        if (!(BitValue & 0xc000000000000000)) { BitValue <<= 2; bit -= 2; };
        if (!(BitValue & 0x8000000000000000)) { BitValue <<= 1; --bit; }

        return bit;
#endif
    };


    //立っているビット中一番最下位のビットの位置を取得します。(1~カウント)
    int16_t GetLSB(uint8_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(BitValue) + 1);

#else

        int16_t bit = 0;
        
        BitValue &= ~BitValue + 1;
        if (BitValue & 0xf0) bit += 4;
        if (BitValue & 0xcc) bit += 2;
        if (BitValue & 0xaa) ++bit;
        ++bit;
        
        return bit;
#endif
    };

    int16_t GetLSB(uint16_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(BitValue) + 1);

#else
        int16_t bit = 0;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xff00) bit += 8;
        if (BitValue & 0xf0f0) bit += 4;
        if (BitValue & 0xcccc) bit += 2;
        if (BitValue & 0xaaaa) ++bit;
        ++bit;

        return bit;
#endif

    };

    int16_t GetLSB(uint32_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(BitValue) + 1);

#else
        int16_t bit = 0; 
        
        BitValue &= ~BitValue + 1;
        if (BitValue & 0xffff0000) bit += 16;
        if (BitValue & 0xff00ff00) bit += 8;
        if (BitValue & 0xf0f0f0f0) bit += 4;
        if (BitValue & 0xcccccccc) bit += 2;
        if (BitValue & 0xaaaaaaaa) ++bit;
        ++bit;
        
        return bit;
#endif
    };

    int16_t GetLSB(uint64_t BitValue)
    {
        if (BitValue == 0)
        {
            return 0;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctzll(BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward64(reinterpret_cast<unsigned long*>(&index), BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u64(BitValue) + 1);

#else
        int bit = 0;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xffffffff00000000) bit += 32;
        if (BitValue & 0xffff0000ffff0000) bit += 16;
        if (BitValue & 0xff00ff00ff00ff00) bit += 8;
        if (BitValue & 0xf0f0f0f0f0f0f0f0) bit += 4;
        if (BitValue & 0xcccccccccccccccc) bit += 2;
        if (BitValue & 0xaaaaaaaaaaaaaaaa) ++bit;
        ++bit;
        
        return bit;
#endif
    };


    //立っていないビット中一番最下位のビットの位置を取得します。(1~カウント)
    int16_t GetZEROLSB(uint8_t BitValue)
    {
        if (BitValue == 0)
        {
            return 1;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(~BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), static_cast<uint32_t>((~BitValue) << 24)) ? index - 23 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(~BitValue) + 1);

#else

        int16_t bit = 0;

        BitValue = ~BitValue;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xf0) bit += 4;
        if (BitValue & 0xcc) bit += 2;
        if (BitValue & 0xaa) ++bit;
        ++bit;

        return bit;
#endif
    };

    int16_t GetZEROLSB(uint16_t BitValue)
    {
        if (BitValue == 0)
        {
            return 1;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(~BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), static_cast<uint32_t>((~BitValue) << 16)) ? index - 15 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(~BitValue) + 1);

#else
        int16_t bit = 0;

        BitValue = ~BitValue;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xff00) bit += 8;
        if (BitValue & 0xf0f0) bit += 4;
        if (BitValue & 0xcccc) bit += 2;
        if (BitValue & 0xaaaa) ++bit;
        ++bit;

        return bit;
#endif
    };

    int16_t GetZEROLSB(uint32_t BitValue)
    {
        if (BitValue == 0)
        {
            return 1;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctz(~BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward(reinterpret_cast<unsigned long*>(&index), ~BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u32(~BitValue) + 1);

#else
        int16_t bit = 0;

        BitValue = ~BitValue;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xffff0000) bit += 16;
        if (BitValue & 0xff00ff00) bit += 8;
        if (BitValue & 0xf0f0f0f0) bit += 4;
        if (BitValue & 0xcccccccc) bit += 2;
        if (BitValue & 0xaaaaaaaa) ++bit;
        ++bit;

        return bit;
#endif
    };

    int16_t GetZEROLSB(uint64_t BitValue)
    {
        if (BitValue == 0)
        {
            return 1;
        };

#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return static_cast<uint16_t>(__builtin_ctzll(~BitValue) + 1);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        uint32_t index = 0;
        return static_cast<uint16_t>(_BitScanForward64(reinterpret_cast<unsigned long*>(&index), ~BitValue) ? index + 1 : 0);

#elif defined(__SONIKLIB_MATHBIT_BMI_BITFUNCTION_SUPPORTED__)
        return static_cast<uint16_t>(__tzcnt_u64(~BitValue) + 1);

#else
        int16_t bit = 0;

        BitValue = ~BitValue;

        BitValue &= ~BitValue + 1;
        if (BitValue & 0xffffffff00000000) bit += 32;
        if (BitValue & 0xffff0000ffff0000) bit += 16;
        if (BitValue & 0xff00ff00ff00ff00) bit += 8;
        if (BitValue & 0xf0f0f0f0f0f0f0f0) bit += 4;
        if (BitValue & 0xcccccccccccccccc) bit += 2;
        if (BitValue & 0xaaaaaaaaaaaaaaaa) ++bit;
        ++bit;

        return bit;
#endif
    };


    //ビットの並びを反転させます。
    uint8_t BitSwap(uint8_t BitValue)
    {
        BitValue = ((BitValue & 0x55) << 1) | ((BitValue & 0xAA) >> 1);
        BitValue = ((BitValue & 0x33) << 2) | ((BitValue & 0xCC) >> 2);

        return ((BitValue & 0x0F) << 4) | ((BitValue & 0xF0) >> 4);
    };

    uint16_t BitSwap(uint16_t BitValue)
    {
        BitValue = ((BitValue & 0x5555) << 1) | ((BitValue & 0xAAAA) >> 1);
        BitValue = ((BitValue & 0x3333) << 2) | ((BitValue & 0xCCCC) >> 2);
        BitValue = ((BitValue & 0x0F0F) << 4) | ((BitValue & 0xF0F0) >> 4);

        return ((BitValue & 0x00FF) << 8) | ((BitValue & 0xFF00) >> 8);
    };

    uint32_t BitSwap(uint32_t BitValue)
    {
#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return __builtin_bswap32(BitValue);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return _byteswap_ulong(BitValue);

#else
        BitValue = ((BitValue & 0x55555555) << 1) | ((BitValue & 0xAAAAAAAA) >> 1);
        BitValue = ((BitValue & 0x33333333) << 2) | ((BitValue & 0xCCCCCCCC) >> 2);
        BitValue = ((BitValue & 0x0F0F0F0F) << 4) | ((BitValue & 0xF0F0F0F0) >> 4);
        BitValue = ((BitValue & 0x00FF00FF) << 8) | ((BitValue & 0xFF00FF00) >> 8);

        return ((BitValue & 0x0000FFFF) << 16) | ((BitValue & 0xFFFF0000) >> 16);
#endif
    };

    uint64_t BitSwap(uint64_t BitValue)
    {
#if defined(__SONIKLIB_MATHBIT_BITFUNCTION_GCC_OTHER__)
        return __builtin_bswap64(BitValue);

#elif defined(__SONIKLIB_MATHBIT_BITFUNCTION_WINDOWS_OTHER__)
        return _byteswap_uint64(BitValue);

#else
        BitValue = ((BitValue & 0x5555555555555555) << 1) | ((BitValue & 0xAAAAAAAAAAAAAAAA) >> 1);
        BitValue = ((BitValue & 0x3333333333333333) << 2) | ((BitValue & 0xCCCCCCCCCCCCCCCC) >> 2);
        BitValue = ((BitValue & 0x0F0F0F0F0F0F0F0F) << 4) | ((BitValue & 0xF0F0F0F0F0F0F0F0) >> 4);
        BitValue = ((BitValue & 0x00FF00FF00FF00FF) << 8) | ((BitValue & 0xFF00FF00FF00FF00) >> 8);
        BitValue = ((BitValue & 0x0000FFFF0000FFFF) << 16) | ((BitValue & 0xFFFF0000FFFF0000) >> 16);

        return ((BitValue & 0x00000000FFFFFFFF) << 32) | ((BitValue & 0xFFFFFFFF00000000) >> 32);
#endif
    };

    void EndianConvertFor2Byte(char* _data_, uint64_t _datasize_)
    {
        uint64_t l_loop = static_cast<uint64_t>(_datasize_ * 0.5); //_datasize_ / sizeof(int16_t)
        uint16_t* lp_data = reinterpret_cast<uint16_t*>(_data_);

        for (uint64_t i = 0; i < l_loop; ++i)
        {
            lp_data[i] = (lp_data[i] << 8) | ((lp_data[i] >> 8) & 0xFF);
        };
    };

    void EndianConvertFor4Byte(char* _data_, uint64_t _datasize_)
    {
        uint64_t l_loop = static_cast<uint64_t>(_datasize_ * 0.25); //_datasize_ / sizeof(int32_t)
        uint32_t* lp_data = reinterpret_cast<uint32_t*>(_data_);

        for (uint64_t i = 0; i < l_loop; ++i)
        {
            lp_data[i] = (lp_data[i] << 24)
                | ((lp_data[i] & 0x0000FF00) << 8)
                | ((lp_data[i] & 0x00FF0000) >> 8)
                | ((lp_data[i] & 0xFF000000) >> 24);
        };
    };

    void EndianConvertFor8Byte(char* _data_, uint64_t _datasize_)
    {
        uint64_t l_loop = static_cast<uint64_t>(_datasize_ * 0.125); //_datasize_ / sizeof(int64_t)
        uint64_t* lp_data = reinterpret_cast<uint64_t*>(_data_);

        for (uint64_t i = 0; i < l_loop; ++i)
        {
            lp_data[i] = (lp_data[i] << 56)
                | ((lp_data[i] & 0x000000000000FF00ULL) << 40)
                | ((lp_data[i] & 0x0000000000FF0000ULL) << 24)
                | ((lp_data[i] & 0x00000000FF000000ULL) << 8)
                | ((lp_data[i] & 0x000000FF00000000ULL) >> 8)
                | ((lp_data[i] & 0x0000FF0000000000ULL) >> 24)
                | ((lp_data[i] & 0x00FF000000000000ULL) >> 40)
                | ((lp_data[i] & 0xFF00000000000000ULL) >> 56);
        };
    };

};
