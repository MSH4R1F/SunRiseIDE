//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdint.h>
#include <stdbool.h>

#ifndef ARMV8_27_BITWISESHIFT_H
    #define ARMV8_27_BITWISESHIFT_H

    uint64_t logicalLeftShift(uint64_t number, uint32_t shift, bool is64Bit);
    uint64_t logicalRightShift(uint64_t number, uint32_t shift, bool is64Bit);
    long long arithmeticRightShift(long long number, uint32_t shift, bool is64Bit);
    long long rotateRight(long long number, uint32_t shift, bool is64Bit);
#endif //ARMV8_27_BITWISESHIFT_H
