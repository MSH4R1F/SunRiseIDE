//
// Created by Faraz Malik on 17/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "../general/labelMap.h"

#ifndef ARMV8_27_KEY_H
    #define ARMV8_27_KEY_H

    uint32_t encodeRegister(char *operand);
    uint32_t encodeSimm(char *operand, bool hasHash);
    uint32_t encodeImm(char *operand, bool hasHash);
    uint32_t encodeShift(char *operand);

    long long encodeLiteralToOffset(char *literal, long long currentAddress, LabelAddressMap **labelMap);
#endif //ARMV8_27_KEY_H
