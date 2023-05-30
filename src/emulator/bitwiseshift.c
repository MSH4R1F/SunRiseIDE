//
// Created by mshar1f on 30/05/23.
//
#include "bitwiseshift.h"
#define WORDSIZE  32

uint32_t logicalLeftShift(uint32_t number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number << shift;
}


uint32_t logicalRightShift(uint32_t number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return number >> shift;
}




uint32_t arithmeticRightShift(uint32_t number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (int32_t) (number << shift);
}
uint32_t rotateRight(uint32_t number, uint32_t shift) {
    if (shift > WORDSIZE) {
        return 0;
    }
    return (number >> shift) | (number << (32 - shift));
}