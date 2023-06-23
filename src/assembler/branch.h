//
// Created by Faraz Malik on 17/06/2023.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "../general/labelMap.h"

#ifndef ARMV8_27_BRANCH_H
    #define ARMV8_27_BRANCH_H

    bool isBranch(char *opcode);
    uint32_t assembleBranch(char *opcode, char **operands, long long currentAddress, LabelAddressMap **labelMap);
#endif //ARMV8_27_BRANCH_H
