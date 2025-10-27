#ifndef MATH_BIT_COMMON_HEADER_
#define MATH_BIT_COMMON_HEADER_

#include <cstdint>

namespace SonikMathBit
{
    //立っているビットの数をカウントします。
    uint16_t OnBitCount(uint8_t BitValue);
    uint16_t OnBitCount(uint16_t BitValue);
    uint16_t OnBitCount(uint32_t BitValue);
    uint16_t OnBitCount(uint64_t BitValue);

    //立っているビット中一番最上位のビットの位置を取得します。(1~カウント)
    int16_t GetMSB(uint8_t BitValue);
    int16_t GetMSB(uint16_t BitValue);
    int16_t GetMSB(uint32_t BitValue);
    int16_t GetMSB(uint64_t BitValue);

    //立っていないビット中一番最上位のビットの位置を取得します。(1~カウント)
    int16_t GetZEROMSB(uint8_t BitValue);
    int16_t GetZEROMSB(uint16_t BitValue);
    int16_t GetZEROMSB(uint32_t BitValue);
    int16_t GetZEROMSB(uint64_t BitValue);

    //立っているビット中一番最下位のビットの位置を取得します。(1~カウント)
    int16_t GetLSB(uint8_t BitValue);
    int16_t GetLSB(uint16_t BitValue);
    int16_t GetLSB(uint32_t BitValue);
    int16_t GetLSB(uint64_t BitValue);

    //立っていないビット中一番最下位のビットの位置を取得します。(1~カウント)
    int16_t GetZEROLSB(uint8_t BitValue);
    int16_t GetZEROLSB(uint16_t BitValue);
    int16_t GetZEROLSB(uint32_t BitValue);
    int16_t GetZEROLSB(uint64_t BitValue);

    //ビットの並びを反転させます。
    uint8_t BitSwap(uint8_t BitValue);
    uint16_t BitSwap(uint16_t BitValue);
    uint32_t BitSwap(uint32_t BitValue);
    uint64_t BitSwap(uint64_t BitValue);

    //エンディアン変換を行います。
    void EndianConvertFor2Byte(char* _data_, uint64_t _datasize_);
    void EndianConvertFor4Byte(char* _data_, uint64_t _datasize_);
    void EndianConvertFor8Byte(char* _data_, uint64_t _datasize_);
};

#endif
