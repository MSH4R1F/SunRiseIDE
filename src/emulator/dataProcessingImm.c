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

    enum DpOpcArithmetic opc = (instruction >> 29) & 0x3;

    long long reg = registers->zeroRegister;
    if (rn < 31) {
        reg = registers->registers[rn];
    }

    long long res;
    if (opc == ADD || opc == SUB) {
        res = reg + op;
    } else {
        res = reg - op;
    }

    // Complete the flags
    if (opc == ADDS || opc == SUBS) {
        registers->stateRegister.negativeFlag = res < 0;
        registers->stateRegister.zeroFlag = res == 0;
    }

    if (rd < 31) {
        registers->registers[rd] = res;
    }
}

void executeWideMoveProcessing(long long instruction, struct Registers *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t hw = (instruction >> 21) & 0x3;
    uint64_t imm16 = (instruction >> 5) & 0xFFFF;

    uint32_t shift = hw * 16;
    uint64_t op = imm16 << shift;

    uint32_t sf = (instruction >> 31);

    enum DpOpcWideMove opc = (instruction >> 29) & 0x3;

    long long res = registers->registers[rd];
    if (opc == MOVZ) {
        res = op;
    } else if (opc == MOVN) {
        res = ~op;
    } else {
        res = res & ~(0xFFFF << shift);
        res = res | (imm16 << shift);
    }

    if (sf == 0) {
        res = res & 0xFFFFFFFF;
    }

    if (rd < 31) {
        registers->registers[rd] = res;
    }
}
