//
// Created by mshar1f on 30/05/23.
//

#ifndef ARMV8_27_BITWISESHIFT_H
#define ARMV8_27_BITWISESHIFT_H
#include <stdint.h>
uint32_t logicalLeftShift(uint32_t number, uint32_t shift);
uint32_t logicalRightShift(uint32_t number, uint32_t shift);
uint32_t arithmeticRightShift(uint32_t number, uint32_t shift);
uint32_t rotateRight(uint32_t number, uint32_t shift);

#endif //ARMV8_27_BITWISESHIFT_H
