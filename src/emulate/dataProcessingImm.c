#include "dataProcessingImm.h"

#include "fileUtils.h"
#include "flags.h"
#include "dataProcessing.h"

// Private functions declaration
static bool isArithmeticProcessing(long long opi);
static void executeArithmeticProcessingImm(uint32_t instruction, struct RegisterStore *registerStore);
static void executeWideMoveProcessing(uint32_t instruction, struct RegisterStore *registers);

/// Returns whether op0 corresponds with data processing (immediate)
bool isDataProcessingImm(long long op0) {
    long long match = 0x9;
    return (op0 | 0x1) == match;
}

/// Executes data processing (immediate) instructions
void executeDataProcessingImm(uint32_t instruction, struct RegisterStore *registers) {
    long long opi = (instruction >> 23) & 0x7;

    if (isArithmeticProcessing(opi)) {
        executeArithmeticProcessingImm(instruction, registers);
    } else {
        executeWideMoveProcessing(instruction, registers);
    }
}

/// Returns whether opi corresponds with arithmetic processing
static bool isArithmeticProcessing(long long opi) {
    long long match = 0x2;
    return opi == match;
}

/// Executes arithmetic processing instructions
static void executeArithmeticProcessingImm(uint32_t instruction, struct RegisterStore *registerStore) {
    uint32_t rd = instruction & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t sf = instruction >> 31;

    uint32_t imm12 = (instruction >> 10) & 0xFFF;
    bool sh = (instruction >> 22) & 0x1;

    uint32_t op = imm12;
    if (sh) {
        op = op << 12;
    }

    enum DpOpcArithmetic opc = (instruction >> 29) & 0x3;

    bool setsFlags = opc == ADDS || opc == SUBS;
    if (rn == 31 && !setsFlags) {
        return;
    }

    long long reg = loadFromRegister(rn, registerStore, true);

    long long res;
    bool multiplier = -1;
    if (opc == ADD || opc == ADDS) {
        res = reg + op;
        multiplier = 1;
    } else {
        res = reg - op;
    }

    // Complete the flags
    if (setsFlags) {
        registerStore->stateRegister->negativeFlag = res < 0;
        registerStore->stateRegister->zeroFlag = res == 0;

        bool V = overunderflow(reg, multiplier * op, res);
        bool C = carry(reg, op, opc == ADDS, sf); //ADDS
        registerStore->stateRegister->carryFlag = C;
        registerStore->stateRegister->overflowFlag = reg > 0 && res < 0;
    }

    storeToRegister(rd, res, registerStore, sf);
}

/// Executes wide move instructions
static void executeWideMoveProcessing(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t rd = instruction & 0x1F;
    uint32_t hw = (instruction >> 21) & 0x3;
    uint64_t imm16 = (instruction >> 5) & 0xFFFF;

    uint32_t shift = hw * 16;
    uint64_t op = imm16 << shift;
    bool sf = (instruction >> 31);

    enum DpOpcWideMove opc = (instruction >> 29) & 0x3;

    long long res = loadFromRegister(rd, registers, true);

    if (opc == MOVZ) {
        res = op;
    } else if (opc == MOVN) {
        res = ~op;
    } else if (opc == MOVK) {
        long long mask = 0xFFFF;
        res = res & ~(mask << shift);
        res = res | op;
    } else {
        // INVALID
        return;
    }

    storeToRegister(rd, res, registers, sf);
}
