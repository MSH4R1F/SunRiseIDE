//
// Created by Faraz Malik on 29/05/2023.
//
#include <stdbool.h>
#include "dataProcessingImm.h"
#include "registers.c"

enum DpOpcArithmetic {
    ADD,
    ADDS,
    SUB,
    SUBS
};

enum DpOpcWideMove {
    MOVZ,
    INVALID,
    MOVN,
    MOVK
};

bool isDataProcessingImm(long long op0) {
    long long match = 0x9;
    return (op0 | 0x1) == match;
}
void executeDataProcessingImm(long long instruction, struct Registers *registers) {
    long long opi = (instruction >> 23) & 0x7;

    if (isArithmeticProcessing(opi)) {
        executeArithmeticProcessingImm(instruction, registers);
    } else {
        executeWideMoveProcessing(instruction, registers);
    }
}

bool isArithmeticProcessing(long long opi) {
    long long match = 0x2;
    return opi == match;
}
void executeArithmeticProcessingImm(long long instruction, struct Registers *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;

    uint32_t op = (instruction >> 10) & 0xFFF;
    uint32_t sh = (instruction >> 22) & 0x1;

    if (sh == 1) {
        op = op << 12;
    }

    enum DpOpcArithmetic operation = (instruction >> 29) & 0x3;

    long long reg;
    if (rn == 31) {
        reg = registers->zeroRegister;
    } else {
        reg = registers->registers[rn];
    }

    long long res;
    if (operation == ADD || operation == ADDS) {
        res = reg + op;
    } else {
        res = reg - op;
    }

    // Complete the flags
    if (operation == ADDS || operation == SUBS) {
        if (res < 0) {
            registers->stateRegister.negativeFlag = true;
        } else if (res == 0) {
            registers->stateRegister.zeroFlag = true;
        }
    }

    if (rn < 31) {
        registers->registers[rd] = res;
    }
}

void executeWideMoveProcessing(long long instruction, struct Registers *registers) {

}
