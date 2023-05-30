//
// Created by mshar1f on 30/05/23.
//
#include "bitwiseshift.h"
#define WORDSIZE  32

long long logicalLeftShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number << shift;
}


long long logicalRightShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number >> shift;
}




long long arithmeticRightShift(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (int32_t) (number << shift);
}
long long rotateRight(long long number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (number >> shift) | (number << (32 - shift));
}