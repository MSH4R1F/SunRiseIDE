#include "dataProcessingReg.h"

#include "bitwiseShift.h"
#include "memory.h"
#include "flags.h"
#include "dataProcessing.h"

// Private function declarations
static void executeArithmeticProcessingReg(uint32_t instruction, struct RegisterStore *registers);
static void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers);
static void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers);

/// Returns whether op0 corresponds with data processing (register)
bool isDataProcessingReg(long long op0) {
    long long match = 0xD;
    return (op0 | 0x8) == match;
}

/// Executes data processing (register) instruction
void executeDataProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t opr = (instruction >> 21) & 0xF;
    uint32_t m = (instruction >> 28) & 0x1;
    uint32_t check = (opr >> 3) + (2*m);
    if (check == 1) {
        executeArithmeticProcessingReg(instruction, registers);
    } else if (check == 0) {
        executeLogicProcessingReg(instruction, registers);
    } else {
        executeMultiplyProcessingReg(instruction, registers);
    }
}


static long long shiftFun(uint32_t shift, long long reg, uint32_t operand, bool sizeBit) {
    if (shift == 0) {
        return logicalLeftShift(reg, operand, sizeBit);
    } else if (shift == 1) {
        return logicalRightShift(reg, operand, sizeBit);
    } else if (shift == 2) {
        return arithmeticRightShift(reg, operand, sizeBit);
    } else {
        return rotateRight(reg, operand, sizeBit);
    }
}

/// Executes arithmetic processing (register) instruction
static void executeArithmeticProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    bool sf = instruction >> 31 & 0x1;
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x3F;

    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t rm = (instruction >> 16) & 0x1F;

    long long rn_val = loadFromRegister(rn, registers, sf);
    long long rm_val = loadFromRegister(rm, registers, sf);

    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t rd = instruction & 0x1F;

    long long op2 = shiftFun(shift, rm_val, operand, sf);
    int64_t multiplier = 1;
    if (opc >= 2) {
        multiplier = -1;
    }
    long long rd_val = rn_val + (multiplier * op2);
    bool signBit = rd_val >> 63;

    if (!sf) {
        signBit = rd_val >> 31 & 0x1;
    }

    if (opc == ADDS || opc == SUBS) {
        bool V = overunderflow(rn_val, multiplier * op2, rd_val);
        bool C = carry(rn_val, op2, multiplier == 1, sf);
        registers->stateRegister->negativeFlag = signBit;
        registers->stateRegister->zeroFlag = rd_val == 0;
        registers->stateRegister->carryFlag = C;
        registers->stateRegister->overflowFlag = V;
    }

    storeToRegister(rd, rd_val, registers, sf);
}

enum OpType {
    AND,
    BIC,
    ORR,
    ORN,
    EOR,
    EON,
    ANDS,
    BICS
};

/// Executes logic processing (register) instruction
static void executeLogicProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    uint32_t shift = (instruction >> 22) & 0x3;
    uint32_t N = (instruction >> 21) & 0x1;
    uint32_t opc = (instruction >> 29) & 0x3;
    uint32_t operand = (instruction >> 10) & 0x3F;

    bool sf = instruction >> 31;
    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t rm = (instruction >> 16) & 0x1F;

    long long rn_val = loadFromRegister(rn, registers, sf);
    long long rm_val = loadFromRegister(rm, registers, sf);

    rm_val = shiftFun(shift, rm_val, operand, sf);
    enum OpType combined_opc = (opc << 1) + N;

    uint32_t rd = instruction & 0x1F;
    long long rd_val = 0;
    switch (combined_opc) {
        case AND:
            rd_val = rn_val & rm_val;
            break;
        case BIC:
            rd_val = rn_val & ~rm_val;
            break;
        case ORR:
            rd_val = rn_val | rm_val;
            break;
        case ORN:
            rd_val = rn_val | ~rm_val;
            break;
        case EOR:
            rd_val = rn_val ^ rm_val;
            break;
        case EON:
            rd_val = rn_val ^ ~rm_val;
            break;
        case ANDS:
            rd_val = rn_val & rm_val;
            bool signBit = rd_val >> (31 + sf * 32);
            registers->stateRegister->negativeFlag = signBit;
            registers->stateRegister->zeroFlag = rd_val == 0;
            registers->stateRegister->carryFlag = false;
            registers->stateRegister->overflowFlag = false;
            break;
        case BICS:
            rd_val = rn_val & ~rm_val;
            bool signBit2 = (rd_val >> (31 + sf * 32)) & 1;
            registers->stateRegister->negativeFlag = signBit2;
            registers->stateRegister->zeroFlag = rd_val == 0;
            registers->stateRegister->carryFlag = false;
            registers->stateRegister->overflowFlag = false;
            break;
    }

    storeToRegister(rd, rd_val, registers, sf);
}

/// Executes multiply processing (register) instruction
static void executeMultiplyProcessingReg(uint32_t instruction, struct RegisterStore *registers) {
    bool sf = instruction >> 31;
    int x = ((instruction >> 15) & 0x1) == 0x1 ? -1 : 1;

    uint32_t rm = (instruction >> 16) & 0x1F;
    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t ra = (instruction >> 10) & 0x1F;
    uint32_t rd = instruction & 0x1F;

    long long rm_val = loadFromRegister(rm, registers, sf);
    long long rn_val = loadFromRegister(rn, registers, sf);
    long long ra_val = loadFromRegister(ra, registers, sf);
    long long rd_val = ra_val + x * (rn_val * rm_val);

    storeToRegister(rd, rd_val, registers, sf);
}