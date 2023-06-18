//
// Created by Faraz Malik on 18/06/2023.
//

#include <stdlib.h>
#include <stdio.h>

#ifndef ARMV8_27_PATTERNMATCH_H
    #define ARMV8_27_PATTERNMATCH_H

    typedef enum {
        B,
        BR
    } branchPattern;

    typedef enum {
        SHIFT_IMM,
        LS_12,
        DP_NEGS,
        DP_ADD,
        DP_CMP,
        DP_BITWISE,
        DP_TST,
        DP_MOVX,
        DP_MOV,
        DP_MVN,
        DP_MADD,
        DP_MUL
    } dpPattern;

    typedef enum {
        REG,
        XREG,
        SHIFT,
        IMM,
        LITERAL,
        SIMM
    } keyPattern;

    char *getKeyPattern(keyPattern key);
    char *getDpPattern(dpPattern key);
    char *getBranchPattern(branchPattern key);
#endif //ARMV8_27_PATTERNMATCH_H
