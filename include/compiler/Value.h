#pragma once

#include <cstdint>

//这里采用了IEEE754 NAN剩下的位数
//下面是IEEE 754标准
//1 bit sign | 11 bits exponent (0x7FF) | 52 bits significand (非零)
//NaN ​Exponent：全为 1（即 0x7FF）。​Significand：非零。
//这里采用，Quiet NaN (QNaN)：用于表示特殊的非数值。 编码格式如下，其中尾数最高位为0
// 1 bit sign | 11 bits exponent (0x7FF) | 52 bits significand (非零)
//​Signaling NaN：用于表示无效的浮点数。编码格式如上，区别是其中尾数最高位为1

constexpr uint64_t SIGN_BIT = 0x8000000000000000;
constexpr uint64_t QNAN = 0x7ffc000000000000;

constexpr uint64_t TAG_UNINITIALIZED = 0;
constexpr uint64_t TAG_NIL = 1;
constexpr uint64_t TAG_FALSE = 2;
constexpr uint64_t TAG_TRUE = 3;

constexpr uint64_t FALSE_VAL = QNAN | TAG_FALSE;
constexpr uint64_t TRUE_VAL = QNAN | TAG_TRUE;
constexpr uint64_t NIL_VAL = QNAN | TAG_NIL;
constexpr uint64_t UNINITIALIZED_VAL = QNAN | TAG_UNINITIALIZED;


enum class ObjctType {
    STRING = 1,
    FUNCTION = 2,
    CLOSURE = 3,
    UPVALUE = 4,
    CLASS = 5,
    INSTANCE = 6,
    BOUND_METHOD = 7,
};