//
// Created by Faraz Malik on 09/06/2023.
//

#include "bitwiseShift.h"

uint64_t logicalLeftShift(uint64_t number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    if (shift > wordSize) {
        return 0;
    }
    if (wordSize == 32) {
        number <<= shift;
        return number & 0xFFFFFFFF;
    } else {
        return number << shift;
    }
}

/// Performs logical shift right
uint64_t logicalRightShift(uint64_t number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    if (shift > wordSize) {
        return 0;
    }
    if (wordSize == 32) {
        number &= 0xFFFFFFFF;
        return number >> shift;
    }
    return number >> shift;
}

/// Performs arithmetic shift right
long long arithmeticRightShift(long long number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    uint64_t mask = is64Bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    if (shift > wordSize) {
        return 0;
    }
    number &= mask;
    bool signBit = (number >> (wordSize - 1)) & 1;
    if (signBit) {
        return ((mask << (wordSize - shift)) | (number >> shift)) & mask;
    }
    return logicalRightShift(number, shift, is64Bit);
}

/// Fileprivate helper function
static uint64_t ToPow2(int power) {
    uint64_t x = 1;
    for (int i = 0; i < power; i++) {
        x *= 2;
    }
    return x;
}

/// Performs bitwise rotation
long long rotateRight(long long number, uint32_t shift, bool is64Bit) {
    int wordSize = is64Bit ? 64 : 32;
    uint64_t mask = is64Bit ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF;
    if (shift > wordSize) {
        return 0;
    }
    number &= mask;
    uint64_t rotatedBits = (number & (ToPow2(shift) - 1)) << (wordSize - shift);
    uint64_t clearer = (ToPow2(wordSize - shift) - 1);
    return (((number >> shift) & clearer) | rotatedBits) & mask;
}