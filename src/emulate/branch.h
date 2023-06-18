//
// Created by Faraz Malik on 09/06/2023.
//

#include <stdbool.h>
#include <stdint.h>

#include "registers.h"

#ifndef ARMV8_27_BRANCH_H
    #define ARMV8_27_BRANCH_H

    bool isBranchM(long long op0);
    void executeBranch(uint32_t instruction, struct RegisterStore *registerStore);

    enum OffsetType {
        UNCONDITIONAL,
        CONDITIONAL,
        INVAL,
        REGISTER
    };

    enum BranchCondition {
        EQ = 0x0,
        NE = 0x1,
        GE = 0xA,
        LT = 0xB,
        GT = 0xC,
        LE = 0xD
    };
#endif //ARMV8_27_BRANCH_H
