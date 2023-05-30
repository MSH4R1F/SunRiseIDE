//
// Created by mshar1f on 30/05/23.
//

#ifndef ARMV8_27_BITWISESHIFT_H
#define ARMV8_27_BITWISESHIFT_H
#include <stdint.h>
long long logicalLeftShift(long long number, uint32_t shift);
long long logicalRightShift(long long number, uint32_t shift);
long long arithmeticRightShift(long long number, uint32_t shift);
long long rotateRight(long long number, uint32_t shift);

#endif //ARMV8_27_BITWISESHIFT_H
